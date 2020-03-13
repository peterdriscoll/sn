#include "sn_thread.h"

#include "sni_thread.h"

#include "sn_pch.h"

namespace SN
{
	SN_Thread::SN_Thread()
		: m_Thread(SNI::SNI_Thread::GetThread())
	{
	}

	SN_Thread::~SN_Thread()
	{
	}

	bool SN_Thread::HasTopManager()
	{
		return m_Thread->GetTopManager(false) != NULL;;
	}

	SNI::SNI_Thread * SN_Thread::GetSNI_Thread()
	{
		return m_Thread;
	}
}
