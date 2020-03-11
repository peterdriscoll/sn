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

	SNI_Error::SNI_Error()
		: m_Success(true)
		, m_Delay(false)
	{
	}

	SNI_Error::SNI_Error(const SNI_Error &p_Other)
		: m_Success(p_Other.m_Success)
		, m_Delay(p_Other.m_Delay)
		, m_Description(p_Other.m_Description)
		, m_CallHistory(p_Other.m_CallHistory)
	{
	}

	SNI_Error::SNI_Error(bool p_Success, bool p_Delay, const string & p_Description)
		: m_Success(p_Success)
		, m_Delay(p_Delay)
		, m_Description(p_Description)
	{
		if (!p_Success)
		{
			long dog = 10;
		}
		if (!m_Success && !p_Description.empty() && !SNI_Thread::GetThread()->ContextWorld())
		{
			LOG(WriteLine(SN::ErrorLevel, GetLogDescription()));
			if (!ErrorIsHandled())
			{
				SNI_Thread::GetThread()->RegisterError(this);
			}
		}
	}

	SNI_Error::~SNI_Error()
	{
	}

	SNI_Expression * SNI_Error::Copy() const
	{
		return new SNI_Error(*this);
	}

	string SNI_Error::GetTypeName() const
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

	string SNI_Error::DisplayCpp() const
	{
		return "Error(" + m_Description + ")";
	}

	string SNI_Error::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		if (m_Success)
		{
			return "true";
		}
		else if (m_Description.empty())
		{
			return "false";
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

	void SNI_Error::AddError(SN::SN_Error &p_Error)
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

	string SNI_Error::GetDescription()
	{
		return m_Description;
	}

	string SNI_Error::GetLogDescription()
	{
		return "Error: " + m_Description;
	}

	void SNI_Error::WriteJS(ostream &p_Stream, SNI::SNI_DisplayOptions &p_DisplayOptions)
	{
		p_Stream << "{\n";
		p_Stream << "\t\"description\" : \"" << EscapeStringToJSON(m_Description) << "\",";
		p_Stream << "\t\"callhistory\" : [";
		string delimeter = "\n";
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
