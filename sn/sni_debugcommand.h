#if !defined(SNI_DEBUGCOMMAND_H_INCLUDED)
#define SNI_DEBUGCOMMAND_H_INCLUDED

#pragma once

#include <string>
#include <set>
using namespace std;

#include "sn_manager.h"

namespace SN
{
	class SN_Error;
}

namespace SNI
{
	class SNI_DebugCommand
	{
	public:
		SNI_DebugCommand();
		virtual ~SNI_DebugCommand();

		long GetThreadNum();

		bool IsBreakPoint(SN::InterruptPoint p_InterruptPoint, long p_ThreadNum, long p_FrameStackDepth, long p_StepCount, const string &p_BreakPoint);
		void LoadBreakPoints(const string &p_BreakPointString);

		bool IsRunning();
		bool IsQuitting();
		bool IsExiting();

		void ScheduleCommand(SN::DebugAction p_DebugAction);

		// Debug break actions.
		void Run();
		void RunToEnd();
		void Debug();
		void DebugBreak();
		void StepOver(long p_StackDepth);
		void StepInto();
		void StepOut(long p_StackDepth);
		void StepParam();
		void GotoStepCount(long p_StepCount, long p_ThreadNum);
		void Quit();
		void Abort();

		// Display actions
		void SelectThread(long p_ThreadNum);

		string Text();
		void SetText(const string &p_Text);

		void SetRunning(bool p_Running);
	private:
		bool m_IsExiting;
		SN::DebugAction m_DebugAction;
		long m_FrameStackDepth;
		long m_ThreadNum;
		long m_StepCount;
		bool m_Running;
		string m_Text;

		set<string> m_BreakPointSet;
		mutex m_Mutex;
		condition_variable m_ReadyForCommandCond;
		bool m_ReadyForCommand;
		condition_variable m_ReadyForProcessingCond;
		bool m_ReadyForProcessing;
	};
}

#endif // !defined(SNI_DEBUGCOMMAND_H_INCLUDED)
