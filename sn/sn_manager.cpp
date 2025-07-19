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

	/*static*/ void SN_Manager::LogicSetup()
	{
		SNI_Manager::LogicSetup();
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

	SN_Manager::SN_Manager(string p_Description, OnErrorHandler p_Handler, bool p_DelayOnEvaluate, size_t p_MaxCardinalityCall, size_t p_MaxCardinalityUnify)
		: m_Manager(new SNI::SNI_Manager(p_Description, p_Handler, p_DelayOnEvaluate, p_MaxCardinalityCall, p_MaxCardinalityUnify))
		, m_MyManager(true)
	{
	}

	SN_Manager::~SN_Manager()
	{
		bool err;
		string err_description;
		OnErrorHandler* handler;
		{
			SN_Error e = DelayedCalls().DoEvaluate().GetError();
			err = e.IsError();
			if (err)
			{
				handler = ErrorHandler();
				err_description = e.GetDescription();
			}
		}

		if (m_MyManager && m_Manager)
		{
			delete m_Manager;
		}

		if (err)
		{
			handler(err, err_description);
		}
	}

	SN_Expression SN_Manager::DelayedCalls()
	{
		return m_Manager->DelayedCalls();
	}

	string SN_Manager::Description()
	{
		return m_Manager->Description();
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
		m_Manager->SetDelayOnEvaluate(p_DelayOnEvaluate);
	}

	void SN_Manager::SetMaxCardinality(size_t p_MaxCardinalityCall, size_t p_MaxCardinalityUnify)
	{
		m_Manager->SetMaxCardinality(p_MaxCardinalityCall, p_MaxCardinalityUnify);
	}

	size_t SN_Manager::MaxStackFrames()
	{
		return m_Manager->MaxStackFrames();
	}

	void SN_Manager::SetMaxStackFrames(size_t p_MaxStackFrame)
	{
		m_Manager->SetMaxStackFrames(p_MaxStackFrame);
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

	void SN_Manager::StartDebugCommandLineServer(skynet::DebugAction p_DebugAction, int p_kbhit(), int p_GetCh())
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

	void SN_Manager::StartWebServer(skynet::DebugAction p_DebugAction, const string & p_Address, const string & p_Port, const string & p_DocRoot, bool p_DoIt)
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

	bool SN_Manager::AutoDefine()
	{
		return m_Manager->AutoDefine();
	}

	void SN_Manager::SetAutoDefine(bool p_AutoDefine)
	{
		m_Manager->SetAutoDefine(p_AutoDefine);
	}

	skynet::DirectPassType SN_Manager::GetDirectPassType()
	{
		return m_Manager->GetDirectPassType();
	}

	void SN_Manager::SetDirectPassType(skynet::DirectPassType p_DirectPassType)
	{
		m_Manager->SetDirectPassType(p_DirectPassType);
	}

	skynet::EvaluationType SN_Manager::GetEvaluationType()
	{
		return m_Manager->GetEvaluationType();
	}

	void SN_Manager::SetEvaluationType(skynet::EvaluationType p_EvaluationType)
	{
		m_Manager->SetEvaluationType(p_EvaluationType);
	}

	skynet::LogicType SN_Manager::GetLogicType()
	{
		return m_Manager->GetLogicType();
	}

	void SN_Manager::SetLogicType(skynet::LogicType p_LogicType)
	{
		m_Manager->SetLogicType(p_LogicType);
	}

	bool SN_Manager::TailCallOptimization() const
	{
		return m_Manager->TailCallOptimization();
	}

	void SN_Manager::SetTailCallOptimization(bool p_TailCallOptimization)
	{
		m_Manager->SetTailCallOptimization(p_TailCallOptimization);
	}

	PGC::PromotionStrategy SN_Manager::MemoryPromotionStrategy() const
	{
		return m_Manager->MemoryPromotionStrategy();
	}

	void SN_Manager::SetMemoryPromotionStrategy(PGC::PromotionStrategy p_MemoryPromotionStrategy)
	{
		return m_Manager->SetMemoryPromotionStrategy(p_MemoryPromotionStrategy);
	}

	bool SN_Manager::AutoExpandNull() const
	{
		return m_Manager->AutoExpandNull();
	}

	void SN_Manager::SetAutoExpandNull(bool p_AutoExpandNull)
	{
		return m_Manager->SetAutoExpandNull(p_AutoExpandNull);
	}

	void SN_Manager::Breakpoint()
	{
		m_Manager->Breakpoint();
	}

	void SN_Manager::SetDebugAction(enum skynet::DebugAction p_DebugAction)
	{
		m_Manager->SetDebugAction(p_DebugAction);
	}

	SNI::SNI_Manager * SN_Manager::GetSNI_Manager()
	{
		return m_Manager;
	}
}
