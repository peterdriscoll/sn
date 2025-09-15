#pragma once

#include <mutex>
#include <type_traits>

#include "refcore.h"           // Your RefCore (holds owner/destination txn)
#include "pinproxy.h"
#include "pgc_typecheck.h"
#include "pgc_transaction.h"
#include "pgc_promotion.h"
#include "pgc_base.h"
#include "promotioncapture.h"
#include "pgc_ready.h"

// Primary: identity (assume data)
template<class T> struct DataOf { using type = T; };
template<class T> using DataOfT = typename DataOf<T>::type;

// Facade detector: true if T declares nested ::Data
template<class, class = void> struct has_nested_Data : std::false_type {};
template<class T>
struct has_nested_Data<T, std::void_t<typename std::remove_cv_t<T>::Data>> : std::true_type {};
template<class T> inline constexpr bool IsFacade_v = has_nested_Data<T>::value;

namespace PGC 
{
    template <typename T>
    class RefA : public Promotable 
    {
    public:
        using Data = DataOfT<T>;

        // Optional convenience: set the destination owner up-front.
        explicit RefA(PGC_Transaction* p_OwnerTransaction = PGC_Transaction::TopTransaction()) noexcept
            : m_Core(p_OwnerTransaction)
        {
			PGC_User::GetCurrentPGC_User()->RequireRegistered<T>("RefA<T> constructor");
        }

        // Optional convenience: construct with owner + initial pointer.
        RefA(T* ptr, PGC_Transaction* p_OwnerTransaction = PGC_Transaction::TopTransaction()) noexcept
			: m_Core(p_OwnerTransaction)
        {
            PGC_User::GetCurrentPGC_User()->RequireRegistered<T>("RefA<T> constructor");
            Set(ptr);
        }

        RefA(const RefA& p_Other) noexcept
            : Promotable([this](PGC_Transaction* dst) { this->RequestPromotion(dst); })
            , m_Core(p_Other.m_Core)
        {
            auto* user = PGC_User::GetCurrentPGC_User();
            user->with_lock(WithLock, [&] {
                m_Core.DetachPromotion();  // detach from other; we get our own copy
                if (PGC_Transaction* top = PGC_Transaction::TopTransaction(); top && !top->IsDying()) {
                    m_Core.RequestPromotion(top);
                }
            });
        }

        // Move constructor
        RefA(RefA&& p_Other) noexcept
            : Promotable([this](PGC_Transaction* dst) { this->RequestPromotion(dst); })
            , m_Core(p_Other.m_Core)
        {
            auto* user = PGC_User::GetCurrentPGC_User();
            user->with_lock(WithLock, [&] {
                PGC_Promotion* promotion = m_Core.DetachPromotion(); // detach from other, we get our own copy
                PGC_Transaction* topTransaction = PGC_Transaction::TopTransaction();
                if (!topTransaction->IsDying())
                {
                    if (promotion)
                    {
                        m_Core.SetLogicalPromotion(promotion);
                    }
                    else
                    {
                        m_Core.RequestPromotion(topTransaction);
                    }
                }
                p_Other.m_Core.Clear();
            });
        }

        ~RefA() noexcept
        {
            auto* user = PGC_User::GetCurrentPGC_User();
            user->with_lock(WithLock, [&] {
                m_Core.Clear(); // Free any promotion
            });
        }

        void Finalize() noexcept
        {
            auto* user = PGC_User::GetCurrentPGC_User();
            user->with_lock(WithLock, [&] {
                m_Core.Finalize();
            });
        }

        bool operator==(const RefA& p_Other)  const noexcept
        {
            return m_Core.GetLogicalPointer() == p_Other.m_Core.GetLogicalPointer();
        }

        // Move assignment
        RefA& operator=(RefA&& p_Other) noexcept
        {
            if (this == &p_Other) return *this;

            auto* user = PGC_User::GetCurrentPGC_User();
            user->with_lock(WithLock, [&] {
                if (!(*this == p_Other)) {
                    m_Core.Clear();  // free existing promotion (keep owner txn)

                    if (auto* promotion = p_Other.m_Core.DetachPromotion()) {
                        m_Core.SetLogicalPromotion(promotion);
                    }
                    else if (auto* pointer = p_Other.m_Core.GetLogicalPointer()) {
                        m_Core.SetLogicalPointer(pointer);
                    }

                    // cosmetic: make it obvious p_Other no longer refers to anything
                    p_Other.m_Core.Clear();
                }
            });
            return *this;
        }

        RefA& operator=(const RefA& p_Other) noexcept
        {
            if (this == &p_Other) return *this;

            auto* user = PGC_User::GetCurrentPGC_User();
            user->with_lock(WithLock, [&] {
                if (!(*this == p_Other)) {
                    m_Core.Clear();  // free existing promotion (keep owner txn)
                    m_Core.SetLogicalPointer(p_Other.m_Core.GetLogicalPointer());
                }
            });
            return *this;
        }

        // Return the current logical T* (after any promotion completes).
        T* Get() const {
			// taking const literally means we cannot modify the core.
            // This may be important for the web server access that reads
            // the state without modifying anything.
            auto* user = PGC_User::GetCurrentPGC_User();
            return user->with_lock(WithLock, [&]() -> T* {
                T* pointer = static_cast<T*>(m_Core.GetLogicalPointer());
                if (T* copy = static_cast<T*>(pointer->GetPromotedCopy()))
                {
                    pointer = copy;
                }
                return pointer;
            });
        }

        T* Get() 
        {
            // Fast path: if we’re under a promotion that’s already promoted,
            // collapse to the final copy and free the promotion now.
            auto* user = PGC_User::GetCurrentPGC_User();
            return user->with_lock(WithLock, [&]() -> T* {
                if (auto* promotion = m_Core.GetLogicalPromotion()) {
                    if (promotion->IsPromotedOrDropped()) {
                        auto* final_copy = promotion->GetFinalCopy(); // PGC_TypeCheck*
                        m_Core.SetLogicalPointer(final_copy);
                        promotion->FreeFromRefAttached();  // return promo to pool
                    }
                }
                T* pointer = static_cast<T*>(m_Core.GetLogicalPointer());
                if (T* copy = static_cast<T*>(pointer->GetPromotedCopy()))
                {
                    pointer = copy;
                }
                return pointer;
            });
        }

        // Set a new pointer. RefCore chooses promotion vs direct encode.
        // Atomically guarded by the destination (owner) transaction’s mutex.
        void Set(T* p_Pointer) 
        {
            auto* user = PGC_User::GetCurrentPGC_User();
            user->with_lock(WithLock, [&] {
                m_Core.Clear(); // free any existing promotion
                m_Core.SetLogicalPointer(static_cast<PGC_TypeCheck*>(p_Pointer));
            });
        }
        // A proxy that materializes a façade from a data handle.
        // It contains a façade VALUE; pointer stays valid to end of full expression.
        template<class Facade>
        struct Proxy {
            Facade f_;
            PGC::UnGuard unguard_;
            explicit Proxy(RefA<Facade> source)
                : f_(Facade(source, PGC::StackAllocationTag{}))
                , unguard_(PGC::PGC_User::GetCurrentPGC_User()->Unguard()) // drop lock for this expression
            {
            }
            Facade* operator->() { return &f_; }
            const Facade* operator->() const { return &f_; }
        };
         
        // ---- non-const ----
        template<class U = T> requires (IsFacade_v<U>)
        Proxy<U> operator->()
        {
            // façade path: NO pin; just pass a handle to data into façade
            return Proxy<U>(*this);
        }

        template<class U = T> requires (!IsFacade_v<U>)
        Pin<Data> operator->()
        {
            // data path: your existing borrow/pin semantics
            return Pin<Data>(Get());
        }

        // ---- const ----
        template<class U = T> requires (IsFacade_v<U>)
        const Proxy<U> operator->() const {
            return Proxy<U>(*this);
        }

        template<class U = T> requires (!IsFacade_v<U>)
        Pin<const Data> operator->() const {
            return Pin<const Data>(Get());
        }

        Pin<T> operator*()
        {
            return Pin<T>(Get());
        }

        ConstPin<T> operator*()  const 
        {
            return ConstPin<T>(Get()); 
        }

        Pin<T> Pinned()
        {
            return Pin<T>(Get());
        }

        ConstPin<T> Pinned() const 
        { 
            return ConstPin<T>(Get());
        }

        void PromoteNow()
        {
            m_Core.RequestPromotion(PGC_Transaction::TopTransaction());
        }

        void RequestPromotion(PGC_Transaction* dst = nullptr) requires PGC_Ready<T>
        {
            RequestPromotion(dst, WithLock);
        }

        /// Tag-dispatched: choose WithLock or NoLock at call site
        template<class Tag>
        void RequestPromotion(PGC_Transaction* dst, Tag tag) requires PGC_Ready<T>
        {
            PGC_User::GetCurrentPGC_User()->RequireRegistered<T>("RequestPromotion");

            auto* user = PGC_User::GetCurrentPGC_User();
            user->with_lock(tag, [&] {
                // RefCore does no locking; we're in the chosen policy here
                m_Core.RequestPromotion(dst);
                });
        }

        // Cheap emptiness check (no borrow, no promotion)
        bool empty() const noexcept {
            // Use whatever "is this bound?" check you already have.
            // Examples:
            // return m_Core.GetLogicalPointer() == nullptr;
            // return !m_Core.IsBound();
            return m_Core.GetLogicalPointer() == nullptr;
        }
        // Safe boolean conversion (C++11+)
        explicit operator bool() const noexcept
        { 
            return !empty();
        }

    public:
        RefCore m_Core;
    };

} // namespace PGC
