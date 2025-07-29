#pragma once

#include "exp_ctrl_sn.h"
#include "../sn/sni_expression.h"

namespace SN {
    class SN_Expression;
    
    class SN_EXPORT SN_DeferredCommand {
    public:
        explicit SN_DeferredCommand(const SN_Expression& expr);
        ~SN_DeferredCommand();

        SNI::SNI_Expression* GetSNI_Expression();

        void Do();
        SN_Error DoReturnError();
        void Throw();

    private:
        SNI::SNI_Expression *m_Expression;
        bool m_Done;
    };

} // namespace SN
