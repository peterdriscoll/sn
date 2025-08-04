#include "sni_transaction.h"  
#include "sni_user.h"  

#include "sn_pch.h"  

namespace SN  
{  
    SN_User::SN_User()  
    {  
        m_User = new SNI::SNI_User();  
    }  
    /*static*/ SN::SN_OperatorVariables& SN_User::GetOperators()
    { 
		return SNI_User::GetCurrentUser()->GetOperators();
    }

    SN_User::~SN_User()  
    {  
    }  
}
