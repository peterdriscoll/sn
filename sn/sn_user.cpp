#include "sni_transaction.h"  
#include "sni_user.h"  

#include "sn_pch.h"  

namespace SN  
{  
    SN_User::SN_User()  
    {  
        m_Transaction = new SNI::SNI_Transaction();  
        m_User = new SNI::SNI_User();  
    }  

    SN_User::~SN_User()  
    {  
        delete m_Transaction;  
    }  
}
