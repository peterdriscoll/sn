#include "sni_user.h"
#include "sni_thread.h"
#include "sni_delayedprocessor.h"

#include "sn_pch.h"

namespace SNI
{
	// Static function to be used as the thread entry point
	void SNI_User::UserThreadFunc(SNI_User* p_User)
	{
		SNI::SNI_Thread::GetThread()->SetUser(p_User);
	}
	SNI_User::SNI_User()
		: m_Processor(new SNI_DelayedProcessor())
		, m_MainThread(SNI_Thread::GetThread())
	{
		SNI::SNI_Thread::GetThread()->SetUser(this);

		// Start a thread passing m_User
		std::thread userThread(UserThreadFunc, this);
		userThread.detach(); // Detach the thread to run independently


	}

}
