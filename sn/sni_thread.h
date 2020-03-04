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
	class SNI_Error;
	class SNI_Frame;
	typedef vector<SNI_Frame *> SNI_FrameList;

	class SNI_Thread
	{
	public:
		static SNI_Thread *GetThread();
		static SNI_Thread *GetThreadByNumber(size_t p_ThreadNum);
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

		void Clear();
		
		size_t GetThreadNum();
		SNI_FrameList & GetFrameList();

		string & GetDebugId();
		void SetDebugId(const string & p_DebugId);

		size_t GetStepCount();

		SNI_DelayedProcessor *GetProcessor();

		void DebugCommand(SN::InterruptPoint p_InterruptPoint, const string & p_Text, unsigned long p_BreakId);
		void ScheduleCommand(skynet::DebugAction p_DebugAction);
		bool IsExiting();
		void LoadBreakPoints(const string &p_BreakPointString);
		void RegisterError(SNI_Error *p_Error);
		void Run();
		void RunToEnd();
		void Debug();
		void CodeBreak();
		void StepOver();
		void StepInto();
		void StepOut();
		void StepParam();
		void GotoStepCount(long p_StepCount);
		void SetMaxStackFrames(long p_MaxStackFrames);
		void Quit();
		void Abort();

		string Skynet(enum DisplayOptionType p_OptionType);
		string RunWeb(enum DisplayOptionType p_OptionType);
		string RunToEndWeb(enum DisplayOptionType p_OptionType);
		string DebugWeb(enum DisplayOptionType p_OptionType);
		string CodeBreakWeb(enum DisplayOptionType p_OptionType);
		string StepOverWeb(enum DisplayOptionType p_OptionType);
		string StepIntoWeb(enum DisplayOptionType p_OptionType);
		string StepOutWeb(enum DisplayOptionType p_OptionType);
		string StepParamWeb(enum DisplayOptionType p_OptionType);
		string GotoStepCountWeb(long p_StepCount, enum DisplayOptionType p_OptionType);
		string SetMaxStackFramesWeb(long p_MaxStackFrame, enum DisplayOptionType p_OptionType);
		string SetThreadNumWeb(enum DisplayOptionType p_OptionType);
		string QuitWeb(enum DisplayOptionType p_OptionType);

		string DashboardJS(DisplayOptionType p_OptionType);
		string StackJS(long p_MaxStackFrame, enum DisplayOptionType p_OptionType);
		string StepCountJS();
		string LogJS(long p_MaxLogEntries);
		string DerivationJS(long p_MaxLogEntries);
		string LogExpJS(long p_MaxLogEntries, enum DisplayOptionType p_OptionType);
		string ErrorJS(enum DisplayOptionType p_OptionType);
		string WorldSetsJS(DisplayOptionType p_OptionType);
		string DelayedJS(DisplayOptionType p_OptionType);

		void Lock();
		void Unlock();

		SNI_Frame * Top();

		void PushFrame(SNI_Frame * p_Frame);
		void PopFrame();
		size_t GetFrameStackDepth();

		void SetDeepBreakPoint(const string & p_BreakPoint, const string & p_BreakPointJS);
		void SetThreadBreakPoint(const string & p_BreakPoint, const string & p_BreakPointJS);

		SN::SN_Error CheckForFails();

		void SetDebugAction(enum skynet::DebugAction p_DebugLevel);

		SNI_World * ContextWorld();
		SNI_WorldList & ContextWorldList();
		void PushContextWorld(SNI_World * p_Context);
		void PopContextWorld();

		void WriteWebPage(ostream &p_Stream, bool p_Refresh, DisplayOptionType p_OptionType);
		void WriteShuttingDown(ostream & p_Stream);

		SNI_Variable * LookupVariable(const string & p_Name);
		void DisplayFrameStack(long p_Depth);
		void DisplayStepCounts();
		void WriteStepCounts(ostream & p_Stream);
		void WriteStepCount(ostream & p_Stream);

		ostream * CreateLogFile(SN::LoggingLevel p_LoggingLevel);

		void PromoteExternals(PGC::PGC_Transaction * p_Transaction);

		SNI_Manager *GetTopManager(bool p_Create = true);
		void SetTopManager(SNI_Manager *p_TopManager);

		long GetDefineId();
		void UpdateIncrementId();

		SNI_WorldSetList *GetWorldSetChanged();
		SNI_WorldSetMap * GetWorldSetProcessMap();
		void ClearDependencyChecks();

		void ScheduleCodeBreak();

		void RegisterChange(SNI_Variable *p_NewValue);
		string ChangeHistoryJS(DisplayOptionType p_OptionType, size_t p_ColumnWidth, size_t p_FromStep, size_t p_ToStep);
		void WriteChangeHistoryJS(ostream & p_Stream, SNI::SNI_DisplayOptions & p_DisplayOptions, size_t p_ColumnWidth, size_t p_FromStep, size_t p_ToStep);

	private:
		string StartCommand(skynet::DebugAction p_DebugAction, const string & p_Description, enum DisplayOptionType p_OptionType);

		static void WriteW3Credentials(ostream & p_Stream);
		void WriteCommands(ostream & p_Stream);
		void WriteSubmit(ostream & p_Stream, const string & p_Action, const string & p_Name, const string & p_Description);
		static void WriteSubmitJS(ostream & p_Stream, const string & p_Action, const string & p_Name, const string & p_Description);
		void WriteGotoStepCount(ostream &p_Stream);
		static void WriteGotoStepCountJS(ostream & p_Stream);
		void WriteSetMaxStackFrames(ostream & p_Stream);
		static void WriteSetMaxStackFramesJS(ostream & p_Stream);

		void WriteWebStack(ostream & p_Stream, size_t  p_Depth, size_t p_DebugFieldWidth, SNI::SNI_DisplayOptions &p_DisplayOptions);

		void WriteDashboardJS(ostream & p_Stream, SNI::SNI_DisplayOptions & p_DisplayOptions);
		void WriteStackJS(ostream & p_Stream, size_t  p_Depth, size_t p_DebugFieldWidth, SNI::SNI_DisplayOptions &p_DisplayOptions);
		void WriteLogJS(ostream & p_Stream, long p_MaxLogEntries);
		void WriteDerivationJS(ostream & p_Stream, long p_MaxLogEntries);
		void WriteLogExpJS(ostream & p_Stream, long p_MaxLogEntries, SNI_DisplayOptions &p_DisplayOptions);
		void WriteStepCountListJS(ostream & p_Stream);
		void WriteStepCountJS(ostream & p_Stream, const string &p_Delimeter);
		void WriteWorldSetsJS(ostream & p_Stream, SNI_DisplayOptions &p_DisplayOptions);

		size_t m_ThreadNum;
		mutex m_Mutex;
		mutex m_ChangeMutex;
		SNI_DebugCommand m_DebugCommand;
		SNI_FrameList m_FrameList;
		size_t m_ThreadStepCount;
		bool m_WebServerThreadUsed;
		SNI_Manager *m_TopManager;
		bool m_Ended;
		bool m_Closing;
		long m_DefineId;

		SNI_Error *m_Error;

		static vector<SNI_Thread *> m_ThreadList;
		static mutex m_ThreadListMutex;
		enum skynet::DebugAction m_DebugAction;
		string m_DebugId;
		string m_BreakPoint;
		string m_BreakPointJS;

		long m_MaxStackFrames;

		SNI_WorldSetList *m_WorldSetChangedList;
		SNI_WorldSetMap *m_WorldSetProcessMap;

		bool m_CodeBreakScheduled;
		SNI_DelayedProcessor *m_Processor;

		SNI_WorldList m_ContextStack;

		SNI_ChangeMap m_ChangeMap;

		static long m_GotoThreadNum;
		static long m_StepCount;
		static bool m_Running;

		static thread *m_WebServerThread;
		static long m_WebServerThreadUsageCount;
	};
}

#endif // !defined(SNI_THREAD_H_INCLUDED)
