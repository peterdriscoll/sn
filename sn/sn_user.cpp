#include "sni_transaction.h"  
#include "sni_user.h"  

#include "sn_pch.h"  

namespace SN  
{  
    /*static*/ SN_User SN_User::GetCurrentUser()
    {
        return SN_User(SNI_User::GetCurrentUser());  
    }

    SN_User::SN_User()  
    {  
        m_User = new SNI::SNI_User();  
    }

    SN_User::SN_User(SNI::SNI_User *p_User)
    {  
        m_User = p_User;  
    }

    SN_User::~SN_User()  
    {  
    }  
    
    /*static*/ SN::SN_OperatorVariables& SN_User::GetOperators()
    { 
		return SNI_User::GetCurrentUser()->GetOperators();
    }

    SN_Domain SN_User::Domain()
    {
        return SNI_User::GetCurrentUser()->Domain();
    }
}
