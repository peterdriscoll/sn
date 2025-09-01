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
            , m_Core(PGC_Transaction::TopTransaction())
        {
            PGC_Transaction* destination = m_Core.GetLogicalOwnerTransaction();
            PGC_Transaction* otherDestination = p_Other.m_Core.GetLogicalOwnerTransaction();
            ASSERTM(destination, "Reference must have a destination transaction.");
            ASSERTM(otherDestination, "Other Reference must have a destination transaction.");
            std::lock_guard<std::mutex> g(destination->m_Mutex);

            // Fix later
            auto* pointer = p_Other.m_Core.GetLogicalPointer();
            m_Core.SetLogicalPointer(pointer);
        }

        // Move constructor
        RefA(RefA&& p_Other) noexcept
			: Promotable(std::bind_front(&RefA<T>::RequestPromotion, this))
            , m_Core(PGC_Transaction::TopTransaction())
        {
            PGC_Transaction* destination = m_Core.GetLogicalOwnerTransaction();
            PGC_Transaction* otherDestination = p_Other.m_Core.GetLogicalOwnerTransaction();
            ASSERTM(destination, "Reference must have a destination transaction.");
            ASSERTM(otherDestination, "Other Reference must have a destination transaction.");
            std::lock_guard<std::mutex> g(destination->m_Mutex);

            auto* promotion = p_Other.m_Core.DetachPromotion();
            if (promotion)
			{   // Since we are moving, we can reuse the promotion, rebind it to the new
                // address. RefCore will rebind(&m_InternalPointer, GetLogicalOwnerTransaction())
                // so that the promotion uses the new address.
                m_Core.SetLogicalPromotion(promotion);
            }
            else
            {
                auto* pointer = p_Other.m_Core.GetLogicalPointer();
                m_Core.SetLogicalPointer(pointer);
            }
            p_Other.m_Core.Clear();
        }

        ~RefA() noexcept
        {
            auto* destination = m_Core.GetLogicalOwnerTransaction();
			ASSERTM(destination, "RefA must have a destination transaction.");
            std::lock_guard<std::mutex> g(destination->m_Mutex);
            m_Core.Clear(); // Clear frees any promotion and preserves the owner txn
        }

        // Move assignment
        RefA& operator=(RefA&& p_Other) noexcept
        {
            if (this == &p_Other) return *this;

            PGC_Transaction* destination = m_Core.GetLogicalOwnerTransaction();
            PGC_Transaction* otherDestination = p_Other.m_Core.GetLogicalOwnerTransaction();
            ASSERTM(destination, "RefA must have a destination transaction.");
            ASSERTM(otherDestination, "Other RefA must have a destination transaction.");

            // Phase 1: under our current destination lock, free any existing promotion.
            std::lock_guard<std::mutex> g1(destination->m_Mutex);
            m_Core.Clear(); // Clear will free any promotion, but keep the owner transaction
            if (auto* promotion = p_Other.m_Core.DetachPromotion())
            {
                // RefCore will rebind promotion address and destination transaction to this ref and retarget to finalDest
                m_Core.SetLogicalPromotion(promotion);
            }
            else if (auto* pointer = p_Other.m_Core.GetLogicalPointer())
            {
                // RefCore will direct-encode or create a promotion to finalDest
                m_Core.SetLogicalPointer(pointer);
            }

            p_Other.m_Core.Clear(); // This won't free the promotion, as it has been detached.

            return *this;
        }

        RefA& operator=(RefA& p_Other) noexcept
        {
            if (this == &p_Other) return *this;

            PGC_Transaction* destination = m_Core.GetLogicalOwnerTransaction();
            PGC_Transaction* otherDestination = p_Other.m_Core.GetLogicalOwnerTransaction();
            ASSERTM(destination, "RefA must have a destination transaction.");
            ASSERTM(otherDestination, "Other RefA must have a destination transaction.");

            // Phase 1: under our current destination lock, free any existing promotion.
            std::lock_guard<std::mutex> g1(destination->m_Mutex);
            m_Core.Clear(); // Clear will free any promotion, but keep the owner transaction
            if (auto* promotion = p_Other.m_Core.DetachPromotion())
            {
                // RefCore will rebind promotion address and destination transaction to this ref and retarget to finalDest
                m_Core.SetLogicalPromotion(promotion);
            }
            else if (auto* pointer = p_Other.m_Core.GetLogicalPointer())
            {
                // RefCore will direct-encode or create a promotion to finalDest
                m_Core.SetLogicalPointer(pointer);
            }

            p_Other.m_Core.Clear(); // This won't free the promotion, as it has been detached.

            return *this;
        }

        // Return the current logical T* (after any promotion completes).
        const T* Get() const {
			// taking const literally means we cannot modify the core.
            // This may be important for the web server access that reads
            // the state without modifying anything.
            return static_cast<T*>(m_Core.GetLogicalPointer());
        }

        T* Get() {
            // Fast path: if we’re under a promotion that’s already promoted,
            // collapse to the final copy and free the promotion now.
            if (auto* promotion = m_Core.GetLogicalPromotion()) {
                if (promotion->IsPromotedOrDropped()) {
                    // Move the core to the final copy so we stop pointing at the promo.
                    auto* final_copy = promotion->GetFinalCopy(); // PGC_TypeCheck*
                    m_Core.SetLogicalPointer(final_copy);
                    promotion->FreeFromRefAttached(); // return the promotion to the pool
                }
            }
            return dynamic_cast<T*>(m_Core.GetLogicalPointer());
        }

        // Set a new pointer. RefCore chooses promotion vs direct encode.
        // Atomically guarded by the destination (owner) transaction’s mutex.
        void Set(T* p_Pointer) {
            PGC_Transaction* destination = m_Core.GetLogicalOwnerTransaction();
            ASSERTM(destination, "RefA::Set: destination (owner) transaction is nullptr");

            std::lock_guard<std::mutex> lock(destination->m_Mutex);

			m_Core.Clear(); // Free any existing promotion.

            // Hand off to RefCore; it will encode directly or create a promotion
            // depending on whether p’s transaction matches dest.
            m_Core.SetLogicalPointer(static_cast<PGC_TypeCheck*>(p_Pointer));
        }
        // A proxy that materializes a façade from a data handle.
        // It contains a façade VALUE; pointer stays valid to end of full expression.
        template<class Facade>
        struct Proxy {
            Facade f_;
            explicit Proxy(RefA<Facade> &source)
                : f_(Facade(source)) {
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
        Proxy<const U> operator->() const {
            return Proxy<const U>(this->as_data());
        }

        template<class U = T> requires (!IsFacade_v<U>)
        Pin<const Data> operator->() const {
            return Pin<const Data>(Get());
        }

        // Accessors with typical pointer ergonomics.
        Pin<T> operator->()
        {
            return Pin<T>(Get());
        }

        ConstPin<T> operator->() const
        {
            return ConstPin<T>(Get()); 
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
            m_Core.RequestPromotion();
        }
		void RequestPromotion(PGC_Transaction* p_DestinationTransaction = nullptr) requires PGC_Ready<T>
        {
            PGC_User::GetCurrentPGC_User()->RequireRegistered<T>("RequestPromotion");
            m_Core.RequestPromotion(p_DestinationTransaction);
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
        explicit operator bool() const noexcept { return !empty(); }

    private:
        RefCore m_Core;
    };

} // namespace PGC
