#include "sn_manager.h"

#include "sni_manager.h"

#include "sn_pch.h"

namespace SN
{
	/*static*/ string GetLoggingLevelCode(SN::LoggingLevel p_LoggingLevel)
	{
		switch (p_LoggingLevel)
		{
		case ErrorLevel:
			return "ERR";
		case WarningLevel:
			return "WRN";
		case DebugLevel:
			return "DBG";
		case InfoLevel:
			return "INF";
		case DetailLevel:
			return "DTL";
		default:
			return "???";
		}
	}

	SN_Manager::SN_Manager()
		: m_Manager(new SNI::SNI_Manager())
		, m_MyManager(true)
	{
	}

	SN_Manager::SN_Manager(SNI::SNI_Manager * p_Manager)
		: m_Manager(p_Manager)
		, m_MyManager(false)
	{
	}

	SN_Manager::SN_Manager(OnErrorHandler *p_Handler, bool p_DelayOnEvaluate, size_t p_MaxCardinalityCall, size_t p_MaxCardinalityUnify)
		: m_Manager(new SNI::SNI_Manager(p_Handler, p_DelayOnEvaluate, p_MaxCardinalityCall, p_MaxCardinalityUnify))
		, m_MyManager(true)
	{
	}

	SN_Manager::~SN_Manager()
	{
		if (m_MyManager && m_Manager)
		{
			delete m_Manager;
		}
	}

	string SN_Manager::LogFilePath()
	{
		return m_Manager->LogFilePath();
	}

	void SN_Manager::SetLogFilePath(string p_LogFilePath)
	{
		return m_Manager->SetLogFilePath(p_LogFilePath);
	}

	OnErrorHandler * SN_Manager::ErrorHandler()
	{
		return m_Manager->ErrorHandler();
	}

	void SN_Manager::SetErrorHandler(OnErrorHandler * p_ErrorHandler)
	{
		return m_Manager->SetErrorHandler(p_ErrorHandler);
	}

	bool SN_Manager::DelayOnEvaluate()
	{
		return m_Manager->DelayOnEvaluate();
	}

	void SN_Manager::SetDelayOnEvaluate(bool p_DelayOnEvaluate)
	{
		return m_Manager->SetDelayOnEvaluate(p_DelayOnEvaluate);
	}

	void SN_Manager::SetMaxCardinality(size_t p_MaxCardinalityCall, size_t p_MaxCardinalityUnify)
	{
		return m_Manager->SetMaxCardinality(p_MaxCardinalityCall, p_MaxCardinalityUnify);
	}

	size_t SN_Manager::MaxCardinalityCall()
	{
		return m_Manager->MaxCardinalityCall();
	}

	size_t SN_Manager::MaxCardinalityUnify()
	{
		return m_Manager->MaxCardinalityUnify();
	}

	void SN_Manager::StartLogging(SN::LoggingLevel p_LoggingLevel, ostream * p_Stream)
	{
		return m_Manager->StartLogging(p_LoggingLevel, p_Stream);
	}

	void SN_Manager::StartDebugCommandLineServer(DebugAction p_DebugAction, int p_kbhit(), int p_GetCh())
	{
		return m_Manager->StartDebugCommandLineServer(p_DebugAction, p_kbhit, p_GetCh);
	}

	bool SN_Manager::HasConsole()
	{
		return m_Manager->HasConsole();
	}
	bool SN_Manager::KbHit()
	{
		return m_Manager->KbHit();
	}
	int SN_Manager::GetCh()
	{
		return m_Manager->GetCh();
	}

	void SN_Manager::StartWebServer(SN::DebugAction p_DebugAction, const string & p_Address, const string & p_Port, const string & p_DocRoot, bool p_DoIt)
	{
		if (p_DoIt)
		{
			m_Manager->StartWebServer(p_DebugAction, p_Address, p_Port, p_DocRoot);
		}
	}

	size_t SN_Manager::DebugFieldWidth()
	{
		return m_Manager->DebugFieldWidth();
	}

	void SN_Manager::SetDebugFieldWidth(size_t p_DebugFieldWidth)
	{
		m_Manager->SetDebugFieldWidth(p_DebugFieldWidth);
	}

	size_t SN_Manager::DebugTitleWidth()
	{
		return m_Manager->DebugTitleWidth();
	}

	void SN_Manager::SetDebugTitleWidth(size_t p_DebugTitleWidth)
	{
		m_Manager->SetDebugTitleWidth(p_DebugTitleWidth);
	}

	SNI::SNI_Manager * SN_Manager::GetSNI_Manager()
	{
		return m_Manager;
	}
}
