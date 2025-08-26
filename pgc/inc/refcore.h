#pragma once
#include "pgc_typecheck.h" 
#include "pgc_base.h" 
#include "pgc_transaction.h" 
#include "pgc_promotion.h"
#include "pgc_promotionstrategy.h"
#include "pgc_user.h"

// Defining if checking the encoding of three logical values in one.
#define PGC_DEBUG_VERIFY_ENCODING

namespace PGC
{
    // This is the core of the PGC::Ref class. It implements and
   // checks Logical pointer, transaction, and promotion stored
   // in one internal pointer. 
    class RefCore
    {
        // --- Encoded pointer (the 3-in-1) ---
        PGC_TypeCheck* m_InternalPointer = nullptr;

    #ifdef PGC_DEBUG_VERIFY_ENCODING
        // --- Debug shadows (optional, for inspection) ---
        // The idea is to check that they are in sync, not enforce
        // it. There are two implementations. The efficient complex
        // one using the one internal pointer, and the simple one 
        // where the infomation is separated into 3 simple variables
        // for the pointer, transaction and promotion. We aim to 
        // check that the complex implementation works the same as 
        // the simple one.
        PGC_TypeCheck* m_LogicalPointer = nullptr;
        PGC_Transaction* m_LogicalOwnerTransaction = nullptr;
        PGC_Promotion* m_LogicalPromotion = nullptr;
    #endif

        private:
            RefCore() = delete;

            RefCore(RefCore&& p_Other) noexcept
                : m_InternalPointer(p_Other.m_InternalPointer)
#ifdef PGC_DEBUG_VERIFY_ENCODING
                , m_LogicalPointer(p_Other.m_LogicalPointer)
                , m_LogicalOwnerTransaction(p_Other.m_LogicalOwnerTransaction)
                , m_LogicalPromotion(p_Other.m_LogicalPromotion)
#endif
            {
            }

            RefCore& operator=(RefCore&& p_Other) noexcept
            {
                m_InternalPointer = p_Other.m_InternalPointer;
#ifdef PGC_DEBUG_VERIFY_ENCODING
                m_LogicalPointer = p_Other.m_LogicalPointer;
                m_LogicalOwnerTransaction = p_Other.m_LogicalOwnerTransaction;
                m_LogicalPromotion = p_Other.m_LogicalPromotion;
#endif
            }

            // ------------------------------------------------------------------------
        // Promotion factory / reuse hook
        // Implement in your PGC layer. This stub just shows intent.
        // ------------------------------------------------------------------------
        PGC_Promotion* CreatePromotion(PGC_TypeCheck* p_Pointer, PGC_Transaction* p_DestinationTransaction)
        {
            PGC_User* user = GetLogicalOwnerTransaction()->GetUser();
            PGC_Promotion* promotion = user->Allocate();
            promotion->Create(&m_InternalPointer, p_DestinationTransaction, PromotionStrategy::DoubleDipping);
            user->AppendRequest(promotion);
            return promotion;
        }

    public:
        // If you want "constructor-only transaction", provide a ctor that sets it.
        explicit RefCore(PGC_Transaction* p_OwnerTransaction)
        {
            SetLogicalTransaction(p_OwnerTransaction);
        }

        void Clear()
        {
            // Cache what is needed *before* clobbering
            auto* promotion = GetLogicalPromotion();
            PGC_Transaction *ownerTransaction = GetLogicalOwnerTransaction();

            // Make this ref an "empty slot owned by owner"
            // (your encoding: internal == owner means null)
            m_InternalPointer = ownerTransaction;
        #ifdef PGC_DEBUG_VERIFY_ENCODING
            m_LogicalOwnerTransaction = ownerTransaction;
            m_LogicalPromotion = nullptr;
            m_LogicalPointer = nullptr;
        #endif

			// Free the promotion if there was one.
            if (promotion)
            {
                promotion->Free(); // Promotion no longer needed.
            }

        #ifdef PGC_DEBUG_VERIFY_ENCODING
            Verify();
        #endif
        }

        PGC_Promotion* DetachPromotion()
        {
			PGC_Promotion* promotion = GetLogicalPromotion();
            if (promotion)
            {
				SetLogicalPointer(promotion->GetFinalCopy());  
            #ifdef PGC_DEBUG_VERIFY_ENCODING
                m_LogicalPromotion = nullptr;  // Clear the shadow reference
            #endif
            }
            return promotion;
        }

        // ------------------------------------------------------------------------
        // Getters: delegate to the encoded pointer (or null if unset)
        // ------------------------------------------------------------------------
        PGC_TypeCheck* GetLogicalPointer() const
        {
            return m_InternalPointer ? m_InternalPointer->GetLogicalPointer() : nullptr;
        }

        PGC_Transaction* GetLogicalOwnerTransaction() const
        {
            return m_InternalPointer ? m_InternalPointer->GetLogicalOwnerTransaction() : nullptr;
        }

        PGC_Promotion* GetLogicalPromotion() const
        {
            return m_InternalPointer ? m_InternalPointer->GetLogicalPromotion() : nullptr;
        }

        // Verify invariants using only the virtual API.
    #ifdef PGC_DEBUG_VERIFY_ENCODING
        void Verify() const {
            // Shadows must match recomputation
            ASSERTM(m_LogicalPointer == GetLogicalPointer(), "Logical pointer mismatch");
            ASSERTM(m_LogicalOwnerTransaction == GetLogicalOwnerTransaction(), "Logical owner transaction mismatch");
            ASSERTM(m_LogicalPromotion == GetLogicalPromotion(), "Logical promotion mismatch");
        }
    #endif
    
        // ------------------------------------------------------------------------
        // Setters
        // ------------------------------------------------------------------------

        // Set as the logical pointer.
        // If its owner matches our current logical owner, encode directly.
        // Otherwise, encode a promotion to the (current) owner.
        void SetLogicalPointer(PGC_TypeCheck* p_Pointer) {
#ifdef PGC_DEBUG_VERIFY_ENCODING
            m_LogicalPointer = p_Pointer;
#endif
            if (p_Pointer == nullptr)
            {
                m_InternalPointer = GetLogicalOwnerTransaction();
            }
            else
            { 
                PGC_Transaction* sourceTransaction = p_Pointer->GetTransaction();
                PGC_Transaction* destinationTransaction = GetLogicalOwnerTransaction();
                m_InternalPointer = p_Pointer;
                if (sourceTransaction != destinationTransaction)
                {   // Different transaction, create a promotion that
                    // promises to move the instance from the source transaction
                    // to this transaction when the source transaction ends,.
                    SetLogicalPromotion(CreatePromotion(p_Pointer, destinationTransaction));
                }
            }

        #ifdef PGC_DEBUG_VERIFY_ENCODING
            Verify();
        #endif
        }

        // Set a *promotion* as the encoded value.
        void SetLogicalPromotion(PGC_Promotion* p_Promotion) {
            ASSERTM(p_Promotion != nullptr, "SetLogicalPromotion: null promotion");
#ifdef PGC_DEBUG_VERIFY_ENCODING
            m_LogicalPromotion = p_Promotion;
#endif

            if (p_Promotion->GetBaseAddress() != &m_InternalPointer)
            {
                // If the promotion base is not bound to this address, fix it.
                p_Promotion->Rebind(&m_InternalPointer, m_InternalPointer->GetLogicalOwnerTransaction());
			}
            m_InternalPointer = p_Promotion;

        #ifdef PGC_DEBUG_VERIFY_ENCODING
            Verify();
        #endif
        }

        // Set the *owner transaction* as the encoded value.
        // As per your rule: only valid in constructor-time setup.
        void SetLogicalTransaction(PGC_Transaction* p_Transaction) {
            ASSERTM(p_Transaction != nullptr, "SetLogicalTransaction: null transaction");
            ASSERTM(m_InternalPointer == nullptr, "Can only set transaction in constructor");

            m_InternalPointer = p_Transaction;

        #ifdef PGC_DEBUG_VERIFY_ENCODING
            m_LogicalOwnerTransaction = p_Transaction;
            Verify();
        #endif
        }

        void RequestPromotion()
        {
            if (!GetLogicalPointer() || GetLogicalPromotion())
            {
				return;
            }

            PGC_Transaction* sourceTransaction = GetLogicalPointer()->GetTransaction();
            PGC_Transaction* destinationTransaction = GetLogicalOwnerTransaction();

            ASSERTM(sourceTransaction, "Must have a source transaction.");
            ASSERTM(sourceTransaction, "Must have a destination transaction.");

            PGC_TypeCheck* typeCheck = PGC_Promotion::CheckRequestPromotion(
                &m_InternalPointer, sourceTransaction, destinationTransaction, PromotionStrategy::DoubleDipping);
            if (typeCheck != m_InternalPointer)
            {
                m_InternalPointer = typeCheck;
            }
        }
    };

}
