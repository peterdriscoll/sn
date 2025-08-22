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

	/*static*/ SNI_Class *SNI_Error::m_Class = NULL;
	/*static*/ SNI_Class *SNI_Error::Class()
	{
		if (!m_Class)
		{
			m_Class = new SNI_Class("Error");
		}
		return m_Class;
	}

	SNI_Class *SNI_Error::VClass()
	{
		return Class();
	}

	// --- Primary ctor ---
	SNI_Error::SNI_Error(bool p_Success, bool p_Delay, const std::string& p_Description)
		: m_Success(p_Success)
		, m_Delay(p_Delay)
		, m_RequestRerun(false)
		, m_Description(p_Description)
		, m_CallHistory(GetTransaction())
		, m_ChildErrorList(GetTransaction())
	{
		if (!m_Success && !p_Description.empty() && !SNI_Thread::GetThread()->ContextWorld())
		{
			if (!ErrorIsHandled())
			{
				SNI_Thread::GetThread()->RegisterError(this);
			}
		}
	}

	SNI_Expression * SNI_Error::Copy() const
	{
		return new SNI_Error(*this);
	}

	std::string SNI_Error::GetTypeName() const
	{
		if (m_Success)
		{
			return "Result";
		}
		else
		{
			return "Error";
		}
	}

	std::string SNI_Error::DisplayCpp() const
	{
		return "Error(" + m_Description + ")";
	}

	std::string SNI_Error::DisplaySN(long /*priority*/, SNI_DisplayOptions & p_DisplayOptions) const
	{
		if (m_Success)
		{
			return "OK";
		}
		if (m_Delay && p_DisplayOptions.GetDebugHTML() == doTextOnly)
		{
			return "delayed";
		}
		if (m_Description.empty())
		{
			return "fail";
		}
		return "Error(\"" + m_Description + "\")";
	}

	long SNI_Error::GetPriority() const
	{
		return 100;
	}

	void SNI_Error::AddNote(SNI_CallRecord * p_CallRecord)
	{
		m_CallHistory.push_back(p_CallRecord);
		LOG(WriteLine(SN::InfoLevel, GetLogDescription()));
		Breakpoint(SN::ErrorStop, SN::ErrorId, GetTypeName(), "Error", NULL, SN::ErrorPoint);
	}

	bool SNI_Error::IsNull() const
	{
		return true;
	}

	void SNI_Error::AddError(const SN::SN_Error &p_Error)
	{
		SNI_Error *e = p_Error.GetSNI_Error();
		ASSERTM(e != this, "Recursive error inclusion.");
		m_ChildErrorList.push_back(e);
	}

	void SNI_Error::CheckChildError()
	{
		if (m_Success && !m_ChildErrorList.empty())
		{
			bool success = false;
			for (SNI_Error *e : m_ChildErrorList)
			{
				if (!e->IsError())
				{
					success = true;
				}
			}
			if (!success)
			{
				m_Success = success;
				m_Description = "All options failed.";
			}
		}
	}

	bool SNI_Error::IsError() const
	{
		const_cast<SNI_Error *>(this)->CheckChildError();
		return !m_Success;
	}

	bool SNI_Error::IsErrorType() const
	{
		return true;
	}

	bool SNI_Error::IsSignificantError() const
	{
		return !m_Success && !m_Description.empty() && !SNI_Thread::GetThread()->ContextWorld() && !ErrorIsHandled();
	}

	bool SNI_Error::GetBool()
	{
		return m_Success;
	}

	bool SNI_Error::GetDelay()
	{
		return m_Delay;
	}

	bool SNI_Error::RequestRerun()
	{
		return m_RequestRerun;
	}

	void SNI_Error::MakeRerunRequest()
	{
		m_RequestRerun =  true;
	}

	std::string SNI_Error::GetDescription()
	{
		return m_Description;
	}

	std::string SNI_Error::GetLogDescription()
	{
		return "Error: " + m_Description;
	}

	void SNI_Error::WriteJSON(std::ostream &p_Stream, SNI::SNI_DisplayOptions &p_DisplayOptions)
	{
		p_Stream << "{\n";
		p_Stream << "\t\"description\" : \"" << EscapeStringToJSON(m_Description) << "\",";
		p_Stream << "\t\"callhistory\" : [";
		std::string delimeter = "\n";
		for (SNI_CallRecord *callRecord : m_CallHistory)
		{
			p_Stream << delimeter << "\t{\n";
			p_Stream << "\t\t\"purpose\" : \"" << EscapeStringToJSON(callRecord->GetPurpose()) << "\",";
			p_Stream << "\t\t\"expression\" : \"" << EscapeStringToJSON(callRecord->GetExpression().DisplaySN(p_DisplayOptions)) << "\",";
			p_Stream << "\t\t\"logcontext\" : \"" << EscapeStringToJSON(callRecord->GetLogContext()) << "\"";
			p_Stream << "\t}";
			delimeter = ",\n";
		}
		p_Stream << "\n\t]\n";
		p_Stream << "}\n";
	}

	void SNI_Error::Log()
	{
#ifdef USE_LOGGING
		SN::LogContext context((GetDelay() ? std::string("Delayed ") : std::string("")) + (GetBool() ? std::string("Error") : std::string("Success")) + std::string(" Report"));
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
