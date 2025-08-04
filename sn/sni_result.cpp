#include "sni_result.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sni_delayedprocessor.h"
#include "sni_world.h"
#include "sni_error.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Result::SNI_Result()
		: m_Result(true)
		, m_Delay(false)
		, m_Call(NULL)
		, m_Error(NULL)
	{
	}

	SNI_Result::SNI_Result(bool p_Result, bool p_Delay, SNI_DelayedCall * p_Call, SNI_Error *p_Error)
		: m_Result(p_Result)
		, m_Delay(p_Delay)
		, m_Call(p_Call)
		, m_Error(p_Error)
	{
		if (!m_Result)
		{
			long d0og = 10;
		}
	}

	SNI_Result::~SNI_Result()
	{
	}

	bool SNI_Result::GetResult()
	{
		return m_Result;
	}

	bool SNI_Result::GetDelay()
	{
		return m_Delay;
	}

	SNI_DelayedCall * SNI_Result::GetCall()
	{
		return m_Call;
	}

	void SNI_Result::ProcessWorld(SNI_World *p_World)
	{
		if (GetResult())
		{
			ASSERTM(p_World, "World must exist.");
			p_World->AddToSetList();
			SNI_User::GetCurrentUser()->GetDelayedProcessor()->DelayCall(GetCall(), p_World);
		}
	}
}
