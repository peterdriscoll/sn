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
            std::fprintf(stderr, message.c_str());
            std::fflush(stderr);
#if defined(_MSC_VER) && !defined(NDEBUG)
            __debugbreak();
#endif
            std::abort();
        }
    }

    thread_local PGC_User* t_CurrentPGC_User = new PGC_User();

    /*static*/ PGC_User* PGC_User::GetCurrentPGC_User()
    {
        return t_CurrentPGC_User;
    }

    /*static*/ void PGC_User::SetCurrentPGC_User(PGC_User* p_User)
    {
        t_CurrentPGC_User = p_User;
    }

    PGC_User::PGC_User(const PGC::RegEntry p_ClassRegistry[], OnErrorHandler *p_ErrorHandler)
        : m_ErrorHandler(p_ErrorHandler)
        , m_TotalNetMemoryUsed(0)
        , m_TotalGrossMemoryUsed(0)
		, m_ProcessedRefAttachedMemory(0) 
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
        ResetProcessedRefAttachedMemory();
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
            + std::to_string(TotalProcessedRefAttachedMemory()) + " (double dipped) != "
            + std::to_string(TotalGrossMemoryUsed()) + " (gross)");

        // Clear all promotions (reset the free list)
        ClearAllPromotions();

        // Post-clear: verify that ClearAllPromotions did its job
        ASSERTM(
            PromotionFreeMemory() == 0,
            "Promotional free memory " + std::to_string(PromotionFreeMemory()) + " should be zero after clear");

        ASSERTM(
            TotalGrossMemoryUsed() == TotalProcessedRefAttachedMemory(),
            "Total gross memory used " + std::to_string(TotalGrossMemoryUsed()) +
            " should equal processed memory " + std::to_string(TotalProcessedRefAttachedMemory()) + " after clear");
    }

    //  Process the list
    //  * Promote items whos source is dieing transaction, and remove from the list.
    //  * Remove items from the list if the destination is a dieing transaction.
    //  Leave other items in the list for future promotion.
    void PGC_User::PromoteRequests()
    {
        std::scoped_lock lk(m_PromoteRequestsMutex);
        
        PGC_Promotion* promotion = nullptr;
        PGC_Promotion** last = nullptr;
        {
            std::scoped_lock lk(m_PromoteListMutex);

            promotion = m_PromoteList;
            last = &m_PromoteList;
        }
        while (promotion)
        {
            PromotionResult result = promotion->PromoteOrReject();
			PGC_Promotion* activePromotion = promotion;
            switch (result)
            {
            case PromotionResult::Dropped:
                {
                    std::scoped_lock lk(m_PromoteListMutex);
                    *last = promotion->m_Next;
                    promotion = *last;
                }
                activePromotion->FreeFromProcessingList();
                break;

            case PromotionResult::PromotedDone:
                {
                    std::scoped_lock lk(m_PromoteListMutex);
                    *last = promotion->m_Next;
                    promotion = *last;
                }
                FreePromotion(activePromotion);
                break;

            case PromotionResult::PromotedKeep:
                {
                    std::scoped_lock lk(m_PromoteListMutex);
                    *last = promotion->m_Next;
                    promotion = *last;
                }
                activePromotion->MarkPromoted();
                break;

            case PromotionResult::Keep:
                {
                    std::scoped_lock lk(m_PromoteListMutex);
                    last = &promotion->m_Next;
                    promotion = promotion->m_Next;
                }
                break;
            default:
                ASSERTM(false, "Unknown promotion result");
				break;  
            }
        }
        {
            std::scoped_lock lk(m_PromoteListMutex);
            m_PromoteListLast = last;
        }
    }

    PGC_Promotion* PGC_User::Allocate()
    {
        std::scoped_lock lk(m_FreeListMutex);

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

    void PGC_User::FreePromotion(PGC_Promotion* p_Promotion)
    {
        ASSERTM(p_Promotion, "Cannot free a null promotion");
#ifdef DEBUG_FREE
        ASSERTM(!FindInFreeList(p_Promotion), "Promotion already in the free list");
#endif
        std::scoped_lock lk(m_FreeListMutex);

        // Add to the free list
        if (p_Promotion) // Ensure p_Promotion is not null
        {
            p_Promotion->m_Next = m_FreeList;
            m_FreeList = p_Promotion;
            if (p_Promotion->IsPromotedOrDropped())
            {
                AddProcessedRefAttachedMemory(-static_cast<long>(sizeof(PGC_Promotion)));
            }
        }
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

    void PGC_User::AppendRequest(PGC_Promotion* p_Promotion)
    {
        ASSERTM(p_Promotion->Verify(), "Invalid promotion.");

        std::scoped_lock lk(m_PromoteListMutex);

        *m_PromoteListLast = p_Promotion;
        m_PromoteListLast = &(p_Promotion->m_Next);
        p_Promotion->m_Next = nullptr;
    }

    size_t PGC_User::TotalNetMemoryUsed() const
    {
        return m_TotalNetMemoryUsed;
    }

    size_t PGC_User::TotalGrossMemoryUsed() const
    {
        return m_TotalGrossMemoryUsed;
    }

    size_t PGC_User::TotalProcessedRefAttachedMemory() const
    {
        return m_ProcessedRefAttachedMemory;
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

    void PGC_User::ResetProcessedRefAttachedMemory()
    {
        m_ProcessedRefAttachedMemory = 0;
    }

    void PGC_User::AddProcessedRefAttachedMemory(long memory)
    {
        ASSERTM(memory >= 0 || m_ProcessedRefAttachedMemory >= static_cast<size_t>(-memory),
            "Processed ref attached memory would underflow: subtracting " + std::to_string(-memory) +
            " from " + std::to_string(m_ProcessedRefAttachedMemory));
        m_ProcessedRefAttachedMemory += memory;
    }

    size_t PGC_User::TotalPromotionMemory()
    {
        return PromotionUsedMemory() + PromotionFreeMemory() + TotalProcessedRefAttachedMemory();
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
//            return false;
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