#include "pgc_user.h"

#include "pgc_transaction.h"
#include "pgc_promotion.h"
#include <string>

#include "pgc_pch.h"

// If DEBUG_FREE is defined there is a check when freeing a promotion to see if
// the promotion is already in the free list.  This shouldnt happen.  
// So the definition of DEBUG_FREE may be ommitted for speed.
#define DEBUG_FREE

namespace PGC
{

    /*static*/ void PGC_User::DefaultErrorHandler(bool p_Err, const string& message)
    {
        if (p_Err)
        {
            ASSERTM(!p_Err, message);
        }
    }

    thread_local PGC_User* t_CurrentPGC_User = new PGC_User();

    /*static*/ PGC_User *PGC_User::GetCurrentPGC_User()
    {
        return t_CurrentPGC_User;
    }

    PGC_User::PGC_User(OnErrorHandler *p_ErrorHandler)
        : m_ErrorHandler(p_ErrorHandler)
        , m_TotalNetMemoryUsed(0)
        , m_TotalGrossMemoryUsed(0)
		, m_ProcessedDoubleDippingMemory(0) 
        , m_FreeList(nullptr)
        , m_PromoteList(nullptr)
        , m_PromoteListLast(&m_PromoteList)
    {
        Initialize();
    }

    PGC_User::~PGC_User()
    {
        Cleanup();
    }

    void PGC_User::Initialize()
    {
        t_CurrentPGC_User = this;

        // reset memory counters
        ResetNetMemoryUsed();
        ResetGrossMemoryUsed();
        ResetProcessedDoubleDippingMemory();
    }

    void PGC_User::Cleanup()
    {
        // Pre-clear memory integrity checks
        ASSERTM(
            TotalNetMemoryUsed() == 0,
            "Total net memory used " + to_string(TotalNetMemoryUsed()) + " not zero");

        ASSERTM(
            PromotionUsedMemory() == 0,
            "Memory used by promotions " + to_string(PromotionUsedMemory()) + " not zero");

        ASSERTM(TotalPromotionMemory() ==
            TotalGrossMemoryUsed(),
            "Promotion memory accounting mismatch: "
            + to_string(PromotionUsedMemory()) + " (used) + "
            + to_string(PromotionFreeMemory()) + " (free) + "
            + to_string(TotalProcessedDoubleDippingMemory()) + " (double dipped) != "
            + to_string(TotalGrossMemoryUsed()) + " (gross)");

        // Clear all promotions (reset the free list)
        ClearAllPromotions();

        // Post-clear: verify that ClearAllPromotions did its job
        ASSERTM(
            PromotionFreeMemory() == 0,
            "Promotional free memory " + to_string(PromotionFreeMemory()) + " should be zero after clear");

        ASSERTM(
            TotalGrossMemoryUsed() == TotalProcessedDoubleDippingMemory(),
            "Total gross memory used " + to_string(TotalGrossMemoryUsed()) +
            " should equal processed memory " + to_string(TotalProcessedDoubleDippingMemory()) + " after clear");
    }

    //  Process the list
    //  * Promote items whos source is dieing transaction, and remove from the list.
    //  * Remove items from the list if the destination is a dieing transaction.
    //  Leave other items in the list for future promotion.
        void PGC_User::PromoteRequests()
    {
        PGC_Promotion* promotion = m_PromoteList;
        PGC_Promotion** last = &m_PromoteList;
        while (promotion)
        {
            if (promotion->PromoteOrReject())
            {
                PGC_Promotion* next = promotion->m_Next;  // Must get next promotion after PromoteOrReject because it might change in the promotion.
                *last = next;			                  // Change the next pointer on the previous promotion to point ot the next promotion.

                // I think here is where the promotion is freed where it maybe shouldnt be.
                // Only free the promotion if the strategy was not DoubleDipping
                if (promotion->GetStrategy() != PromotionStrategy::DoubleDipping)
                {
                    promotion->Free();
                }
                else
                {
                    promotion->GetDestination()->GetUser()->AddProcessedDoubleDippingMemory(sizeof(PGC_Promotion));
                }
                promotion = next;		                  // Set the promotion for the next loop
            }
            else
            {
                last = &(promotion->m_Next);              // Record the last element (not removed from the list.
                promotion = promotion->m_Next;            // Set the promotion for the next loop
            }
        }
        m_PromoteListLast = last;
    }

    void PGC_User::AppendRequest(PGC_Promotion *p_Promotion)
    {
        *m_PromoteListLast = p_Promotion;
        m_PromoteListLast = &(p_Promotion->m_Next);
        p_Promotion->m_Next = nullptr;
    }

    PGC_Promotion* PGC_User::Allocate()
    {
        PGC_Promotion* result = m_FreeList;
        if (result)
        {
            m_FreeList = m_FreeList->m_Next;
        }
        else
        {
            AddTotalGrossMemorySize(sizeof(PGC_Promotion));
            result = new PGC_Promotion();
        }
        result->m_Next = nullptr;
        return result;
    }

    // Used to check if an item already in the free list is being re-added.
    // This shouldnt happen.
    bool PGC_User::FindInFreeList(PGC_Promotion* p_Promotion)
    {
        PGC_Promotion* promotion = m_FreeList;
        while (promotion)
        {
            if (promotion == p_Promotion)
            {
                return true;
            }
            promotion = promotion->m_Next;
        }
        return false;
    }

    void PGC_User::FreePromotion(PGC_Promotion* p_Promotion)
    {
        ASSERTM(p_Promotion, "Cannot free a null promotion");
#ifdef DEBUG_FREE
        ASSERTM(!FindInFreeList(p_Promotion), "Promotion already in the free list");
#endif

        // Add to the free list
        p_Promotion->m_Next = m_FreeList;
        m_FreeList = p_Promotion;
	}

    size_t PGC_User::TotalNetMemoryUsed() const
    {
        return m_TotalNetMemoryUsed;
    }

    size_t PGC_User::TotalGrossMemoryUsed() const
    {
        return m_TotalGrossMemoryUsed;
    }

    size_t PGC_User::TotalProcessedDoubleDippingMemory() const
    {
        return m_ProcessedDoubleDippingMemory;
    }

    void PGC_User::AddTotalNetMemorySize(long memory)
    {
        if (!(memory >= 0 || m_TotalNetMemoryUsed >= static_cast<size_t>(-memory)))
        {
            long dog = 10;
        }
        ASSERTM(memory >= 0 || m_TotalNetMemoryUsed >= static_cast<size_t>(-memory),
            "Total net memory would underflow: subtracting " + std::to_string(-memory) +
            " from " + std::to_string(m_TotalNetMemoryUsed));
        m_TotalNetMemoryUsed += memory;
    }

    void PGC_User::AddTotalGrossMemorySize(long memory)
    {
        ASSERTM(memory >= 0 || m_TotalGrossMemoryUsed >= static_cast<size_t>(-memory),
            "Total gross memory would underflow: subtracting " + std::to_string(-memory) +
            " from " + std::to_string(m_TotalGrossMemoryUsed));
        m_TotalGrossMemoryUsed += memory;
    }

    void PGC_User::ResetNetMemoryUsed()
    {
        m_TotalNetMemoryUsed = 0;
    }

    void PGC_User::ResetGrossMemoryUsed()
    {
        m_TotalGrossMemoryUsed = PromotionFreeMemory();
    }

    void PGC_User::ResetProcessedDoubleDippingMemory()
    {
        m_ProcessedDoubleDippingMemory = 0;
    }

    void PGC_User::AddProcessedDoubleDippingMemory(long memory)
    {
        ASSERTM(memory >= 0 || m_ProcessedDoubleDippingMemory >= static_cast<size_t>(-memory),
            "Processed double-dipping memory would underflow: subtracting " + std::to_string(-memory) +
            " from " + std::to_string(m_ProcessedDoubleDippingMemory));
        m_ProcessedDoubleDippingMemory += memory;
    }

    size_t PGC_User::TotalPromotionMemory()
    {
        return PromotionUsedMemory() + PromotionFreeMemory() + TotalProcessedDoubleDippingMemory();
    }

    size_t PGC_User::PromotionFreeMemory()
    {
        size_t memory = 0;
        PGC_Promotion* promotion = m_FreeList;
        while (promotion)
        {
            memory += sizeof(PGC_Promotion);
            promotion = promotion->m_Next;
        }
        return memory;
    }

    size_t PGC_User::PromotionUsedMemory()
    {
        size_t memory = 0;
        PGC_Promotion* promotion = m_PromoteList;
        while (promotion)
        {
            memory += sizeof(PGC_Promotion);
            promotion = promotion->m_Next;
        }
        return memory;
    }


    void PGC_User::ClearAllPromotions()
    {
        // Walk through the free list and delete everything
        while (m_FreeList)
        {
            PGC_Promotion* next = m_FreeList->m_Next;

            AddTotalGrossMemorySize(-static_cast<long>(sizeof(PGC_Promotion)));

            delete m_FreeList;

            m_FreeList = next;
        }
    }
}