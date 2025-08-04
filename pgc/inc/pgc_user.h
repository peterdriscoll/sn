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

    private:
        OnErrorHandler *m_ErrorHandler;
        static PGC_User* g_PGC_User;
    };
}