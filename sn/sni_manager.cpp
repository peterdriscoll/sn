#include "sni_manager.h"

#include "sn_error.h"

#include "logcontext.h"

#include <chrono>
#include <thread>

#include "sn_pch.h"

#define VK_F5 63
#define VK_SHIFT_F5 64
#define VK_F10 68
#define VK_F11 133
#define VK_SHIFT_F11 135
#define VK_F12 136
#define VK_H 104
#define VK_SHIFT_H 72

namespace SNI
{
	string DefaultLogFilePath = "\\log\\SN_";
	/*static*/ enum DebugAction SNI_Manager::m_DebugAction;
	/*static*/ long SNI_Manager::m_FrameStackDepth;
	/*static*/ long SNI_Manager::m_ThreadNum;

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

	void SNI_Manager::ConsoleFunctions(int p_KbHit(), int p_GetCh())
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

	bool SNI_Manager::IsBreakPoint(SN::InterruptPoint p_InterruptPoint, long p_ThreadNum, long p_FrameStackDepth)
	{
		bool baseInterrupt = (p_InterruptPoint == SN::BreakPoint || p_InterruptPoint == SN::ErrorPoint);
		switch (m_DebugAction)
		{
		case RunToEnd:
			return false;
		case Run:
			return baseInterrupt;
		case StepInto:
			return baseInterrupt || p_InterruptPoint == SN::CallPoint;
		case StepOver:
			return baseInterrupt || (p_InterruptPoint == SN::CallPoint && p_ThreadNum == m_ThreadNum && p_FrameStackDepth <= m_FrameStackDepth);
		case StepOut:
			return baseInterrupt || (p_InterruptPoint == SN::CallPoint&& p_ThreadNum == m_ThreadNum  && p_FrameStackDepth < m_FrameStackDepth);
		case StepParameter:
			return p_InterruptPoint == SN::BreakPoint || p_InterruptPoint == SN::CallPoint || p_InterruptPoint == SN::ParameterPoint;
		}
		return false;
	}

	void SNI_Manager::DebugCommand(SN::InterruptPoint p_InterruptPoint)
	{
		long l_ThreadNum = SNI_Frame::GetThreadNum();
		long l_FrameStackDepth = SNI_Frame::GetFrameStackDepth();
		if (HasConsole() && IsBreakPoint(p_InterruptPoint, l_ThreadNum, l_FrameStackDepth))
		{
			m_DebugAction = None;
			m_ThreadNum = l_ThreadNum;
			m_FrameStackDepth = l_FrameStackDepth;
			cout << "\n>> ";
			while (m_DebugAction == None)
			{
				int response = GetCh();
				switch (response)
				{
				case VK_F5:
					cout << "F5";
					m_DebugAction = Run;
					break;
				case VK_SHIFT_F5:
					cout << "Shift F5";
					m_DebugAction = RunToEnd;
					break;
				case VK_F10:
					cout << "F10";
					m_DebugAction = StepOver;
					break;
				case VK_F11:
					cout << "F11";
					m_DebugAction = StepInto;
					break;
				case VK_SHIFT_F11:
					cout << "Shift F11";
					m_DebugAction = StepOut;
					break;
				case VK_F12:
					cout << "F12";
					m_DebugAction = StepParameter;
					break;
				case VK_H:
				case VK_SHIFT_H:
				{
					SN::LogContext context("Help: ");
					context.LogText("F5", "Run");
					context.LogText("F10", "Step over");
					context.LogText("F11", "Step into");
					context.LogText("Shift F11", "Step out");
					context.LogText("h, H", "Help");
					break;
				}
				default:
					break;
				}
			}
		}
	}
}
