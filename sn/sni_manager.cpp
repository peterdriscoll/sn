#include "sni_manager.h"

#include "sn_error.h"
#include "sn_manager.h"

#include "logcontext.h"

#include <chrono>
#include <thread>
#include <WinBase.h>

#include "sn_pch.h"

#define VK_F4 62
#define VK_SHIFT_F4 87
#define VK_F5 63
#define VK_SHIFT_F5 88
#define VK_F6 64
#define VK_SHIFT_F6 89
#define VK_F7 65
#define VK_SHIFT_F7 90
#define VK_F8 66
#define VK_SHIFT_F8 91
#define VK_F10 68
#define VK_F11 133
#define VK_SHIFT_F11 135
#define VK_F12 136
#define VK_H 104
#define VK_SHIFT_H 72
#define VK_Q 113
#define VK_SHIFT_Q 81

#define MAX_DEPTH_CHARS 20

namespace SNI
{
	string DefaultLogFilePath = "\\log\\SN_";
	/*static*/ enum SN::DebugAction SNI_Manager::m_DebugAction;
	/*static*/ long SNI_Manager::m_FrameStackDepth;
	/*static*/ long SNI_Manager::m_ThreadNum;
	/*static*/ long SNI_Manager::m_Depth = -1;
	/*static*/ vector<long> SNI_Manager::m_ThreadStepCountList;
	/*static*/ long SNI_Manager::m_GotoThreadNum;
	/*static*/ long SNI_Manager::m_StepCount;

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
		, m_DebugFieldWidth(20)
		, m_DebugTitleWidth(40)
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
		, m_DebugFieldWidth(20)
		, m_DebugTitleWidth(40)
		, m_LogFilePath(DefaultLogFilePath)
		, m_HasConsole(false)
		, m_KbHit(NULL)
		, m_GetCh(NULL)
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

	void SNI_Manager::StartLogging(SN::LoggingLevel p_LoggingLevel, ostream *p_Stream)
	{
		ostream *l_Stream = p_Stream;
		if (!l_Stream)
		{
			l_Stream = CreateLogFile(p_LoggingLevel);
		}
		SNI_Log::GetLog()->AddStream(p_LoggingLevel, l_Stream);
	}

	void SNI_Manager::StartDebug(SN::DebugAction p_DebugAction, int p_KbHit(), int p_GetCh())
	{
		m_HasConsole = true;
		m_DebugAction = p_DebugAction;
		m_KbHit = p_KbHit;
		m_GetCh = p_GetCh;
		SNI_Log::GetLog()->AddStream(SN::DebugLevel, &cout);
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
		case SN::RunToEnd:
			return false;
		case SN::Run:
			return baseInterrupt;
		case SN::StepInto:
			return baseInterrupt || p_InterruptPoint == SN::CallPoint;
		case SN::StepOver:
			return baseInterrupt || (p_InterruptPoint == SN::CallPoint && p_ThreadNum == m_ThreadNum && p_FrameStackDepth <= m_FrameStackDepth);
		case SN::StepOut:
			return baseInterrupt || (p_InterruptPoint == SN::CallPoint&& p_ThreadNum == m_ThreadNum  && p_FrameStackDepth < m_FrameStackDepth);
		case SN::StepParameter:
			return p_InterruptPoint == SN::BreakPoint || p_InterruptPoint == SN::CallPoint || p_InterruptPoint == SN::ParameterPoint;
		case SN::GotoStepCount:
			return m_StepCount == m_ThreadStepCountList[m_GotoThreadNum];
		}
		return false;
	}

	void SNI_Manager::DebugCommand(SN::InterruptPoint p_InterruptPoint, const string &p_Text)
	{
		if (!HasConsole())
		{
			return;
		}
		long l_ThreadNum = GetThreadNum();
		long l_FrameStackDepth = SNI_Frame::GetFrameStackDepth();
		if (KbHit())
		{
			ProcessCommand(p_Text, l_ThreadNum, l_FrameStackDepth);
		}
		if (IsBreakPoint(p_InterruptPoint, l_ThreadNum, l_FrameStackDepth))
		{
			ProcessCommand(p_Text, l_ThreadNum, l_FrameStackDepth);
		}
	}

	size_t SNI_Manager::DebugFieldWidth()
	{
		return m_DebugFieldWidth;
	}

	void SNI_Manager::SetDebugFieldWidth(size_t p_DebugFieldWidth)
	{
		m_DebugFieldWidth = p_DebugFieldWidth;
	}

	size_t SNI_Manager::DebugTitleWidth()
	{
		return m_DebugTitleWidth;
	}

	void SNI_Manager::SetDebugTitleWidth(size_t p_DebugTitleWidth)
	{
		m_DebugTitleWidth = p_DebugTitleWidth;
	}

	void SNI_Manager::ProcessCommand(const string & p_Text, long l_ThreadNum, long l_FrameStackDepth)
	{
		SNI_Log::GetLog()->WriteFrameStack(SN::DebugLevel, m_Depth);
		m_DebugAction = SN::None;
		WriteStepCounts(l_ThreadNum);
		m_FrameStackDepth = l_FrameStackDepth;
		bool displayPrompt = true;
		while (m_DebugAction == SN::None)
		{
			if (displayPrompt)
			{
				cout << p_Text << ">> ";
			}
			displayPrompt = true;
			int response = GetCh();
			switch (response)
			{
			case VK_F4:
				cout << "F4 - Display value\nEnter name >> ";
				char buffer[MAX_DEPTH_CHARS];
				cin.getline(buffer, MAX_DEPTH_CHARS);
				SNI_Log::GetLog()->WriteVariableByName(SN::DebugLevel, buffer);
				break;
			case VK_SHIFT_F4:
				break;
			case VK_F5:
				cout << "F5 - Run\n";
				m_DebugAction = SN::Run;
				break;
			case VK_SHIFT_F5:
				cout << "Shift F5 - Run to end\n";
				m_DebugAction = SN::RunToEnd;
				break;
			case VK_F6:
				cout << "F6 - Display stack\n";
				SNI_Log::GetLog()->WriteFrameStack(SN::DebugLevel, -1);
				break;
			case VK_SHIFT_F6:
			{
				cout << "F6 - Display stack\nEnter depth >> ";
				char buffer[MAX_DEPTH_CHARS];
				cin.getline(buffer, MAX_DEPTH_CHARS);
				m_Depth = atol(buffer);
				SNI_Log::GetLog()->WriteFrameStack(SN::DebugLevel, m_Depth);
				break;
			}
			case VK_F7:
				m_DebugAction = SN::StepInto;
				__debugbreak();
				break;
			case VK_F8:
			{
				cout << "F8 - Goto step count\nEnter step count >> ";
				char buffer[MAX_DEPTH_CHARS];
				m_GotoThreadNum = 0;
				cin.getline(buffer, MAX_DEPTH_CHARS);
				m_StepCount = atol(buffer);
				m_DebugAction = SN::GotoStepCount;
				break;
			}
			case VK_SHIFT_F8:
			{
				cout << "F8 - Goto step count\nEnter thread >> ";
				char buffer[MAX_DEPTH_CHARS];
				cin.getline(buffer, MAX_DEPTH_CHARS);
				m_GotoThreadNum = atol(buffer);
				cin.getline(buffer, MAX_DEPTH_CHARS);
				cout << "Enter step count >> ";
				m_StepCount = atol(buffer);
				m_DebugAction = SN::GotoStepCount;
				break;
			}
			case VK_F10:
				cout << "F10 - Step over\n1";
				m_DebugAction = SN::StepOver;
				break;
			case VK_F11:
				cout << "F11 -  Step into\n";
				m_DebugAction = SN::StepInto;
				break;
			case VK_SHIFT_F11:
				cout << "Shift F11 - Step out\n";
				m_DebugAction = SN::StepOut;
				break;
			case VK_F12:
				cout << "F12 - Step to parameter\n";
				m_DebugAction = SN::StepParameter;
				break;
			case VK_H:
			case VK_SHIFT_H:
			{
				cout << "Help:\n";
				cout << "F4        - Variable value\n";
				cout << "F5        - Run\n";
				cout << "Shift F5  - Run to end (ignore breakpoints)\n";
				cout << "F6        - Frame stack\n";
				cout << "Shift F6  - Frame stack to depth\n";
				cout << "F7        - Debug break to C++\n";
				cout << "F8        - Run until step count reaches value\n";
				cout << "Shift F8  - Run until step count reaches value on thread\n";
				cout << "F10       - Step over\n";
				cout << "F11       - Step into\n";
				cout << "Shift F11 - Step out\n";
				cout << "F12       - Step to parameter\n";
				cout << "h, H      - Help\n";
				break;
			}
			case VK_Q:
			case VK_SHIFT_Q:
				exit(-1);
			default:
				displayPrompt = false;
				break;
			}
		}
	}

	size_t SNI_Manager::GetThreadNum()
	{
		size_t l_ThreadNum = SNI_Frame::GetThreadNum();
		size_t lastSize = m_ThreadStepCountList.size();
		if (lastSize <= l_ThreadNum)
		{
			m_ThreadStepCountList.resize(l_ThreadNum + 1);
			for (size_t j = lastSize; j <= l_ThreadNum; j++)
			{
				m_ThreadStepCountList[j] = 0;
			}
		}
		m_ThreadStepCountList[l_ThreadNum]++;
		return l_ThreadNum;
	}

	void SNI_Manager::WriteStepCounts(long l_ThreadNum)
	{
		cout << "Step count: ";
		string separator;
		for (size_t k = 0; k <= l_ThreadNum; k++)
		{
			long stepCount = m_ThreadStepCountList[k];
			cout << separator << to_string(k) << ':' << to_string(stepCount);
			if (k == l_ThreadNum)
			{
				cout << '*';
			}
			separator = " ";
		}
		cout << "\n";
	}

	ostream * SNI_Manager::CreateLogFile(SN::LoggingLevel p_LoggingLevel)
	{
		string currentDirectory = CurrentWorkingDirectory();

		string timeId = GetFormattedTime();
		string logIndex = SN::GetLoggingLevelCode(p_LoggingLevel);
		string fileName = SN::SN_Manager::GetTopManager().LogFilePath() + logIndex + /*"_" + timeId +*/ ".log"; //" + timeId + "
		fstream *logFile = new fstream;
		logFile->open(fileName.data(), ios::out | ios::trunc);
		if (!logFile->is_open())
		{
			delete logFile;
			throw SN::SN_Error("Log file " + fileName + " not opened. Check folders exist in path from " + currentDirectory);
		}
		return logFile;
	}
}
