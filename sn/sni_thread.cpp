#include "sni_thread.h"

#include "sn_error.h"
#include "sn_manager.h"

#include "logcontext.h"

#include <chrono>
#include <thread>
#include <WinBase.h>

#include "sn_pch.h"

namespace SNI
{
	thread_local SN::SN_Thread t_Thread;

	/*static*/ vector<SNI_Thread *> SNI_Thread::m_ThreadList;
	/*static*/ mutex SNI_Thread::m_ThreadListMutex;

	/*static*/ long SNI_Thread::m_StackDepth;
	/*static*/ long SNI_Thread::m_GotoThreadNum;
	/*static*/ long SNI_Thread::m_StepCount;
	/*static*/ bool SNI_Thread::m_Running = false;
	/*static*/ thread *SNI_Thread::m_WebServerThread = NULL;
	/*static*/ long SNI_Thread::m_WebServerThreadUsageCount = 0;

	/*static*/ SNI_Thread *SNI_Thread::GetThread()
	{
		return SN::SN_Thread::GetThread().GetSNI_Thread();
	}
	/*static*/ SNI_Thread *SNI_Thread::GetThreadByNumber(long p_ThreadNum)
	{
		return m_ThreadList[p_ThreadNum];
	}

	/*static*/ size_t SNI_Thread::GetNumThreads()
	{
		return m_ThreadList.size();
	}
	/*static*/ SNI_Manager *SNI_Thread::TopManager()
	{
		return GetThread()->GetTopManager();
	}
	/*static*/ SNI_Thread *SNI_Thread::BuildThread()
	{
		SNI_Thread *l_Thread = new SNI_Thread(m_ThreadList.size());
		m_ThreadList.push_back(l_Thread);
		return l_Thread;
	}

	/*static*/ string SNI_Thread::ThreadEnded(long p_ThreadNum)
	{
		stringstream ss;
		WriteThreadEnded(ss, p_ThreadNum);
		return ss.str();
	}

	/*static*/ void SNI_Thread::WriteThreadEnded(ostream & p_Stream, long p_ThreadNum)
	{
		p_Stream << "<!doctype html>\n";
		p_Stream << "<html lang = \"en\">\n";
		p_Stream << "<head>\n";
		p_Stream << "<meta charset = \"utf-8\">\n";
		p_Stream << "<title>Skynet Dashboard</title>\n";
		p_Stream << "<meta name = \"description\" content = \"Skynet\">\n";
		p_Stream << "<meta name = \"author\" content = \"SitePoint\">\n";
		p_Stream << "<link rel = \"stylesheet\" href = \"styles.css\">\n";
		p_Stream << "</head>\n";
		p_Stream << "<body>\n";
		p_Stream << "<h1>Skynet thread " << p_ThreadNum << " not running</h1>\n";
/*		p_Stream << "<div><table>\n";
		p_Stream << "<caption>Threads</caption>\n";
		p_Stream << "<tr>\n";
		string separator;
		bool hasThreads = false;
		for (size_t k = 0; k < m_ThreadList.size(); k++)
		{
			SNI_Thread *l_thread = m_ThreadList[k];
			if (l_thread)
			{
				hasThreads = true;
				l_thread->WriteStepCount(p_Stream);
			}
		}
		p_Stream << "</tr>\n";
		p_Stream << "</table></div>\n";
		if (!hasThreads)
		{
			p_Stream << "<h1>No threads running</h1>\n";
		}
		*/
		p_Stream << "</body>\n";
		p_Stream << "</html>\n";
	}

	void SNI_Thread::ThreadListLock()
	{
		m_ThreadListMutex.lock();
	}

	void SNI_Thread::ThreadListUnlock()
	{
		m_ThreadListMutex.unlock();
	}
	
	SNI_Thread::SNI_Thread(size_t p_ThreadNum)
		: m_ThreadStepCount(0)
		, m_WebServerThreadUsed(false)
		, m_TopManager(NULL)
		, m_ThreadNum(p_ThreadNum)
		, m_Ended(false)
	{
	}

	SNI_Thread::~SNI_Thread()
	{
		ThreadListLock();
		m_ThreadList[m_ThreadNum] = NULL;
		ThreadListUnlock();
	}

	void SNI_Thread::Init()
	{
		m_Ended = false;
		m_FrameList.clear();
	}

	size_t SNI_Thread::GetThreadNum()
	{
		return m_ThreadNum;
	}

	SNI_FrameList & SNI_Thread::GetFrameList()
	{
		return m_FrameList;
	}

	void SNI_Thread::DebugCommand(SN::InterruptPoint p_InterruptPoint, const string &p_Text)
	{
		if (p_InterruptPoint == SN::EndPoint)
		{
			m_Ended = true;
		}
		if (GetTopManager()->HasDebugServer())
		{
			m_ThreadStepCount++;
			m_DebugCommand.SetText(p_Text);
			while (m_DebugCommand.IsBreakPoint(p_InterruptPoint, m_ThreadNum, SNI_Frame::GetFrameStackDepth(), m_ThreadStepCount))
			{
			}
		}
	}
	
	void SNI_Thread::DisplayStepCounts()
	{
		cout << "Step count: " << to_string(m_ThreadNum) << ':' << to_string(m_ThreadStepCount) << '*' << "\n";
	}

	void SNI_Thread::WriteStepCounts(ostream &p_Stream)
	{
		p_Stream << "<div><table>\n";
		p_Stream << "<caption>Threads</caption>\n";
		p_Stream << "<tr>\n";
		string separator;
		for (size_t k = 0; k < m_ThreadList.size(); k++)
		{
			SNI_Thread *l_thread = m_ThreadList[k];
			if (l_thread)
			{
				l_thread->WriteStepCount(p_Stream);
			}
		}
		p_Stream << "</tr>\n";
		p_Stream << "</table></div>\n";
	}

	void SNI_Thread::WriteStepCount(ostream &p_Stream)
	{
		p_Stream << "<td><form action = '/thread' method='get'>\n";
		p_Stream << "<input type = 'submit' name = 'threadnum' value='" << m_ThreadNum << ":" << m_ThreadStepCount << "'/>\n";
		p_Stream << "</form></td>\n";
	}

	ostream * SNI_Thread::CreateLogFile(SN::LoggingLevel p_LoggingLevel)
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

	void SNI_Thread::ScheduleCommand(SN::DebugAction p_DebugAction)
	{
		return m_DebugCommand.ScheduleCommand(p_DebugAction);
	}

	bool SNI_Thread::IsExiting()
	{
		return m_DebugCommand.IsExiting();
	}

	string SNI_Thread::Skynet()
	{
		stringstream ss;
		cout << "Skynet\n";
		WriteWebPage(ss, m_Running);
		return ss.str();
	}

	SNI_Manager * SNI_Thread::GetTopManager(bool p_Create)
	{
		if (p_Create && !m_TopManager)
		{
			m_TopManager = new SNI_Manager();
		}
		return m_TopManager;
	}

	void SNI_Thread::SetTopManager(SNI_Manager * p_TopManager)
	{
		m_TopManager = p_TopManager;
	}

	string SNI_Thread::StartCommand(enum SN::DebugAction p_DebugAction, const string &p_Description)
	{
		stringstream ss;
		m_DebugAction = p_DebugAction;
		cout << p_Description << "\n";
		std::this_thread::yield();
		WriteWebPage(ss, true);
		return ss.str();
	}

	void SNI_Thread::Run()
	{
		m_DebugCommand.Run();
	}

	void SNI_Thread::RunToEnd()
	{
		m_DebugCommand.RunToEnd();
	}

	void SNI_Thread::Debug()
	{
		m_DebugCommand.Debug();
	}

	void SNI_Thread::DebugBreak()
	{
		m_DebugCommand.DebugBreak();
	}

	void SNI_Thread::StepOver(long p_StackDepth)
	{
		m_DebugCommand.StepOver(p_StackDepth);
	}

	void SNI_Thread::StepInto()
	{
		m_DebugCommand.StepInto();
	}

	void SNI_Thread::StepOut(long p_StackDepth)
	{
		m_DebugCommand.StepOut(p_StackDepth);
	}

	void SNI_Thread::StepParam()
	{
		m_DebugCommand.StepParam();
	}

	void SNI_Thread::GotoStepCount(long p_StepCount)
	{
		m_DebugCommand.GotoStepCount(p_StepCount, m_ThreadNum);
	}

	void SNI_Thread::SetMaxStackFrames(long p_StackDepth)
	{
		m_StackDepth = p_StackDepth;
	}

	void SNI_Thread::Quit()
	{
		m_DebugAction = SN::Abort;
	}

	void SNI_Thread::Abort()
	{
		m_DebugAction = SN::Abort;
	}

	string SNI_Thread::RunWeb()
	{
		m_DebugCommand.Run();
		stringstream ss;
		WriteWebPage(ss, true);
		return ss.str();
	}

	string SNI_Thread::RunToEndWeb()
	{
		m_DebugCommand.RunToEnd();
		stringstream ss;
		WriteWebPage(ss, true);
		return ss.str();
	}

	string SNI_Thread::DebugWeb()
	{
		m_DebugCommand.Debug();
		stringstream ss;
		WriteWebPage(ss, true);
		return ss.str();
	}

	string SNI_Thread::DebugBreakWeb()
	{
		m_DebugCommand.DebugBreak();
		stringstream ss;
		WriteWebPage(ss, true);
		return ss.str();
	}

	string SNI_Thread::StepOverWeb(long p_StackDepth)
	{
		m_DebugCommand.StepOver(p_StackDepth);
		stringstream ss;
		WriteWebPage(ss, true);
		return ss.str();
	}

	string SNI_Thread::StepIntoWeb()
	{
		m_DebugCommand.StepInto();
		stringstream ss;
		WriteWebPage(ss, true);
		return ss.str();
	}

	string SNI_Thread::StepOutWeb(long p_StackDepth)
	{
		m_DebugCommand.StepOut(p_StackDepth);
		stringstream ss;
		WriteWebPage(ss, true);
		return ss.str();
	}

	string SNI_Thread::StepParamWeb()
	{
		m_DebugCommand.StepParam();
		stringstream ss;
		WriteWebPage(ss, true);
		return ss.str();
	}

	string SNI_Thread::GotoStepCountWeb(long p_StepCount)
	{
		m_DebugCommand.GotoStepCount(p_StepCount, m_ThreadNum);
		stringstream ss;
		WriteWebPage(ss, true);
		return ss.str();
	}

	string SNI_Thread::SetMaxStackFramesWeb(long p_StackDepth)
	{
		stringstream ss;
		m_StackDepth = p_StackDepth;
		cout << "Set max stack frames\n";
		WriteWebPage(ss, true);
		return ss.str();
	}

	string SNI_Thread::SetThreadNumWeb()
	{
		stringstream ss;
		WriteWebPage(ss, true);
		return ss.str();
	}

	string SNI_Thread::QuitWeb()
	{
		stringstream ss;
		m_DebugAction = SN::Abort;
		cout << "Abort\n";
		WriteShuttingDown(ss);
		return ss.str();
	}

	string SNI_Thread::AbortWeb()
	{
		stringstream ss;
		m_DebugAction = SN::Abort;
		cout << "Abort\n";
		WriteShuttingDown(ss);
		return ss.str();
	}

	void SNI_Thread::Lock()
	{
		m_Mutex.lock();
	}

	void SNI_Thread::Unlock()
	{
		m_Mutex.unlock();
	}

	void SNI_Thread::WriteWebPage(ostream & p_Stream, bool p_Refresh)
	{
		m_DebugCommand.SetRunning(p_Refresh);
		p_Stream << "<!doctype html>\n";
		p_Stream << "<html lang = \"en\">\n";
		p_Stream << "<head>\n";

		p_Stream << "<style>\n";
		p_Stream << "table{ \n";
		p_Stream << "font - family: arial, sans - serif;\n";
		p_Stream << "border - collapse: collapse;\n";
		p_Stream << "width: 100 % ; \n";
		p_Stream << "}\n";
		p_Stream << "td, th{\n";
		p_Stream << "border: 1px solid #dddddd; \n";
		p_Stream << "text - align: left; \n";
		p_Stream << "padding: 8px; \n";
		p_Stream << "}\n";
		p_Stream << "tr:nth - child(even){\n";
		p_Stream << "\n";
		p_Stream << "background - color: #dddddd; \n";
		p_Stream << "}\n";
		p_Stream << "</style>\n";
		p_Stream << "<meta charset = \"utf-8\">\n";
		if (m_DebugCommand.IsRunning())
		{
			p_Stream << "<meta http-equiv = 'refresh' content = '1;url=/skynet'/>\n";
		}
		p_Stream << "<title>Skynet Dashboard</title>\n";
		p_Stream << "<meta name = \"description\" content = \"Skynet\">\n";
		p_Stream << "<meta name = \"author\" content = \"SitePoint\">\n";
		p_Stream << "<link rel = \"styles,hheet\" href = \"styles.css\">\n";
		p_Stream << "</head>\n";
		p_Stream << "<body>\n";
		p_Stream << "<script src = 'js/scripts.js'></script>\n";

		p_Stream << "<h1>Skynet Dashboard";
		SNI_Manager *manager = GetTopManager(false);
		if (manager)
		{
			p_Stream << " - " << manager->Description();
		}
		if (m_DebugCommand.IsRunning())
		{
			p_Stream << " - Running";
		}
		if (m_Ended)
		{
			p_Stream << " - Thread ended";
		}
		p_Stream << "</h1>\n";
		WriteCommands(p_Stream);
		WriteStepCounts(p_Stream);
		if (manager)
		{
			WriteWebStack(p_Stream, m_StackDepth, manager->DebugFieldWidth());
		}
		p_Stream << "</body>\n";
		p_Stream << "</html>\n";
	}

	void SNI_Thread::WriteShuttingDown(ostream & p_Stream)
	{
		p_Stream << "<!doctype html>\n";
		p_Stream << "<html lang = \"en\">\n";
		p_Stream << "<head>\n";
		p_Stream << "<meta charset = \"utf-8\">\n";
		p_Stream << "<title>Skynet Dashboard</title>\n";
		p_Stream << "<meta name = \"description\" content = \"Skynet\">\n";
		p_Stream << "<meta name = \"author\" content = \"SitePoint\">\n";
		p_Stream << "<link rel = \"stylesheet\" href = \"styles.css\">\n";
		p_Stream << "</head>\n";
		p_Stream << "<body>\n";
		p_Stream << "<h1>Skynet shutting down</h1>\n";
		p_Stream << "</body>\n";
		p_Stream << "</html>\n";
	}

	void SNI_Thread::WriteCommands(ostream & p_Stream)
	{
		p_Stream << "<div><table bgcolor='silver'><tr>\n";
		WriteSubmit(p_Stream, "run", "Run", "Run");
		WriteSubmit(p_Stream, "runtoend", "End", "Run to end");
		WriteSubmit(p_Stream, "debug", "Debug", "Run to breakpoint");
		WriteSubmit(p_Stream, "debugbreak", "Debug break", "Debug break C++");
		WriteSubmit(p_Stream, "stepover", "Step over", "Step over call");
		WriteSubmit(p_Stream, "stepinto", "Step into", "Step into call");
		WriteSubmit(p_Stream, "stepout", "Step out", "Step out of call");
		WriteSubmit(p_Stream, "stepparam", "Step parameter", "Step into parameters");
		WriteGotoStepCount(p_Stream);
		WriteSetMaxStackFrames(p_Stream);
		WriteSubmit(p_Stream, "quit", "Quit", "Abort program");
		p_Stream << "</table></tr></div>\n";
	}

	void SNI_Thread::WriteSubmit(ostream &p_Stream, const string &p_Action, const string &p_Name, const string &p_Description)
	{
		p_Stream << "<td valign = 'top'>\n";
		p_Stream << p_Description << "<br>\n";
		p_Stream << "<form action = '/" << p_Action << "' method='get'>\n";
		p_Stream << "<input type = 'hidden' name = 'thread' value = '" << m_ThreadNum << "'>\n";
		p_Stream << "<input type = 'hidden' name = 'stackdepth' value = '" << m_FrameList.size() << "'>\n";
		p_Stream << "<input type = 'submit' value = '" << p_Name << "'>\n";
		p_Stream << "</form>\n";
		p_Stream << "</td>\n";
	}

	void SNI_Thread::WriteGotoStepCount(ostream &p_Stream)
	{
		p_Stream << "<td valign = 'top'>\n";
		p_Stream << "<form action = '/gotostepcount' method='get'>\n";
		p_Stream << "<details>";
		p_Stream << "<summary>";
		p_Stream << "Goto step count<br>\n";
		p_Stream << "<input type = 'submit' value = 'Goto'><br>\n";
		p_Stream << "</summary>";
		p_Stream << "Step count : <br>\n";
		p_Stream << "<input type = 'text' name = 'stepcount'>\n";
		p_Stream << "<br>\n";
		p_Stream << "Thread : <br>\n";
		p_Stream << "<input type = 'text' name = 'threadnum' value = '0'>\n";
		p_Stream << "</details>";
		p_Stream << "</form>\n";
		p_Stream << "</td>\n";
	}

	void SNI_Thread::WriteSetMaxStackFrames(ostream &p_Stream)
	{
		p_Stream << "<td valign = 'top'>\n";
		p_Stream << "<form action = '/maxstackframes' method='get'>\n";
		p_Stream << "<details>";
		p_Stream << "<summary>";
		p_Stream << "Max stack frames<br>\n";
		p_Stream << "<input type = 'submit' value = 'Stack depth'><br>\n";
		p_Stream << "</summary>";
		p_Stream << "<input type = 'hidden' name = 'thread' value = '" << m_ThreadNum << "'>\n";
		p_Stream << "Number of stack frames to display :<br>\n";
		p_Stream << "<input type = 'text' name = 'maxstackframes'>\n";
		p_Stream << "</details>";
		p_Stream << "</form>\n";
		p_Stream << "</td>\n";
	}

	void SNI_Thread::WriteWebStack(ostream &p_Stream, long p_Depth, size_t p_DebugFieldWidth)
	{
		p_Stream << "<td><table>\n";
		p_Stream << "<caption>Thread " << m_ThreadNum << "</caption>";
		size_t base = 0;
		if (0 < p_Depth)
		{
			base = m_FrameList.size() - p_Depth;
		}
		Lock();
		for (size_t j = m_FrameList.size(); j > base; j--)
		{
			p_Stream << "<tr><td align = \"center\">\n";
			m_FrameList[j - 1]->WriteWebFrame(p_Stream, j, p_DebugFieldWidth);
			p_Stream << "<tr><td>\n";
		}
		Unlock();
		p_Stream << "</table></td>\n";
	}

	void SNI_Thread::DisplayFrameStack(long p_Depth)
	{
		size_t base = 0;
		if (0 < p_Depth)
		{
			base = m_FrameList.size() - p_Depth;
		}
		for (size_t j = base; j < m_FrameList.size(); j++)
		{
			LOG(WriteFrame(this, SN::DebugLevel, m_FrameList[j]));
		}
	}

	SNI_Variable *SNI_Thread::LookupVariable(const string &p_Name)
	{
		for (SNI_Frame *f : m_FrameList)
		{
			SNI_Variable *v = f->LookupVariableInFrame(p_Name);
			if (v)
			{
				return v;
			}
		}
		return NULL;
	}


	SNI_Frame * SNI_Thread::Top()
	{
		if (m_FrameList.size())
		{
			return m_FrameList.back();
		}
		return NULL;
	}

	void SNI_Thread::PushFrame(SNI_Frame *p_Frame)
	{
		Lock();
		m_FrameList.push_back(p_Frame);
		Unlock();
	}

	void SNI_Thread::PopFrame()
	{
		Lock();
		m_FrameList.pop_back();
		Unlock();
	}

	size_t SNI_Thread::GetFrameStackDepth()
	{
		return m_FrameList.size();
	}
}
