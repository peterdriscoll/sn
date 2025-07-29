#include "sn_deferredcommand.h"
#include <iostream>
#include "sni_thread.h"
#include "sni_manager.h"

#include "sn_pch.h"

namespace SN {

    SN_DeferredCommand::SN_DeferredCommand(const SN_Expression& expr)
        : m_Expression(expr.GetSNI_Expression()), m_Done(false)
    {}

    __declspec(noinline) void SN_DeferredCommand::Do() noexcept(false)
    {
        m_Done = true;
        return m_Expression->Do();
    }

    __declspec(noinline) SN_Error SN_DeferredCommand::DoReturnError()
    {
        m_Done = true;
        return m_Expression->DoReturnError();
    }

    void SN_DeferredCommand::Throw() noexcept(false)
    {
        m_Done = true;
        return m_Expression->Throw();
    }

    SNI::SNI_Expression* SN_DeferredCommand::GetSNI_Expression()
    {
        m_Done = true;
        return m_Expression;
    }

    SN_DeferredCommand::~SN_DeferredCommand() {
        if (!m_Done) {
            OnErrorHandler* handler = SNI::SNI_Thread::TopManager()->ErrorHandler();
            string err_msg = "ERROR: symbolic command created but not executed (missing .Do())";
            if (handler) {
                SNI::SNI_DisplayOptions l_DisplayOptions(doTextOnly);
                SN_Error e(false, false, err_msg + m_Expression->DisplayValueSN(0, l_DisplayOptions));
                handler(e.IsError(), e.GetDescription());
            }
            else {
                std::cerr << err_msg << std::endl;
                std::terminate();
            }
        }
    }

} // namespace SN
