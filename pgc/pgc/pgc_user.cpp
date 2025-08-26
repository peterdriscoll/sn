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

    /*static*/ void PGC_User::DefaultErrorHandler(bool p_Err, const std::string& message)
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

    PGC_User::PGC_User(const PGC::RegEntry p_ClassRegistry[], OnErrorHandler *p_ErrorHandler)
        : m_ErrorHandler(p_ErrorHandler)
        , m_TotalNetMemoryUsed(0)
        , m_TotalGrossMemoryUsed(0)
		, m_ProcessedDoubleDippingMemory(0) 
        , m_FreeList(nullptr)
        , m_PromoteList(nullptr)
        , m_PromoteListLast(&m_PromoteList)
    {
        Initialize();
        if (p_ClassRegistry)
        {
            // Build set; stop at sentinel (name == nullptr)
            for (auto p = p_ClassRegistry; p->name != nullptr; ++p)
            {
                // (optional) detect duplicates and mismatched names
                auto [it, inserted] = m_registered.insert(p->type);
                ASSERTM(inserted, "Duplicate class registration " + std::string(p->name));
            }
        }
    }

    PGC_User::~PGC_User()
    {
		t_CurrentPGC_User = m_LastPGC_User;
        Cleanup();
    }

    void PGC_User::Initialize()
    {
		m_LastPGC_User = t_CurrentPGC_User;
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
            "Total net memory used " + std::to_string(TotalNetMemoryUsed()) + " not zero");

        ASSERTM(
            PromotionUsedMemory() == 0,
            "Memory used by promotions " + std::to_string(PromotionUsedMemory()) + " not zero");

        ASSERTM(TotalPromotionMemory() ==
            TotalGrossMemoryUsed(),
            "Promotion memory accounting mismatch: "
            + std::to_string(PromotionUsedMemory()) + " (used) + "
            + std::to_string(PromotionFreeMemory()) + " (free) + "
            + std::to_string(TotalProcessedDoubleDippingMemory()) + " (double dipped) != "
            + std::to_string(TotalGrossMemoryUsed()) + " (gross)");

        // Clear all promotions (reset the free list)
        ClearAllPromotions();

        // Post-clear: verify that ClearAllPromotions did its job
        ASSERTM(
            PromotionFreeMemory() == 0,
            "Promotional free memory " + std::to_string(PromotionFreeMemory()) + " should be zero after clear");

        ASSERTM(
            TotalGrossMemoryUsed() == TotalProcessedDoubleDippingMemory(),
            "Total gross memory used " + std::to_string(TotalGrossMemoryUsed()) +
            " should equal processed memory " + std::to_string(TotalProcessedDoubleDippingMemory()) + " after clear");
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
            PromotionResult result = promotion->PromoteOrReject();
            switch (result)
            {
            case PromotionResult::Dropped:
                *last = promotion->m_Next;
                FreePromotion(promotion);
                promotion = *last;
                break;

            case PromotionResult::PromotedDone:
                *last = promotion->m_Next;
                FreePromotion(promotion);
                promotion = *last;
                break;

            case PromotionResult::PromotedKeep:
                *last = promotion->m_Next;
                promotion->MarkPromoted();
                promotion = *last;
                break;

            case PromotionResult::Keep:
                last = &promotion->m_Next;
                promotion = promotion->m_Next;
                break;
            default:
                ASSERTM(false, "Unknown promotion result");
				break;  
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
        if (p_Promotion) // Ensure p_Promotion is not null
        {
            p_Promotion->m_Next = m_FreeList;
            m_FreeList = p_Promotion;
            if (p_Promotion->IsPromoted())
            {
                AddProcessedDoubleDippingMemory(-static_cast<long>(sizeof(PGC_Promotion)));
            }
        }
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

    bool PGC_User::ShouldRaiseError()
    {
        if (m_ErrorRaised)
        {
            return false;
        }
        m_ErrorRaised = true;
        return true;
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