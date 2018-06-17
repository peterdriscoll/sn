#include "sni_debugcommand.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_DebugCommand::SNI_DebugCommand()
		: m_IsExiting(false)
		, m_DebugAction(SN::None)
		, m_FrameStackDepth(0)
	    , m_ThreadNum(0)
	    , m_StepCount(0)
	    , m_Running(false)
		, m_ReadyForCommand(true)
		, m_ReadyForProcessing(false)
	{
	}

	SNI_DebugCommand::~SNI_DebugCommand()
	{
	}

	long SNI_DebugCommand::GetThreadNum()
	{
		return m_ThreadNum;
	}

	bool SNI_DebugCommand::IsBreakPoint(SN::InterruptPoint p_InterruptPoint, long p_ThreadNum, long p_FrameStackDepth, long p_StepCount)
	{
		bool breakPoint = false;
		unique_lock<mutex> mutex_lock(m_Mutex);
		bool processCommand = m_ReadyForProcessing;
		if (!m_ReadyForProcessing)
		{
			cout << m_Text << " >> ";
		}
		while (!m_ReadyForProcessing)
		{
			m_ReadyForProcessingCond.wait(mutex_lock);
		}
		bool baseInterrupt = (p_InterruptPoint == SN::BreakPoint || p_InterruptPoint == SN::ErrorPoint || p_InterruptPoint == SN::EndPoint);
		switch (m_DebugAction)
		{
		case SN::Run:
			breakPoint = p_InterruptPoint == SN::ErrorPoint;
			break;
		case SN::RunToEnd:
			breakPoint = p_InterruptPoint == SN::EndPoint || p_InterruptPoint == SN::ErrorPoint;
			break;
		case SN::Debug:
			breakPoint = baseInterrupt;
			break;
		case SN::StepInto:
			breakPoint = baseInterrupt || p_InterruptPoint == SN::CallPoint;
			break;
		case SN::StepOver:
			breakPoint = baseInterrupt || (p_InterruptPoint == SN::CallPoint && p_ThreadNum == m_ThreadNum && p_FrameStackDepth <= m_FrameStackDepth);
			break;
		case SN::StepOut:
			breakPoint = baseInterrupt || (p_InterruptPoint == SN::CallPoint&& p_ThreadNum == m_ThreadNum  && p_FrameStackDepth < m_FrameStackDepth);
			if (breakPoint)
			{
				long dog = 10;
			}
			break;
		case SN::StepParameter:
			breakPoint = p_InterruptPoint == SN::BreakPoint || p_InterruptPoint == SN::CallPoint || p_InterruptPoint == SN::ParameterPoint;
			break;
		case SN::GotoStepCount:
			breakPoint = m_StepCount == p_StepCount;
			break;
		}
		m_ReadyForCommand = true;
		m_ReadyForProcessing = !breakPoint;
		m_ReadyForCommandCond.notify_one();
		return breakPoint && processCommand;
	}

	bool SNI_DebugCommand::IsExiting()
	{
		unique_lock<mutex> mutex_lock(m_Mutex);
		return m_IsExiting;
	}

	void SNI_DebugCommand::ScheduleCommand(SN::DebugAction p_DebugAction)
	{
		unique_lock<mutex> mutex_lock(m_Mutex);
		while (!m_ReadyForCommand)
		{
			m_ReadyForCommandCond.wait(mutex_lock);
		}
		m_DebugAction = p_DebugAction;
		m_ReadyForProcessing = true;
		m_ReadyForCommand = false;
		m_ReadyForProcessingCond.notify_one();
	}

	void SNI_DebugCommand::Run()
	{
		ScheduleCommand(SN::Run);
	}

	void SNI_DebugCommand::RunToEnd()
	{
		ScheduleCommand(SN::RunToEnd);
	}

	void SNI_DebugCommand::Debug()
	{
		ScheduleCommand(SN::Debug);
	}

	void SNI_DebugCommand::DebugBreak()
	{
		ScheduleCommand(SN::RunToEnd);
	}

	void SNI_DebugCommand::StepOver(long p_StackDepth)
	{
		m_FrameStackDepth = p_StackDepth;
		ScheduleCommand(SN::StepOver);
	}

	void SNI_DebugCommand::StepInto()
	{
		ScheduleCommand(SN::StepInto);
	}

	void SNI_DebugCommand::StepOut(long p_StackDepth)
	{
		m_FrameStackDepth = p_StackDepth;
		ScheduleCommand(SN::StepOut);
	}

	void SNI_DebugCommand::StepParam()
	{
		ScheduleCommand(SN::StepParameter);
	}

	void SNI_DebugCommand::GotoStepCount(long p_StepCount, long p_ThreadNum)
	{
		unique_lock<mutex> mutex_lock(m_Mutex);
		while (!m_ReadyForCommand)
		{
			m_ReadyForCommandCond.wait(mutex_lock);
		}
		m_DebugAction = SN::GotoStepCount;
		m_StepCount = p_StepCount;
		m_ThreadNum = p_ThreadNum;
		m_ReadyForProcessing = true;
		m_ReadyForCommand = false;
		m_ReadyForProcessingCond.notify_one();
	}

	void SNI_DebugCommand::Abort()
	{
		m_DebugAction = SN::Abort;
	}

	void SNI_DebugCommand::Quit()
	{
		m_DebugAction = SN::Quit;
	}

	void SNI_DebugCommand::SelectThread(long p_ThreadNum)
	{
		m_ThreadNum = p_ThreadNum;
	}

	string SNI_DebugCommand::Text()
	{
		return m_Text;
	}

	void SNI_DebugCommand::SetText(const string & p_Text)
	{
		m_Text = p_Text;
	}
	void SNI_DebugCommand::SetRunning(bool p_Running)
	{
		m_Running = p_Running;
	}
}
