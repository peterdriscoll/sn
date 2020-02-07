#include "sni_error.h"

#include "logcontext.h"

#include "sn_pch.h"

namespace SNI
{
	thread_local long g_ErrorHandledDepth = 0;

	/*static*/ bool SNI_Error::ErrorIsHandled()
	{
		return 0 <g_ErrorHandledDepth;
	}

	/*static*/ void SNI_Error::PushHandler()
	{
		g_ErrorHandledDepth++;
	}

	/*static*/ void SNI_Error::PopHandler()
	{
		g_ErrorHandledDepth--;
	}

	SNI_Error::SNI_Error()
	{
	}

	SNI_Error::SNI_Error(bool p_Success, bool p_Delay, const string & p_Description)
		: m_Success(p_Success)
		, m_Delay(p_Delay)
		, m_Description(p_Description)
	{
		if (!m_Success && !p_Description.empty() && !SNI_World::ContextWorld())
		{
			LOG(WriteLine(SN::ErrorLevel, GetLogDescription()));
			if (!ErrorIsHandled())
			{
				SNI_Thread::GetThread()->DebugCommand(SN::ErrorPoint, "Error", SN::ErrorId);
			}
		}
	}

	SNI_Error::SNI_Error(const string &p_Description)
		: m_Success(false)
		, m_Delay(false)
		, m_Description(p_Description)
	{
		if (!m_Success && !p_Description.empty() && !SNI_World::ContextWorld())
		{
			LOG(WriteLine(SN::ErrorLevel, GetLogDescription()));
			if (!ErrorIsHandled())
			{
				SNI_Thread::GetThread()->DebugCommand(SN::ErrorPoint, "Error", SN::ErrorId);
			}
		}
	}

	SNI_Error::SNI_Error(const SNI_Error &p_Other)
		: m_Success(p_Other.m_Success)
		, m_Delay(p_Other.m_Delay)
		, m_Description(p_Other.m_Description)
	{
	}

	SNI_Error::~SNI_Error()
	{
	}

	string SNI_Error::GetTypeName() const
	{
		return "Error";
	}

	string SNI_Error::DisplayCpp() const
	{
		return "Error(" + m_Description + ")";
	}

	string SNI_Error::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "Error(" + m_Description + ")";
	}

	long SNI_Error::GetPriority() const
	{
		return 100;
	}

	void SNI_Error::AddNote(SNI_CallRecord * p_CallRecord)
	{
		m_CallHistory.push_back(p_CallRecord);
	}

	bool SNI_Error::IsNull() const
	{
		return true;
	}

	bool SNI_Error::IsError() const
	{
		return !m_Success;
	}

	bool SNI_Error::GetBool()
	{
		return m_Success;
	}

	bool SNI_Error::GetDelay()
	{
		return m_Delay;
	}

	string SNI_Error::GetDescription()
	{
		return m_Description;
	}

	string SNI_Error::GetLogDescription()
	{
		return "Error: " + m_Description;
	}

	void SNI_Error::Log()
	{
#ifdef USE_LOGGING
		SN::LogContext context((GetDelay() ? string("Delayed ") : string("")) + (GetBool() ? string("Error") : string("Success")) + string(" Report"));
		context.LogText("Description ", GetDescription());
		for (SNI_CallRecord *callRecord : m_CallHistory)
		{
			context.LogText("Note", callRecord->GetPurpose());
			context.LogText("Exp", callRecord->GetExpression().DisplaySN());
			context.LogText("Context", callRecord->GetLogContext());
		}
#endif
	}
}
