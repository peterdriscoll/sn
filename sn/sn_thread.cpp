#include "sn_thread.h"

#include "sni_thread.h"

#include "sn_pch.h"

namespace SN
{
	SN_Thread t_Thread;

	SN_Thread &SN_Thread::GetThread()
	{
		return t_Thread;
	}

	/*static*/ SNI::SNI_Manager * SN_Thread::TopManager()
	{
		return t_Thread.GetSNI_Thread()->GetTopManager();
	}

	SN_Thread::SN_Thread()
		: m_Thread(NULL)
	{
	}

	SN_Thread::SN_Thread(SNI::SNI_Thread * p_Thread)
		: m_Thread(p_Thread)
	{
	}

	SN_Thread::~SN_Thread()
	{
		if (m_Thread)
		{
			delete m_Thread;
		}
	}

	SNI::SNI_Thread * SN_Thread::GetSNI_Thread()
	{
		if (!m_Thread)
		{
			m_Thread = SNI_Thread::BuildThread();
		}
		return m_Thread;
	}
}
