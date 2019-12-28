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

		void SetDebugId(const string & p_DebugId);

		void DebugCommand(SN::InterruptPoint p_InterruptPoint, const string & p_Text, unsigned long p_BreakId);
		void ScheduleCommand(SN::DebugAction p_DebugAction);
		bool IsExiting();
		void LoadBreakPoints(const string &p_BreakPointString);

		void Run();
		void RunToEnd();
		void Debug();
		void DebugBreak();
		void StepOver();
		void StepInto();
		void StepOut();
		void StepParam();
		void GotoStepCount(long p_StepCount);
		void SetMaxStackFrames(long p_MaxStackFrames);
		void Quit();
		void Abort();

		string Skynet(enum DisplayOptionType p_DebugHTML);
		string RunWeb(enum DisplayOptionType p_DebugHTML);
		string RunToEndWeb(enum DisplayOptionType p_DebugHTML);
		string DebugWeb(enum DisplayOptionType p_DebugHTML);
		string DebugBreakWeb(enum DisplayOptionType p_DebugHTML);
		string StepOverWeb(enum DisplayOptionType p_DebugHTML);
		string StepIntoWeb(enum DisplayOptionType p_DebugHTML);
		string StepOutWeb(enum DisplayOptionType p_DebugHTML);
		string StepParamWeb(enum DisplayOptionType p_DebugHTML);
		string GotoStepCountWeb(long p_StepCount, enum DisplayOptionType p_DebugHTML);
		string SetMaxStackFramesWeb(long p_MaxStackFrame, enum DisplayOptionType p_DebugHTML);
		string SetThreadNumWeb(enum DisplayOptionType p_DebugHTML);
		string QuitWeb(enum DisplayOptionType p_DebugHTML);

		string SkynetJS();
		string StackJS(long p_MaxStackFrame, enum DisplayOptionType p_DebugHTML);
		string StepCountJS();
		string LogJS(long p_MaxLogEntries);

		void Lock();
		void Unlock();

		SNI_Frame * Top();

		void PushFrame(SNI_Frame * p_Frame);
		void PopFrame();
		size_t GetFrameStackDepth();

		void SetDeepBreakPoint(const string & p_BreakPoint);

		void WriteWebPage(ostream &p_Stream, bool p_Refresh, DisplayOptionType p_DebugHTML);
		static void WriteWebPageJS(ostream & p_Stream, bool p_Refresh);
		void WriteShuttingDown(ostream & p_Stream);

		SNI_Variable * LookupVariable(const string & p_Name);
		void DisplayFrameStack(long p_Depth);
		void DisplayStepCounts();
		void WriteStepCounts(ostream & p_Stream);
		void WriteStepCount(ostream & p_Stream);

		static void WriteWebStepCountListJS(ostream & p_Stream);

		ostream * CreateLogFile(SN::LoggingLevel p_LoggingLevel);

		void PromoteExternals(PGC::PGC_Transaction * p_Transaction);

		SNI_Manager *GetTopManager(bool p_Create = true);
		void SetTopManager(SNI_Manager *p_TopManager);

		long GetDefineId();
		void UpdateIncrementId();

	private:
		string StartCommand(SN::DebugAction p_DebugAction, const string & p_Description, enum DisplayOptionType p_DebugHTML);

		static void WriteW3Credentials(ostream & p_Stream);
		void WriteCommands(ostream & p_Stream);
		static void WriteWebCommandsJS(ostream & p_Stream);
		void WriteSubmit(ostream & p_Stream, const string & p_Action, const string & p_Name, const string & p_Description);
		static void WriteSubmitJS(ostream & p_Stream, const string & p_Action, const string & p_Name, const string & p_Description);
		void WriteGotoStepCount(ostream &p_Stream);
		static void WriteGotoStepCountJS(ostream & p_Stream);
		void WriteSetMaxStackFrames(ostream & p_Stream);
		static void WriteSetMaxStackFramesJS(ostream & p_Stream);

		void WriteWebStack(ostream & p_Stream, size_t  p_Depth, size_t p_DebugFieldWidth, SNI::SNI_DisplayOptions &p_DisplayOptions);

		static void WriteWebStackJS(ostream & p_Stream);
		static void WriteWebLogJS(ostream & p_Stream);

		void WriteStackJS(ostream & p_Stream, size_t  p_Depth, size_t p_DebugFieldWidth, SNI::SNI_DisplayOptions &p_DisplayOptions);
		void WriteLogJS(ostream & p_Stream, long p_MaxLogEntries);
		void WriteStepCountListJS(ostream & p_Stream);
		void WriteStepCountJS(ostream & p_Stream, const string &p_Delimeter);

		size_t m_ThreadNum;
		mutex m_Mutex;
		SNI_DebugCommand m_DebugCommand;
		SNI_FrameList m_FrameList;
		long m_ThreadStepCount;
		bool m_WebServerThreadUsed;
		SNI_Manager *m_TopManager;
		bool m_Ended;
		long m_DefineId;

		static vector<SNI_Thread *> m_ThreadList;
		static mutex m_ThreadListMutex;
		enum SN::DebugAction m_DebugAction;
		string m_DebugId;

		long m_MaxStackFrames;
		static long m_GotoThreadNum;
		static long m_StepCount;
		static bool m_Running;

		static thread *m_WebServerThread;
		static long m_WebServerThreadUsageCount;;
	};
}

#endif // !defined(SNI_THREAD_H_INCLUDED)
