#include "sni_debugcommand.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_DebugCommand::SNI_DebugCommand()
		: m_IsExiting(false)
		, m_DebugAction(skynet::None)
		, m_FrameStackDepth(0)
	    , m_ThreadNum(0)
	    , m_StepCount(0)
	    , m_Running(false)
		, m_ReadyForCommand(true)
		, m_ReadyForProcessing(false)
		, m_DebugStop(SN::DebugStop)
	{
	}

	SNI_DebugCommand::~SNI_DebugCommand()
	{
	}

	long SNI_DebugCommand::GetThreadNum()
	{
		return m_ThreadNum;
	}

	bool SNI_DebugCommand::IsBreakPoint(SN::InterruptPoint p_InterruptPoint, long p_ThreadNum, long p_FrameStackDepth, long p_StepCount, const string &p_BreakPoint, SN::DebuggingStop p_DebuggingStop)
	{
		bool breakPoint = false;
		unique_lock<mutex> mutex_lock(m_Mutex);
		bool processCommand = m_ReadyForProcessing;
		if (!m_ReadyForProcessing)
		{
			cout << m_Description << " >> ";
		}
		while (!m_ReadyForProcessing)
		{
			m_ReadyForProcessingCond.wait(mutex_lock);
		}
		if (p_InterruptPoint == SN::EndPoint)
		{
			breakPoint = (m_DebugAction != skynet::Quit);
			m_DebugAction = skynet::Run;
		}
		else
		{
			bool baseInterrupt = (p_InterruptPoint == SN::BreakPoint || p_InterruptPoint == SN::ErrorPoint || p_InterruptPoint == SN::EndPoint);
			bool callFound = (p_InterruptPoint == SN::CallPoint) ||
				(p_InterruptPoint == SN::FailPoint) ||
				(p_InterruptPoint == SN::WarningPoint) ||
				(p_InterruptPoint == SN::MirrorPoint) ||
				(p_InterruptPoint == SN::ClonePoint) ||
				(p_InterruptPoint == SN::DelayedPoint) ||
				(p_InterruptPoint == SN::StaticPoint);

			switch (m_DebugAction)
			{
			case skynet::Run:
				breakPoint = p_InterruptPoint == SN::ErrorPoint;
				break;
			case skynet::RunToEnd:
				breakPoint = p_InterruptPoint == SN::EndPoint || p_InterruptPoint == SN::ErrorPoint;
				break;
			case skynet::Debug:
				breakPoint = baseInterrupt || (m_BreakPointSet.find(p_BreakPoint) != m_BreakPointSet.end());
				break;
			case skynet::StepInto:
				breakPoint = baseInterrupt || (callFound && (p_DebuggingStop <= m_DebugStop));
				break;
			case skynet::StepOver:
				breakPoint = baseInterrupt || (callFound && p_ThreadNum == m_ThreadNum && p_FrameStackDepth <= m_FrameStackDepth);
				break;
			case skynet::StepOut:
				breakPoint = baseInterrupt || (callFound && p_ThreadNum == m_ThreadNum && (m_FrameStackDepth == 0 || p_FrameStackDepth < m_FrameStackDepth));
				break;
			case skynet::StepParameter:
				breakPoint = p_InterruptPoint == SN::BreakPoint || callFound || p_InterruptPoint == SN::ParameterPoint;
				break;
			case skynet::GotoStepCount:
				breakPoint = baseInterrupt || (m_StepCount == p_StepCount);
				break;
			case skynet::CodeBreak:
				SNI_Thread::GetThread()->ScheduleCodeBreak();
				m_DebugAction = skynet::StepInto;
				breakPoint = false;
				break;
			case skynet::Rerun:
				{
					SN::SN_Error e(false, false, "Rerun request.");
					e.GetSNI_Error()->MakeRerunRequest();
					throw e;
				}
				break;
			case skynet::Quit:
				breakPoint = false;
				if (p_InterruptPoint != SN::EndPoint)
				{
					SN::SN_Error e(false, true, "User abort.");
					SNI_Thread::TopManager()->ErrorHandler()(e);
					m_DebugAction = skynet::StepInto;
				}
				break;
			case skynet::Abort:
				exit(-1);
				break;
			}
		}
		m_ReadyForCommand = true;
		m_ReadyForProcessing = !(breakPoint && processCommand);
		m_ReadyForCommandCond.notify_one();
		return breakPoint && processCommand;
	}

	void SNI_DebugCommand::LoadBreakPoints(const string & p_BreakPointString)
	{
		vector<string> breakPointList;
		Split(p_BreakPointString, ",", breakPointList);
		m_BreakPointSet.clear();
		for (const string &s : breakPointList)
		{
			m_BreakPointSet.insert(s);
		}
	}

	bool SNI_DebugCommand::IsRunning()
	{
		return m_ReadyForProcessing;
	}

	bool SNI_DebugCommand::IsQuitting()
	{
		return m_DebugAction == skynet::Quit;
	}

	bool SNI_DebugCommand::IsExiting()
	{
		unique_lock<mutex> mutex_lock(m_Mutex);
		return m_IsExiting;
	}

	void SNI_DebugCommand::ScheduleCommand(skynet::DebugAction p_DebugAction)
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
		ScheduleCommand(skynet::Run);
	}

	void SNI_DebugCommand::RunToEnd()
	{
		ScheduleCommand(skynet::RunToEnd);
	}

	void SNI_DebugCommand::Debug()
	{
		ScheduleCommand(skynet::Debug);
	}

	void SNI_DebugCommand::CodeBreak()
	{
		ScheduleCommand(skynet::CodeBreak);
	}

	void SNI_DebugCommand::Rerun()
	{
		ScheduleCommand(skynet::Rerun);
	}

	void SNI_DebugCommand::StepOver(long p_StackDepth)
	{
		m_FrameStackDepth = p_StackDepth;
		ScheduleCommand(skynet::StepOver);
	}

	void SNI_DebugCommand::StepInto(SN::DebuggingStop p_DebugStop)
	{
		m_DebugStop = p_DebugStop;
		ScheduleCommand(skynet::StepInto);
	}

	void SNI_DebugCommand::StepOut(long p_StackDepth)
	{
		m_FrameStackDepth = p_StackDepth;
		ScheduleCommand(skynet::StepOut);
	}

	void SNI_DebugCommand::StepParam()
	{
		ScheduleCommand(skynet::StepParameter);
	}

	void SNI_DebugCommand::GotoStepCount(long p_StepCount, long p_ThreadNum)
	{
		unique_lock<mutex> mutex_lock(m_Mutex);
		while (!m_ReadyForCommand)
		{
			m_ReadyForCommandCond.wait(mutex_lock);
		}
		m_DebugAction = skynet::GotoStepCount;
		m_StepCount = p_StepCount;
		m_ThreadNum = p_ThreadNum;
		m_ReadyForProcessing = true;
		m_ReadyForCommand = false;
		m_ReadyForProcessingCond.notify_one();
	}

	void SNI_DebugCommand::Abort()
	{
		m_DebugAction = skynet::Abort;
	}

	void SNI_DebugCommand::Quit()
	{
		ScheduleCommand(skynet::Quit);
	}

	void SNI_DebugCommand::SelectThread(long p_ThreadNum)
	{
		m_ThreadNum = p_ThreadNum;
	}

	string SNI_DebugCommand::Description()
	{
		return m_Description;
	}

	void SNI_DebugCommand::SetDescription(const string & p_Description)
	{
		m_Description = p_Description;
	}
	void SNI_DebugCommand::SetRunning(bool p_Running)
	{
		m_Running = p_Running;
	}
}
