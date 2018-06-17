#if !defined(SNI_THREAD_H_INCLUDED)
#define SNI_THREAD_H_INCLUDED

#pragma once

#include "pgc.h"
#include <string>
#include <vector>
using namespace std;

#include "sni_debugcommand.h"

namespace SN
{
	class SN_Error;
}

namespace SNI
{
	typedef void OnErrorHandler(SN::SN_Error p_Result);

	class SNI_Variable;
	class SNI_Frame;
	typedef vector<SNI_Frame *> SNI_FrameList;

	class SNI_Thread
	{
	public:
		static SNI_Thread *GetThread();
		static SNI_Thread *GetThreadByNumber(long p_ThreadNum);
		static size_t GetNumThreads();
		static SNI_Thread *BuildThread();
		static SNI_Manager *TopManager();

		static string ThreadEnded(long p_ThreadNum);
		static void WriteThreadEnded(ostream & p_Stream, long p_ThreadNum);

		static void ThreadListLock();
		static void ThreadListUnlock();

		SNI_Thread(size_t p_ThreadNum);

		virtual ~SNI_Thread();

		void Init();
		
		size_t GetThreadNum();
		SNI_FrameList & GetFrameList();

		void DebugCommand(SN::InterruptPoint p_InterruptPoint, const string & p_Text);
		void ScheduleCommand(SN::DebugAction p_DebugAction);
		bool IsExiting();

		void Run();
		void RunToEnd();
		void Debug();
		void DebugBreak();
		void StepOver(long p_StackDepth);
		void StepInto();
		void StepOut(long p_StackDepth);
		void StepParam();
		void GotoStepCount(long p_StepCount);
		void SetMaxStackFrames(long p_StackDepth);
		void Quit();
		void Abort();

		string Skynet();
		string RunWeb();
		string RunToEndWeb();
		string DebugWeb();
		string DebugBreakWeb();
		string StepOverWeb(long p_StackDepth);
		string StepIntoWeb();
		string StepOutWeb(long p_StackDepth);
		string StepParamWeb();
		string GotoStepCountWeb(long p_StepCount);
		string SetMaxStackFramesWeb(long p_StackDepth);
		string SetThreadNumWeb();
		string QuitWeb();
		string AbortWeb();

		void Lock();
		void Unlock();

		SNI_Frame * Top();

		void PushFrame(SNI_Frame * p_Frame);
		void PopFrame();
		size_t GetFrameStackDepth();

		void WriteWebPage(ostream &p_Stream, bool p_Refresh);
		void WriteShuttingDown(ostream & p_Stream);

		SNI_Variable * LookupVariable(const string & p_Name);
		void DisplayFrameStack(long p_Depth);
		void DisplayStepCounts();
		void WriteStepCounts(ostream & p_Stream);
		void WriteStepCount(ostream & p_Stream);

		ostream * CreateLogFile(SN::LoggingLevel p_LoggingLevel);

		SNI_Manager *GetTopManager(bool p_Create = true);
		void SetTopManager(SNI_Manager *p_TopManager);
	private:
		string StartCommand(SN::DebugAction p_DebugAction, const string & p_Description);

		void WriteCommands(ostream & p_Stream);
		void WriteSubmit(ostream & p_Stream, const string & p_Action, const string & p_Name, const string & p_Description);
		void WriteGotoStepCount(ostream &p_Stream);
		void WriteSetMaxStackFrames(ostream & p_Stream);

		void WriteWebStack(ostream & p_Stream, long p_Depth, size_t p_DebugFieldWidth);

		size_t m_ThreadNum;
		mutex m_Mutex;
		SNI_DebugCommand m_DebugCommand;
		SNI_FrameList m_FrameList;
		long m_ThreadStepCount;
		bool m_WebServerThreadUsed;
		SNI_Manager *m_TopManager;
		bool m_Ended;

		static vector<SNI_Thread *> m_ThreadList;
		static mutex m_ThreadListMutex;
		enum SN::DebugAction m_DebugAction;

		static long m_StackDepth;
		static long m_GotoThreadNum;
		static long m_StepCount;
		static bool m_Running;
		static thread *m_WebServerThread;
		static long m_WebServerThreadUsageCount;;
	};
}

#endif // !defined(SNI_THREAD_H_INCLUDED)
