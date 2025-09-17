#pragma once

#include <mutex>
#include "refcore.h"           // Your RefCore (holds owner/destination txn)
#include "pinproxy.h"
#include "pgc_typecheck.h"
#include "pgc_transaction.h"
#include "pgc_promotion.h"
#include "pgc_base.h"
#include "promotioncapture.h"
#include "pgc_ready.h"

namespace PGC 
{
    
    template <typename T>
    class Ref : public Promotable 
    {
    public:
        // Optional convenience: set the destination owner up-front.
        explicit Ref(PGC_Transaction* p_OwnerTransaction = PGC_Transaction::TopTransaction()) noexcept
            : m_Core(p_OwnerTransaction)
        {
			PGC_User::GetCurrentPGC_User().RequireRegistered<T>("Ref<T> constructor");
        }

        // Optional convenience: construct with owner + initial pointer.
        Ref(T* ptr, PGC_Transaction* p_OwnerTransaction = PGC_Transaction::TopTransaction()) noexcept
			: m_Core(p_OwnerTransaction)
        {
            PGC_User::GetCurrentPGC_User().RequireRegistered<T>("Ref<T> constructor");
            Set(ptr);
        }

        // No custom copy/move for now (promotion rebinding is tricky).
        Ref(const Ref&) = delete;
        Ref& operator=(const Ref&) = delete;

        // Move constructor
        Ref(Ref&& p_Other) noexcept
			: Promotable([this](PGC_Transaction* dst) { this->RequestPromotion(dst); })
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

        ~Ref() noexcept
        {
            auto* destination = m_Core.GetLogicalOwnerTransaction();
			ASSERTM(destination, "Ref must have a destination transaction.");
            //std::lock_guard<std::mutex> g(destination->m_Mutex);
            m_Core.Clear(); // Clear frees any promotion and preserves the owner txn
        }

        // Move assignment
        Ref& operator=(Ref&& p_Other) noexcept
        {
            if (this == &p_Other) return *this;

            PGC_Transaction* destination = m_Core.GetLogicalOwnerTransaction();
            PGC_Transaction* otherDestination = p_Other.m_Core.GetLogicalOwnerTransaction();
            ASSERTM(destination, "Ref must have a destination transaction.");
            ASSERTM(otherDestination, "Other Ref must have a destination transaction.");

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
            return static_cast<T*>(m_Core.GetLogicalPointer());
        }

        // Set a new pointer. RefCore chooses promotion vs direct encode.
        // Atomically guarded by the destination (owner) transaction’s mutex.
        void Set(T* p_Pointer) {
            PGC_Transaction* destination = m_Core.GetLogicalOwnerTransaction();
            ASSERTM(destination, "Ref::Set: destination (owner) transaction is nullptr");

            std::lock_guard<std::mutex> lock(destination->m_Mutex);

			m_Core.Clear(); // Free any existing promotion.

            // Hand off to RefCore; it will encode directly or create a promotion
            // depending on whether p’s transaction matches dest.
            m_Core.SetLogicalPointer(static_cast<PGC_TypeCheck*>(p_Pointer));
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
            PGC_User::GetCurrentPGC_User().RequireRegistered<T>("RequestPromotion");
            m_Core.RequestPromotion(p_DestinationTransaction);
        }

    public:
        RefCore m_Core;
    };

} // namespace PGC
