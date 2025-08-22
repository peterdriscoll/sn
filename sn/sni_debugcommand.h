#if !defined(SNI_DEBUGCOMMAND_H_INCLUDED)
#define SNI_DEBUGCOMMAND_H_INCLUDED

#pragma once

#include <string>
#include <set>

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

		size_t size_tGetThreadNum();

		bool IsBreakPoint(SN::InterruptPoint p_InterruptPoint, size_t p_ThreadNum, size_t p_FrameStackDepth, size_t p_StepCount, const std::string &p_BreakPoint, SN::DebuggingStop p_DebuggingStop);
		void LoadBreakPoints(const std::string &p_BreakPointString);

		bool IsRunning();
		bool IsQuitting();
		bool IsExiting();

		void SetDebugAction(enum skynet::DebugAction p_DebugAction);
		void ScheduleCommand(enum skynet::DebugAction p_DebugAction);

		// Debug break actions.
		void Run();
		void RunToEnd();
		void Debug();
		void CodeBreak();
		void Rerun();
		void StepOver(size_t p_StackDepth);
		void StepInto(SN::DebuggingStop p_DebugStop);
		void StepOut(size_t p_StackDepth);
		void StepParam();
		void GotoStepCount(skynet::DebugAction p_DebugAction, size_t  p_StepCount, size_t p_ThreadNum);
		void Quit();
		void Abort();

		// Display actions
		void SelectThread(long p_ThreadNum);

		std::string Description();
		void SetDescription(const std::string &p_Text);

		void SetRunning(bool p_Running);
	private:
		bool m_IsExiting;
		skynet::DebugAction m_DebugAction;
		size_t m_FrameStackDepth;
		size_t m_ThreadNum;
		size_t m_StepCount;
		bool m_Running;
		std::string m_Description;
		SN::DebuggingStop m_DebugStop;
		set<std::string> m_BreakPointSet;
		std::mutex m_Mutex;
		condition_variable m_ReadyForCommandCond;
		bool m_ReadyForCommand;
		condition_variable m_ReadyForProcessingCond;
		bool m_ReadyForProcessing;
	};
}

#endif // !defined(SNI_DEBUGCOMMAND_H_INCLUDED)
