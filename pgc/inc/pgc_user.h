#pragma once

#include "exp_ctrl_pgc.h"
#include "pgc_base.h"
#include "pgc_promotion.h"
#include <string>

namespace PGC
{
    // Base class for any session/user that needs automatic promotion cleanup and state checks
    class PGC_EXPORT PGC_User
    {
    public:
        // Default error handler implementation
        // This can be overridden by the user if needed
        static void DefaultErrorHandler(bool p_Err, const std::string& message);
        
        static PGC_User* GetCurrentPGC_User();

        // Constructor and destructor
        PGC_User(OnErrorHandler *p_ErrorHandler = DefaultErrorHandler);
        virtual ~PGC_User();

        // Optional explicit calls (if needed)
        void Initialize();
        void Cleanup();

        OnErrorHandler* GetErrorHandler() const
        {
            return m_ErrorHandler;
		}

        void ClearAllPromotions();
        void PromoteRequests();

        PGC_Promotion* Allocate();
        void AppendRequest(PGC_Promotion*);

        bool FindInFreeList(PGC_Promotion* p_Promotion);
        void FreePromotion(PGC_Promotion* p_Promotion);

        size_t TotalNetMemoryUsed() const;
        size_t TotalGrossMemoryUsed() const;
        size_t TotalProcessedDoubleDippingMemory() const;

        void AddTotalNetMemorySize(long memory);
        void AddTotalGrossMemorySize(long memory);
        void AddProcessedDoubleDippingMemory(long memory);

        void ResetNetMemoryUsed();
        void ResetGrossMemoryUsed();
        void ResetProcessedDoubleDippingMemory();

        size_t PromotionFreeMemory();
        size_t PromotionUsedMemory();
        size_t TotalPromotionMemory();

    private:
        OnErrorHandler *m_ErrorHandler;
    
        PGC_Promotion* m_FreeList;
        PGC_Promotion* m_PromoteList;
        PGC_Promotion** m_PromoteListLast;

        size_t m_TotalNetMemoryUsed;
        size_t m_TotalGrossMemoryUsed;
        size_t m_ProcessedDoubleDippingMemory;
    };
}