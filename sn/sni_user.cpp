#include "sni_user.h"
#include "sni_thread.h"
#include "sni_delayedprocessor.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_User::SNI_User()
		: m_Processor(new SNI_DelayedProcessor())
		, m_MainThread(SNI_Thread::GetThread())
	{

	}

}
