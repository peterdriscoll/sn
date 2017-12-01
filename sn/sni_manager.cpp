#include "sni_manager.h"

#include "sn_error.h"

#include "sn_pch.h"

#define VK_F10 0x79
#define VK_F11 0x7A

namespace SNI
{
	string DefaultLogFilePath = "\\log\\SN_";

	void ThrowErrorHandler(SN::SN_Error p_Result)
	{
		throw p_Result;
	}

	/*static*/ SNI_Manager *SNI_Manager::m_TopManager =  NULL;

	SNI_Manager::SNI_Manager()
		: m_ErrorHandler(ThrowErrorHandler)
		, m_DelayOnEvaluate(false)
		, m_MaxCardinalityCall(10)
		, m_MaxCardinalityUnify(10)
		, m_LogFilePath(DefaultLogFilePath)
		, m_HasConsole(false)
	    , m_KbHit(NULL)
	    , m_GetCh(NULL)
	{
		m_LastManager = m_TopManager;
		m_TopManager = this;
	}

	SNI_Manager::SNI_Manager(OnErrorHandler *p_ErrorHandler, bool p_DelayOnEvaluate, size_t p_MaxCardinalityCall, size_t p_MaxCardinalityUnify)
		: m_ErrorHandler(p_ErrorHandler)
		, m_DelayOnEvaluate(p_DelayOnEvaluate)
		, m_MaxCardinalityCall(p_MaxCardinalityCall)
		, m_MaxCardinalityUnify(p_MaxCardinalityUnify)
		, m_LogFilePath(DefaultLogFilePath)
	{
		m_LastManager = m_TopManager;
		m_TopManager = this;
	}
	
	SNI_Manager::~SNI_Manager()
	{
		m_TopManager = m_LastManager;
	}

	void SNI_Manager::SetMaxCardinality(size_t p_MaxCardinalityCall, size_t p_MaxCardinalityUnify)
	{
		m_MaxCardinalityCall = p_MaxCardinalityCall;
		m_MaxCardinalityUnify = p_MaxCardinalityUnify;
	}

	string SNI_Manager::LogFilePath()
	{
		return m_LogFilePath;
	}

	void SNI_Manager::SetLogFilePath(string p_LogFilePath)
	{
		m_LogFilePath = p_LogFilePath;
	}
	OnErrorHandler * SNI_Manager::ErrorHandler()
	{
		return m_ErrorHandler;
	}

	void SNI_Manager::SetErrorHandler(OnErrorHandler *p_ErrorHandler)
	{
		if (p_ErrorHandler)
		{
			m_ErrorHandler = p_ErrorHandler;
		}
		else
		{
			m_ErrorHandler = ThrowErrorHandler;
		}
	}

	bool SNI_Manager::DelayOnEvaluate()
	{
		return m_DelayOnEvaluate;
	}

	void SNI_Manager::SetDelayOnEvaluate(bool p_DelayOnEvaluate)
	{
		m_DelayOnEvaluate = p_DelayOnEvaluate;
	}

	size_t SNI_Manager::MaxCardinalityCall()
	{
		return m_MaxCardinalityCall;
	}

	size_t SNI_Manager::MaxCardinalityUnify()
	{
		return m_MaxCardinalityUnify;
	}

	/*static*/ SNI_Manager * SNI_Manager::GetTopManager()
	{
		if (!m_TopManager)
		{
			m_TopManager = new SNI_Manager();
		}
		return m_TopManager;
	}

	void SNI_Manager::ConsoleFunctions(bool p_KbHit(), int p_GetCh())
	{
		m_HasConsole = true;
		m_KbHit = p_KbHit; 
		m_GetCh = p_GetCh;
	}

	bool SNI_Manager::HasConsole()
	{
		return m_GetCh;
	}

	bool SNI_Manager::KbHit()
	{
		return (*m_KbHit)();
	}

	int SNI_Manager::GetCh()
	{
		return (*m_GetCh)();
	}

	void SNI_Manager::DebugCommand(string p_text)
	{
		while (KbHit())
		{
			int response = GetCh();
			/*
			switch (response)
			{
			case VK_F1:

			}
			*/
		}
	}
}
