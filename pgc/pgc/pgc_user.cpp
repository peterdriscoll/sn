#include "pgc_user.h"

#include "pgc_transaction.h"
#include "pgc_promotion.h"
#include <string>

#include "pgc_pch.h"

namespace PGC
{

    /*static*/ void PGC_User::DefaultErrorHandler(bool p_Err, const string& message)
    {
        if (p_Err)
        {
            ASSERTM(!p_Err, message);
        }
    }
    PGC_User *PGC_User::g_PGC_User;

    /*static*/ PGC_User *PGC_User::GetCurrentPGC_User()
    {
        return g_PGC_User;
    }

    PGC_User::PGC_User(OnErrorHandler *p_ErrorHandler)
    {
        m_ErrorHandler = p_ErrorHandler;
        Initialize();
    }

    PGC_User::~PGC_User()
    {
        Cleanup();
    }

    void PGC_User::Initialize()
    {
        // reset memory counters
        PGC_Transaction::ResetNetMemoryUsed();
        PGC_Transaction::ResetGrossMemoryUsed();
    }

    void PGC_User::Cleanup()
    {
        // Memory checks
        if (PGC_Transaction::TotalNetMemoryUsed() != 0)
        {
            m_ErrorHandler(PGC_Transaction::TotalNetMemoryUsed() == 0, "Total net memory used " + to_string(PGC_Transaction::TotalNetMemoryUsed()) + "not zero");
        }
        if (PGC_Promotion::PromotionUsedMemory() != 0)
        {
            m_ErrorHandler(PGC_Promotion::PromotionUsedMemory() == 0, "Memory used by promotions " + to_string(PGC_Promotion::PromotionUsedMemory()) + " not zero");
        }
        if (PGC_Promotion::PromotionFreeMemory() != PGC_Transaction::TotalGrossMemoryUsed())
        {
            m_ErrorHandler(PGC_Promotion::PromotionFreeMemory() == PGC_Transaction::TotalGrossMemoryUsed(), "Promotional free memory " + to_string(PGC_Promotion::PromotionFreeMemory()) + " != " + to_string(PGC_Transaction::TotalGrossMemoryUsed()));
        }

        // Clear all promotions before checking memory stats
        PGC_Promotion::ClearAllPromotions();

        if (PGC_Promotion::PromotionFreeMemory() != 0)
        {
            m_ErrorHandler(PGC_Promotion::PromotionFreeMemory() == 0, "Promotional free memory " + to_string(PGC_Promotion::PromotionFreeMemory()) + " should be zero after free");
        }

        if (PGC_Transaction::TotalGrossMemoryUsed() != 0)
        {
            m_ErrorHandler(PGC_Transaction::TotalGrossMemoryUsed() != 0, "Total gross memory used " + to_string(PGC_Transaction::TotalGrossMemoryUsed()) + " should bee zero after free");
        }
    }
}