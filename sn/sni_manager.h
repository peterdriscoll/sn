#if !defined(SNI_MANAGER_H_INCLUDED)
#define SNI_MANAGER_H_INCLUDED

#pragma once

#include <string>
using namespace std;

#include "sni_debugcommand.h"

namespace SN
{
	class SN_Error;
}

namespace SNI
{
	typedef void OnErrorHandler(SN::SN_Error p_Result);

	class SNI_Manager
	{
	public:
		static SNI_Manager *GetTopManager();

		SNI_Manager();
		SNI_Manager(OnErrorHandler *p_ErrorHandler, bool p_DelayOnEvaluate, size_t p_MaxCardinalityCall, size_t p_MaxCardinalityUnify);

		virtual ~SNI_Manager();

		string LogFilePath();
		void SetLogFilePath(string p_LogFilePath);

		OnErrorHandler *ErrorHandler();
		void SetErrorHandler(OnErrorHandler * p_ErrorHandler);

		size_t MaxCardinalityCall();
		size_t MaxCardinalityUnify();
		void SetMaxCardinality(size_t p_MaxCardinalityCall, size_t p_MaxCardinalityUnify);

		bool DelayOnEvaluate();
		void SetDelayOnEvaluate(bool p_DelayOnEvaluate);

		void StartLogging(SN::LoggingLevel p_LoggingLevel, ostream *p_Stream);
		void ScheduleCommand(SN::DebugAction p_DebugAction);
		void DebugCommandLineServer(int p_KbHit(), int p_GetCh());
		void StartDebugCommandLineServer(SN::DebugAction p_DebugAction, int p_KbHit(), int p_GetCh());
		void StartDebug(SN::DebugAction p_DebugAction, int p_Kbhit(), int p_GetCh());
		void StartWebServer(SN::DebugAction p_DebugAction, const string& p_Address, const string& p_Port, const string& p_DocRoot);

		bool HasConsole();
		bool KbHit();
		int  GetCh();

		bool IsBreakPoint(SN::InterruptPoint p_InterruptPoint, long p_ThreadNum, long p_FrameStackDepth);
		void DebugCommand(SN::InterruptPoint p_InterruptPoint, const string & p_Text);
		void DebugCommand2(SN::InterruptPoint p_InterruptPoint, const string & p_Text);

		size_t DebugFieldWidth();
		void SetDebugFieldWidth(size_t p_DebugFieldWidth);

		size_t DebugTitleWidth();
		void SetDebugTitleWidth(size_t p_DebugFieldWidth);

		ostream * CreateLogFile(SN::LoggingLevel);

		string StartCommand(SN::DebugAction p_DebugAction, const string & p_Description);
		string Skynet();
		string Run();
		string RunToEnd();
		string DebugBreak();
		string StepOver();
		string StepInto();
		string StepOut();
		string StepParam();
		string GotoStepCount(long p_StepCount, long p_ThreadNum);
		string SetMaxStackFrames(long p_StackDepth);
		string Quit();

		void Lock();
		void Unlock();

		void WriteWebPage(ostream &p_Stream, bool p_Refresh);
		void WriteShuttingDown(ostream & p_Stream);
	private:
		void WriteCommands(ostream & p_Stream);
		void WriteSubmit(ostream & p_Stream, const string & p_Action, const string & p_Name, const string & p_Description);
		void WriteGotoStepCount(ostream &p_Stream);
		void WriteSetMaxStackFrames(ostream & p_Stream);

		static SNI_Manager *m_TopManager;
		SNI_Manager *m_LastManager;

		string m_LogFilePath;

		OnErrorHandler *m_ErrorHandler;

		void ProcessCommand(const string & p_Text, long l_ThreadNum, long l_FrameStackDepth);

		size_t GetThreadNum();
		void DisplayStepCounts(long l_ThreadNum);
		void WriteStepCounts(ostream & p_Stream);
		size_t m_MaxCardinalityCall;
		size_t m_MaxCardinalityUnify;

		bool m_DelayOnEvaluate;

		long m_DebugFieldWidth;
		long m_DebugTitleWidth;

		bool m_HasConsole;

		int (*m_KbHit)();
		int (*m_GetCh)();
		mutex m_Mutex;

		SNI_DebugCommand m_DebugCommand;
		static long m_StackDepth;
		static enum SN::DebugAction m_DebugAction;
		static long m_ThreadNum;
		static long m_FrameStackDepth;
		static vector<long> m_ThreadStepCountList;
		static long m_GotoThreadNum;
		static long m_StepCount;
		static bool m_Running;
	};
}

#endif // !defined(SNI_MANAGER_H_INCLUDED)
