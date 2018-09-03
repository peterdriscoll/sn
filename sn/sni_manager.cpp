#include "sni_manager.h"

#include "sn_error.h"
#include "sn_manager.h"

#include "logcontext.h"

#include <chrono>
#include <thread>
#include <windows.h>

#include "sn_pch.h"

#define VK_FUNCTION 0
#define VK_FUNCTION2 224

//#define VK_F4 62
#define VK_SHIFT_F4 87
//#define VK_F5 63
#define VK_SHIFT_F5 188
#define VK_CONTROL_F5 88
//#define VK_F6 64
#define VK_SHIFT_F6 89
//#define VK_F7 65
#define VK_SHIFT_F7 90
//#define VK_F8 66
#define VK_SHIFT_F8 91
//#define VK_F10 68
//#define VK_F11 133
#define VK_SHIFT_F11 135
//#define VK_F12 136
#define VK_H 104
#define VK_SHIFT_H 72
#define VK_Q 113
#define VK_SHIFT_Q 81

#define MAX_DEPTH_CHARS 20

namespace SNI
{
	string DefaultLogFilePath = "\\log\\SN_";

	/*static*/ thread *SNI_Manager::m_CommandServerThread = NULL;
	/*static*/ long SNI_Manager::m_CommandServerThreadUsageCount = 0;

	/*static*/ thread *SNI_Manager::m_WebServerThread = NULL;
	/*static*/ HTTP::server::server *SNI_Manager::m_WebServer = NULL;
	/*static*/ long SNI_Manager::m_WebServerThreadUsageCount = 0;

	/*static*/ void SNI_Manager::ThrowErrorHandler(SN::SN_Error p_Result)
	{
		throw p_Result;
	}

	bool SNI_Manager::HasDebugServer()
	{
		return m_HasDebugServer;
	}

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
		, m_WebServerThreadUsed(false)
		, m_CommandServerThreadUsed(false)
		, m_HasDebugServer(false)
		, m_LogBufferCapacity(50)
		, m_MaxStackFrames(3)
		, m_Transaction(false)
	{
		if (!m_ErrorHandler)
		{
			m_ErrorHandler = ThrowErrorHandler;
		}
		m_LastManager = SNI_Thread::GetThread()->GetTopManager(false);
		if (!m_LastManager)
		{
			m_Transaction.Init();
		}
		SNI_Thread::GetThread()->SetTopManager(this);
	}

	SNI_Manager::SNI_Manager(SNI_Manager *p_Manager)
		: m_Description(p_Manager->m_Description)
		, m_ErrorHandler(p_Manager->m_ErrorHandler)
		, m_DelayOnEvaluate(p_Manager->m_DelayOnEvaluate)
		, m_MaxCardinalityCall(p_Manager->m_MaxCardinalityCall)
		, m_MaxCardinalityUnify(p_Manager->m_MaxCardinalityUnify)
		, m_LogFilePath(p_Manager->m_LogFilePath)
		, m_HasConsole(p_Manager->m_HasConsole)
		, m_DebugFieldWidth(p_Manager->m_DebugFieldWidth)
		, m_DebugTitleWidth(p_Manager->m_DebugTitleWidth)
		, m_KbHit(p_Manager->m_KbHit)
		, m_GetCh(p_Manager->m_GetCh)
		, m_WebServerThreadUsed(false)
		, m_CommandServerThreadUsed(false)
		, m_HasDebugServer(false)
		, m_LogBufferCapacity(50)
		, m_MaxStackFrames(3)
		, m_Transaction(false)
	{
		if (!m_ErrorHandler)
		{
			m_ErrorHandler = ThrowErrorHandler;
		}
		m_LastManager = SNI_Thread::GetThread()->GetTopManager(false);
		if (!m_LastManager)
		{
			m_Transaction.Init();
		}
		SNI_Thread::GetThread()->SetTopManager(this);
	}

	SNI_Manager::SNI_Manager(string p_Description, OnErrorHandler *p_ErrorHandler, bool p_DelayOnEvaluate, size_t p_MaxCardinalityCall, size_t p_MaxCardinalityUnify)
		: m_Description(p_Description)
		, m_ErrorHandler(p_ErrorHandler)
		, m_DelayOnEvaluate(p_DelayOnEvaluate)
		, m_MaxCardinalityCall(p_MaxCardinalityCall)
		, m_MaxCardinalityUnify(p_MaxCardinalityUnify)
		, m_DebugFieldWidth(20)
		, m_DebugTitleWidth(40)
		, m_LogFilePath(DefaultLogFilePath)
		, m_HasConsole(false)
		, m_KbHit(NULL)
		, m_GetCh(NULL)
		, m_WebServerThreadUsed(false)
		, m_CommandServerThreadUsed(false)
		, m_HasDebugServer(false)
		, m_LogBufferCapacity(50)
		, m_MaxStackFrames(3)
		, m_Transaction(false)
	{
		if (!m_ErrorHandler)
		{
			m_ErrorHandler = ThrowErrorHandler;
		}
		m_LastManager = SNI_Thread::GetThread()->GetTopManager(false);
		if (!m_LastManager)
		{
			SNI_Thread::GetThread()->Init();
			m_Transaction.Init();
		}
		SNI_Thread::GetThread()->SetTopManager(this);
	}
	
	SNI_Manager::~SNI_Manager()
	{
		SNI_Thread::GetThread()->DebugCommand(SN::EndPoint,"Exit");
		SNI_Thread::GetThread()->SetTopManager(m_LastManager);
		if (m_CommandServerThreadUsed)
		{
			m_CommandServerThreadUsageCount--;
			if (m_CommandServerThreadUsageCount == 0)
			{
				delete m_CommandServerThread;
				m_CommandServerThread = NULL;
			}
		}

		if (m_WebServerThreadUsed)
		{
			m_WebServerThreadUsageCount--;
			if (m_WebServerThreadUsageCount == 0)
			{
				// m_WebServer->stop();
				// m_WebServerThread->join();
				// delete m_WebServer;
				// m_WebServer = NULL;
				// delete m_WebServerThread;
				// m_WebServerThread = NULL;
			}
		}
	}

	void SNI_Manager::Init()
	{
		m_LastManager = SNI_Thread::TopManager();
		if (!m_LastManager)
		{
			m_Transaction.Init();
		}
		SNI_Thread::GetThread()->SetTopManager(this);
	}

	string SNI_Manager::Description()
	{
		return m_Description;
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

	void SNI_Manager::StartLogging(SN::LoggingLevel p_LoggingLevel, ostream *p_Stream)
	{
		ostream *l_Stream = p_Stream;
		if (!l_Stream)
		{
			l_Stream = CreateLogFile(p_LoggingLevel);
		}
		SNI_Log::GetLog()->AddStream(p_LoggingLevel, l_Stream);
	}

	void SNI_Manager::StartDebugCommandLineServer(SN::DebugAction p_DebugAction, int p_KbHit(), int p_GetCh())
	{
		try
		{
			m_HasDebugServer = true;
			SNI_Thread *l_thread = SNI_Thread::GetThread();
			l_thread->ScheduleCommand(p_DebugAction);
			SNI_Log::GetLog()->AddStream(SN::DebugLevel, &cout);
			if (m_CommandServerThreadUsageCount == 0)
			{
				thread *commandThread = new thread(DebugCommandLineServer, l_thread, p_KbHit, p_GetCh);
				m_CommandServerThreadUsed = true;
			}
			m_CommandServerThreadUsageCount++;
		}
		catch (std::exception& e)
		{
			std::cerr << "exception: " << e.what() << "\n";
		}
	}

	void SNI_Manager::ScheduleWebServerShutdown()
	{
		try
		{
			thread *commandThread = new thread(WebServerShutdown);
		}
		catch (std::exception& e)
		{
			std::cerr << "exception: " << e.what() << "\n";
		}
	}

	void SNI_Manager::WebServerShutdown()
	{

		Sleep(1000);

		m_WebServer->stop();
		m_WebServerThread->join();
		delete m_WebServer;
		m_WebServer = NULL;
		delete m_WebServerThread;
		m_WebServerThread = NULL;
	}

	void SNI_Manager::DebugCommandLineServer(SNI_Thread * p_Thread, int p_KbHit(), int p_GetCh())
	{
		SNI_Thread *l_thread = p_Thread;
		l_thread->DisplayFrameStack(20);
		while (!l_thread->IsExiting())
		{
			int response = p_GetCh();
			switch (response)
			{
			case VK_FUNCTION:
			case VK_FUNCTION2:
			{
				int responseFunctionKey = p_GetCh();
				switch (responseFunctionKey)
				{
				case VK_F3:
				{
					cout << "F3 - Select thread 0.." << SNI_Thread::GetNumThreads() - 1 << "\nEnter thread num  >> ";
					char buffer[MAX_DEPTH_CHARS];
					cin.getline(buffer, MAX_DEPTH_CHARS);
					l_thread = SNI_Thread::GetThreadByNumber(atol(buffer));
					break;
				}
				case VK_F4:
				{
					cout << "F4 - Display value\nEnter name >> ";
					char buffer[MAX_DEPTH_CHARS];
					cin.getline(buffer, MAX_DEPTH_CHARS);
					SNI_Log::GetLog()->WriteVariableByName(SN::DebugLevel, buffer);
					break;
				}
				case VK_SHIFT_F4:
					break;
				case VK_F5:
					cout << "F5 - Run\n";
					l_thread->Run();
					break;
				case VK_SHIFT_F5:
					cout << "Shift F5 - Run to end\n";
					l_thread->RunToEnd();
					break;
				case VK_CONTROL_F5:
					cout << "Shift F5 - Run to end\n";
					l_thread->Debug();
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
					l_thread->SetMaxStackFrames(atol(buffer));
					SNI_Log::GetLog()->WriteFrameStack(SN::DebugLevel, l_thread->GetThreadNum(), l_thread->GetFrameStackDepth());
					break;
				}
				case VK_F7:
					cout << "F7 - Debug break into C++\n";
					l_thread->DebugBreak();
					break;
				case VK_F8:
				{
					cout << "F8 - Goto step count\nEnter step count >> ";
					char buffer[MAX_DEPTH_CHARS];
					cin.getline(buffer, MAX_DEPTH_CHARS);
					long stepCount = atol(buffer);
					l_thread->GotoStepCount(stepCount);
					break;
				}
				case VK_F10:
					cout << "F10 - Step over\n";
					l_thread->StepOver();
					break;
				case VK_F11:
					cout << "F11 -  Step into\n";
					l_thread->StepInto();
					break;
				case VK_SHIFT_F11:
					cout << "Shift F11 - Step out\n";
					l_thread->StepOut();
					break;
				case VK_F12:
					cout << "F12 - Step to parameter\n";
					l_thread->StepParam();
					break;
				default:
					break;
				}
				break;
			}
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
				l_thread->Quit();
				break;
			case VK_SHIFT_Q:
				l_thread->Abort();
				break;
			default:
				break;
			}
		}
	}

	void SNI_Manager::RunServer(const string& p_Address, const string& p_Port, const string& p_DocRoot)
	{
		try
		{
			// Initialise the server.
			// "0.0.0.0", "80", "C:/sn/html"
			SNI_Manager::m_WebServer = new HTTP::server::server(p_Address, p_Port, p_DocRoot);
			// Run the server until stopped.
			m_WebServer->run();
		}
		catch (std::exception& e)
		{
			std::cerr << "exception: " << e.what() << "\n";
		}
	}

	void SNI_Manager::StartWebServer(SN::DebugAction p_DebugAction, const string& p_Address, const string& p_Port, const string& p_DocRoot)
	{
		m_HasDebugServer = true;
		if (m_WebServerThreadUsageCount == 0 && !m_WebServerThread)
		{
			SNI_Thread::GetThread()->ScheduleCommand(p_DebugAction);
			SNI_Log::GetLog()->SetLogBuffer(SN::DebugLevel, m_LogBufferCapacity);
			m_WebServerThread = new thread(RunServer, p_Address, p_Port, p_DocRoot);
			m_WebServerThreadUsed = true;
			OpenURLInBrowser("http://127.0.0.1/skynetjs.html");
		}
		m_WebServerThreadUsageCount++;
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

	ostream * SNI_Manager::CreateLogFile(SN::LoggingLevel p_LoggingLevel)
	{
		string currentDirectory = CurrentWorkingDirectory();

		string timeId = GetFormattedTime();
		string logIndex = SN::GetLoggingLevelCode(p_LoggingLevel);
		string fileName = SNI_Thread::TopManager()->LogFilePath() + logIndex + /*"_" + timeId +*/ ".log"; //" + timeId + "
		fstream *logFile = new fstream;
		logFile->open(fileName.data(), ios::out | ios::trunc);
		if (!logFile->is_open())
		{
			delete logFile;
			throw SN::SN_Error("Log file " + fileName + " not opened. Check folders exist in path from " + currentDirectory);
		}
		return logFile;
	}
	size_t SNI_Manager::LogBufferCapacity()
	{
		return m_LogBufferCapacity;
	}
	void SNI_Manager::SetLogBufferCapacity(size_t p_LogBufferCapacity)
	{
		m_LogBufferCapacity = p_LogBufferCapacity;
	}
	size_t SNI_Manager::MaxStackFrames()
	{
		return m_MaxStackFrames;
	}
	void SNI_Manager::SetMaxStackFrames(size_t p_MaxStackFrame)
	{
		m_MaxStackFrames = p_MaxStackFrame;
	}
}
