#include "sni_thread.h"

#include "sn_error.h"
#include "sn_manager.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include <chrono>
#include <thread>
#include <WinBase.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "sn_pch.h"

namespace SNI
{
	thread_local SNI_Thread *t_Thread = nullptr;

	/*static*/ long SNI_Thread::m_GotoThreadNum;
	/*static*/ long SNI_Thread::m_StepCount;
	/*static*/ bool SNI_Thread::m_Running = false;
	/*static*/ thread *SNI_Thread::m_WebServerThread = NULL;
	/*static*/ long SNI_Thread::m_WebServerThreadUsageCount = 0;

	/*static*/ SNI_Thread* SNI_Thread::GetThread() {
		if (!t_Thread) {
			t_Thread = new SNI_Thread();
		}
		return t_Thread;
	}
	/*static*/ void SNI_Thread::ResetThread() {
		delete t_Thread;
		t_Thread = nullptr;
	}
	/*static*/ void SNI_Thread::ClearThread() {
		t_Thread = nullptr;
	}

	/*static*/ SNI_Manager *SNI_Thread::TopManager()
	{
		return GetThread()->GetTopManager();
	}

	/*static*/ std::string SNI_Thread::ThreadEnded(long p_ThreadNum)
	{
		stringstream ss;
		WriteThreadEnded(ss, p_ThreadNum);
		return ss.str();
	}

	/*static*/ void SNI_Thread::WriteThreadEnded(std::ostream & p_Stream, long p_ThreadNum)
	{
		p_Stream << "<!doctype html>\n";
		p_Stream << "<html lang = \"en\">\n";
		p_Stream << "<head>\n";
		p_Stream << "<meta charset = \"utf-8\">\n";
		p_Stream << "<title>Skynet Dashboard</title>\n";
		p_Stream << "<meta name = \"description\" content = \"Skynet\">\n";
		p_Stream << "<meta name = \"author\" content = \"P.J.Driscoll\">\n";
		p_Stream << "<link rel = \"stylesheet\" type=\"text/css\" href = \"style.css\">\n";
		p_Stream << "</head>\n";
		p_Stream << "<body>\n";
		p_Stream << "<h1>Skynet thread " << p_ThreadNum << " not running</h1>\n";
		p_Stream << "</body>\n";
		p_Stream << "</html>\n";
	}

	/*static*/ void SNI_Thread::WriteW3Credentials(std::ostream &p_Stream)
	{
		p_Stream << "<div><p>\n";
		p_Stream << "<a href='http://jigsaw.w3.org/css-validator/check/referer'>\n";
		p_Stream << "<img style='border:0;width:88px;height:31px'\n";
		p_Stream << "src='http://jigsaw.w3.org/css-validator/images/vcss-blue'\n";
		p_Stream << "alt='Valid CSS!' />\n";
		p_Stream << "</a>\n";
		p_Stream << "</p></div>\n";
	}

	SNI_Thread::SNI_Thread()
		: m_ThreadStepCount(0)
		, m_LastThreadStepCount(0)
		, m_WebServerThreadUsed(false)
		, m_TopManager(NULL)
		, m_MaxStackFrames(-1)
		, m_Closing(false)
		, m_DebugAction(skynet::StepInto)
		, m_Ended(false)
		, m_DefineId(0)
		, m_WorldSetChangedList(NULL)
		, m_WorldSetProcessMap(NULL)
		, m_CodeBreakScheduled(false)
		, m_Error(NULL)
		, m_Processor(NULL)
		, m_User(nullptr)
	{
	}

	SNI_Thread::~SNI_Thread()
	{
	}

	void SNI_Thread::Init()
	{
		m_Ended = false;
		m_FrameList.clear();
		m_Processor = NULL;
	}

	void SNI_Thread::Clear()
	{
		m_Processor = NULL;
		m_FrameList.clear();
		m_WorldSetProcessMap = NULL;
		SNI_Log::GetLog()->ClearLogExpressions();
		m_Error = NULL;
	}

	size_t SNI_Thread::GetThreadNum()
	{
		return m_ThreadNum;
	}

	SNI_FrameList & SNI_Thread::GetFrameList()
	{
		return m_FrameList;
	}
	
	size_t SNI_Thread::GetStepCount()
	{
		return m_ThreadStepCount;
	}

	void SNI_Thread::SaveStepCount()
	{
		m_LastThreadStepCount = m_ThreadStepCount;
	}

	void SNI_Thread::ResetStepCount()
	{
		m_ThreadStepCount = m_LastThreadStepCount;
		m_FrameList.resize(1);
	}

	void SNI_Thread::Breakpoint(SN::DebuggingStop p_DebuggingStop, SN::BreakId p_BreakId, const std::string &p_TypeName, const std::string &p_Description, const SNI_Expression *p_DebugSource, SN::InterruptPoint p_InterruptPoint, const std::string &p_DebugId)
	{
		m_ThreadStepCount++;
		SNI_Manager *topManager = GetTopManager(false);
		if (topManager && topManager->HasDebugServer())
		{
			m_DebugCommand.SetDescription(p_Description);
			std::string debugId;
			switch (p_InterruptPoint)
			{
			case SN::MirrorPoint:
				debugId = "MIR";
				break;
			case SN::ErrorPoint:
				debugId = "Error";
				break;
			case SN::UserPoint:
				debugId = "user";
				break;
			case SN::ClonePoint:
				debugId = "clone";
				break;
			case SN::DelayedPoint:
				debugId = "delayed";
				break;
			case SN::CodePoint:
				debugId = p_DebugId;
				break;
			default:
				if (p_DebugSource)
				{
					debugId = p_DebugSource->GetDebugId();
				}
				break;
			}
			std::string breakPoint = MakeBreakPoint(debugId, p_BreakId);
			std::string breakPointJS = MakeBreakPointJS(debugId, p_BreakId);
			SetThreadBreakPoint(breakPoint, breakPointJS);
			while (m_DebugCommand.IsBreakPoint(p_InterruptPoint, m_ThreadNum, SNI_Frame::GetFrameStackDepth(), m_ThreadStepCount, breakPoint, p_DebuggingStop))
			{
			}
		}
		if (m_CodeBreakScheduled)
		{
			// Code break
#ifdef _DEBUG
			// DebugBreak(); // Or just set breakpoint on the next line. It is easier.
#endif
			m_CodeBreakScheduled = false;
			// Shift F11 (step out) will return you to the current code.
		}
	}

	void SNI_Thread::DisplayStepCounts()
	{
		cout << "Step count: " << std::to_string(m_ThreadNum) << ':' << std::to_string(m_ThreadStepCount) << '*' << "\n";
	}

	void SNI_Thread::WriteStepCount(std::ostream &p_Stream)
	{
		p_Stream << "<td><form action = '/thread' method='get'>\n";
		p_Stream << "<input type = 'submit' name = 'threadnum' value='" << m_ThreadNum << ":" << m_ThreadStepCount << "'/>\n";
		p_Stream << "</form></td>\n";
	}

	std::ostream * SNI_Thread::CreateLogFile(SN::LoggingLevel p_LoggingLevel)
	{
		std::string currentDirectory = CurrentWorkingDirectory();

		std::string timeId = GetFormattedTime();
		std::string logIndex = SN::GetLoggingLevelCode(p_LoggingLevel);
		std::string fileName = SNI_Thread::TopManager()->LogFilePath() + logIndex + /*"_" + timeId +*/ ".log"; //" + timeId + "
		fstream *logFile = new fstream;
		logFile->open(fileName.data(), ios::out | ios::trunc);
		if (!logFile->is_open())
		{
			delete logFile;
			throw new SNI_Error(false, false, "Log file " + fileName + " not opened. Check folders exist in path from " + currentDirectory);
		}
		return logFile;
	}

	void SNI_Thread::ScheduleCommand(skynet::DebugAction p_DebugAction)
	{
		return m_DebugCommand.ScheduleCommand(p_DebugAction);
	}

	bool SNI_Thread::IsExiting()
	{
		return m_DebugCommand.IsExiting();
	}

	std::string SNI_Thread::DashboardJS(enum DisplayOptionType p_OptionType)
	{
		stringstream ss;
		cout << "DashboardJS\n";
		SNI_Manager *manager = GetTopManager(false);
		if (manager)
		{
			SNI_DisplayOptions l_DisplayOptions(p_OptionType);
			WriteDashboardJS(ss, l_DisplayOptions);
		}
		else
		{
			ss << "{\"records\":[]}\n";
		}
		return ss.str();
	}

	std::string SNI_Thread::StackJS(long p_MaxStackFrame, long p_StartStackFrame, long p_StartStepCount, enum DisplayOptionType p_OptionType)
	{
		stringstream ss;
		SNI_Manager *manager = GetTopManager(false);
		cout << "StackJS\n";
		if (manager && (p_StartStepCount == 0 || p_StartStepCount == m_ThreadStepCount))
		{
			SNI_DisplayOptions l_DisplayOptions(p_OptionType);
			WriteStackJS(ss, p_MaxStackFrame, p_StartStackFrame, manager->DebugFieldWidth(), l_DisplayOptions);
		}
		else
		{
			ss << "{\"records\":[]}\n";
		}
		return ss.str();
	}

	std::string SNI_Thread::CallStackJS(long p_MaxCallStackFrame, long p_StartCallStackFrame, long p_StartStepCount, enum DisplayOptionType p_OptionType)
	{
		stringstream ss;
		cout << "CallStackJS\n";
		if (p_StartStepCount == 0 || p_StartStepCount == m_ThreadStepCount)
		{
			SNI_DisplayOptions l_DisplayOptions(p_OptionType);
			WriteCallStackJS(ss, p_MaxCallStackFrame, p_StartCallStackFrame, l_DisplayOptions);
		}
		else
		{
			ss << "{\"records\":[]}\n";
		}
		return ss.str();
	}

	std::string SNI_Thread::WatchListJS(long p_StartStepCount, enum DisplayOptionType p_OptionType)
	{
		stringstream ss;
		SNI_Manager* manager = GetTopManager(false);
		if (manager && (p_StartStepCount == 0 || p_StartStepCount == m_ThreadStepCount))
		{
			SNI_DisplayOptions l_DisplayOptions(p_OptionType);
			WriteWatchListJS(ss, manager->DebugFieldWidth(), l_DisplayOptions);
		}
		else
		{
			ss << "{\"records\":[]}\n";
		}
		return ss.str();
	}

	std::string SNI_Thread::LogJS(long p_MaxLogEntries, long p_StartLogEntries, long p_StartStepCount)
	{
		stringstream ss;
		cout << "LogJS\n";
		if (p_StartStepCount == 0 || p_StartStepCount == m_ThreadStepCount)
		{
			WriteLogJS(ss, p_MaxLogEntries, p_StartLogEntries);
		}
		else
		{
			ss << "{\"records\":[]}\n";
		}
		return ss.str();
	}

	std::string SNI_Thread::DerivationJS(long p_MaxLogEntries)
	{
		//stringstream ss;
		//WriteDerivationJS(ss, p_MaxLogEntries);
		return ""; // ss.str();
	}

	std::string SNI_Thread::CodeJS(long p_MaxLogEntries, long p_StartCode, long p_StepCount, enum DisplayOptionType p_OptionType)
	{
		stringstream ss;
		SNI_DisplayOptions displayOptions(p_OptionType);
		cout << "LogJS\n";
		if (p_StepCount == 0 || p_StepCount == m_ThreadStepCount)
		{
			WriteCodeJS(ss, p_MaxLogEntries, p_StartCode, displayOptions);
		}
		else
		{
			ss << "{\"records\":[]}\n";
		}
		return ss.str();
	}

	std::string SNI_Thread::ErrorJS(enum DisplayOptionType p_OptionType)
	{
		if (m_Error)
		{
			Lock();
			stringstream ss;
			SNI_DisplayOptions displayOptions(p_OptionType);
			m_Error->WriteJSON(ss, displayOptions);
			Unlock();
			return ss.str();
		}
		return "{\"description\":\"\",\"callhistory\":[]}";
	}

	std::string SNI_Thread::WorldSetsJS(DisplayOptionType p_OptionType)
	{
		stringstream ss;
		SNI_DisplayOptions displayOptions(p_OptionType);
		WriteWorldSetsJS(ss, displayOptions);
		return ss.str();
	}

	std::string SNI_Thread::DelayedJS(DisplayOptionType p_OptionType)
	{
		stringstream ss;
		SNI_DisplayOptions displayOptions(p_OptionType);
		if (m_Processor)
		{
			m_Processor->WriteJSON(ss, displayOptions);
		}
		else
		{
			ss << "{\"records\":[]}\n";
		}
		return ss.str();
	}

	std::string SNI_Thread::Skynet(enum DisplayOptionType p_OptionType)
	{
		stringstream ss;
		cout << "Skynet\n";
		WriteWebPage(ss, m_Running, p_OptionType);
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

	long SNI_Thread::GetDefineId()
	{
		return m_DefineId;
	}

	void SNI_Thread::UpdateIncrementId()
	{
		m_DefineId++;
	}

	SNI_WorldSetList *SNI_Thread::GetWorldSetChanged()
	{
		if (!m_WorldSetChangedList)
		{
			m_WorldSetChangedList = new SNI_WorldSetList;
		}
		return m_WorldSetChangedList;
	}
	
	SNI_WorldSetMap *SNI_Thread::GetWorldSetProcessMap()
	{
		if (!m_WorldSetProcessMap)
		{
			m_WorldSetProcessMap = new SNI_WorldSetMap;
		}
		return m_WorldSetProcessMap;
	}

	void SNI_Thread::ClearDependencyChecks()
	{
		delete m_WorldSetProcessMap;
		delete m_WorldSetChangedList;
		m_WorldSetProcessMap = NULL;
		m_WorldSetChangedList = NULL;
	}

	void SNI_Thread::ScheduleCodeBreak()
	{
		m_CodeBreakScheduled = true;
	}

	void SNI_Thread::RegisterChange(SNI_Variable * p_Variable)
	{
/* Not working properly. Expensive on memory.
		if (p_Variable && p_Variable->GetName().substr(0, 1) != "_")
		{
			ChangeKey changeKey(m_ThreadStepCount, p_Variable);
			m_ChangeMutex.lock();
			auto it = m_ChangeMap.find(changeKey);
			if (it == m_ChangeMap.end())
			{
				ChangeValue changeValue(p_Variable);
				m_ChangeMap[changeKey] = changeValue;
			}
			m_ChangeMutex.unlock();
		}
*/
	}

	std::string SNI_Thread::ChangeHistoryJS(enum DisplayOptionType p_OptionType, size_t p_ColumnWidth, size_t p_FromStep, size_t p_ToStep)
	{
		stringstream ss;
		SNI_Manager *manager = GetTopManager(false);
		cout << "ChangeHistoryJS\n";
		if (manager)
		{
			SNI_DisplayOptions l_DisplayOptions(p_OptionType);
			WriteChangeHistoryJS(ss, l_DisplayOptions, p_ColumnWidth, p_FromStep, p_ToStep);
		}
		else
		{
			ss << "{\"records\":[]}\n";
		}
		return ss.str();
	}

	void SNI_Thread::WriteChangeHistoryJS(std::ostream &p_Stream, SNI::SNI_DisplayOptions &p_DisplayOptions, size_t p_ColumnWidth, size_t p_FromStep, size_t p_ToStep)
	{
		p_Stream << "{\"records\":[";
		set<std::string> rowChanges;
		std::vector<std::string> rowVector;
		std::string delimeter1 = "\n";
		m_ChangeMutex.lock();
		SNI_DisplayOptions textDisplayOptions(doTextOnly);

		for (size_t stepCount = p_FromStep; stepCount <= m_ThreadStepCount && stepCount <= p_ToStep ; stepCount++)
		{
			bool changes = false;
			for (auto it = m_ChangeMap.lower_bound(ChangeKey(stepCount));
				it != m_ChangeMap.end() && it->first.GetStepCount() == stepCount;
				it++)
			{
				std::string name = it->first.GetName();
				changes = true;
				if (rowChanges.find(name) == rowChanges.end())
				{
					rowChanges.insert(name);
					rowVector.push_back(name);
				}
			}
			if (changes)
			{
				p_Stream << delimeter1 << "\t{\n";
				delimeter1 = ",\n";
				p_Stream << "\t\t\"stepcount\":" << std::to_string(stepCount) << ",\n";
				p_Stream << "\t\t\"variables\":[";
				std::string delimeter2 = "\n";
				for (const std::string &name : rowVector)
				{
					auto itLoop = m_ChangeMap.find(ChangeKey(stepCount, name));
					p_Stream << delimeter2 << "\t\t\t{\n";
					delimeter2 = ",\n";
					if (itLoop == m_ChangeMap.end())
					{
						p_Stream << "\t\t\t\t\"name\" : \"\",\n";
						p_Stream << "\t\t\t\t\"nabbreviation\" : \"\",\n";
						p_Stream << "\t\t\t\t\"framename\" : \"\",\n";
						p_Stream << "\t\t\t\t\"vabbreviation\" : \"\",\n";
						p_Stream << "\t\t\t\t\"value\" : \"\"\n";
					}
					else
					{
						std::string frameName = itLoop->second.GetFrameName();
						std::string nabbreviation = frameName.substr(0, p_ColumnWidth-3);
						if (nabbreviation.length() == frameName.length())
						{
							nabbreviation = "";
						}

						std::string htmlDescription = itLoop->second.DisplaySN(p_DisplayOptions);
						std::string textDescription = itLoop->second.DisplaySN(textDisplayOptions);
						std::string vabbreviation = textDescription.substr(0, p_ColumnWidth-3);
						if (vabbreviation.length() == textDescription.length())
						{
							vabbreviation = "";
						}
			
						p_Stream << "\t\t\t\t\"name\" : \"" << name << "\",\n";
						p_Stream << "\t\t\t\t\"nabbreviation\" : \"" << nabbreviation << "\",\n";
						p_Stream << "\t\t\t\t\"framename\" : \"" << frameName << "\",\n";
						p_Stream << "\t\t\t\t\"vabbreviation\" : \"" << EscapeStringToJSON(vabbreviation) << "\",\n";
						p_Stream << "\t\t\t\t\"value\" : \"" << EscapeStringToJSON(htmlDescription) << "\"\n";
					}
					p_Stream << "\t\t\t}";
				}
				p_Stream << "\n\t\t]\n";
				p_Stream << "\n\t}";
			}
		}
		m_ChangeMutex.unlock();
		p_Stream << "\n]}\n";
	}

	SNI_User * SNI_Thread::GetUser()
	{
		return m_User;
	}

	void SNI_Thread::SetUser(SNI_User* p_User)
	{
		m_User = p_User;
		p_User->AddThread(this);
		m_ThreadNum = p_User->GetNumThreads()-1;
	}

	std::string SNI_Thread::StartCommand(enum skynet::DebugAction p_DebugAction, const std::string &p_Description, enum DisplayOptionType p_OptionType)
	{
		stringstream ss;
		m_DebugAction = p_DebugAction;
		cout << p_Description << "\n";
		std::this_thread::yield();
		WriteWebPage(ss, true, p_OptionType);
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

	void SNI_Thread::LoadBreakPoints(const std::string &p_BreakPointString)
	{
		m_DebugCommand.LoadBreakPoints(p_BreakPointString);
	}

	void SNI_Thread::RegisterError(SNI_Error *p_Error)
	{
		m_Error = p_Error;
		Breakpoint(SN::ErrorStop, SN::ErrorId, "", "Error", NULL, SN::ErrorPoint);
	}

	void SNI_Thread::Debug()
	{
		m_DebugCommand.Debug();
	}

	void SNI_Thread::CodeBreak()
	{
		m_DebugCommand.CodeBreak();
	}

	void SNI_Thread::Rerun()
	{
		m_DebugCommand.Rerun();
	}

	void SNI_Thread::StepOver()
	{
		m_DebugCommand.StepOver(m_FrameList.size());
	}

	void SNI_Thread::StepInto(SN::DebuggingStop p_DebugStop)
	{
		m_DebugCommand.StepInto(p_DebugStop);
	}

	void SNI_Thread::StepOut()
	{
		m_DebugCommand.StepOut(m_FrameList.size()-1);
	}

	void SNI_Thread::StepOutCall()
	{
		size_t lastCode = m_FrameList.size();
		for (auto it = m_FrameList.rbegin(); it != m_FrameList.rend(); it++)
		{
			if ((*it)->HasCode())
			{
				break;
			}
			lastCode--;
		}
		m_DebugCommand.StepOut(lastCode-1);
	}


	void SNI_Thread::StepParam()
	{
		m_DebugCommand.StepParam();
	}

	void SNI_Thread::GotoStepCount(size_t p_StepCount)
	{
		if (m_ThreadStepCount < p_StepCount)
		{
			m_DebugCommand.GotoStepCount(skynet::GotoStepCount, p_StepCount, m_ThreadNum);
		}
		else if (m_LastThreadStepCount < p_StepCount)
		{
			m_DebugCommand.GotoStepCount(skynet::GoBackToStepCount, p_StepCount, m_ThreadNum);
		}
	}

	void SNI_Thread::SetMaxStackFrames(long p_MaxStackFrames)
	{
		m_MaxStackFrames = p_MaxStackFrames;
	}

	void SNI_Thread::Quit()
	{
		m_DebugAction = skynet::Quit;
	}

	void SNI_Thread::Abort()
	{
		m_DebugAction = skynet::Abort;
	}

	std::string SNI_Thread::RunWeb(enum DisplayOptionType p_OptionType)
	{
		m_DebugCommand.Run();
		stringstream ss;
		WriteWebPage(ss, true, p_OptionType);
		return ss.str();
	}

	std::string SNI_Thread::RunToEndWeb(enum DisplayOptionType p_OptionType)
	{
		m_DebugCommand.RunToEnd();
		stringstream ss;
		WriteWebPage(ss, true, p_OptionType);
		return ss.str();
	}

	std::string SNI_Thread::DebugWeb(enum DisplayOptionType p_OptionType)
	{
		m_DebugCommand.Debug();
		stringstream ss;
		WriteWebPage(ss, true, p_OptionType);
		return ss.str();
	}

	std::string SNI_Thread::CodeBreakWeb(enum DisplayOptionType p_OptionType)
	{
		m_DebugCommand.CodeBreak();
		stringstream ss;
		WriteWebPage(ss, true, p_OptionType);
		return ss.str();
	}

	std::string SNI_Thread::StepOverWeb(enum DisplayOptionType p_OptionType)
	{
		m_DebugCommand.StepOver(m_FrameList.size());
		stringstream ss;
		WriteWebPage(ss, true, p_OptionType);
		return ss.str();
	}

	std::string SNI_Thread::StepIntoWeb(enum DisplayOptionType p_OptionType)
	{
		m_DebugCommand.StepInto(SN::DebugStop);
		stringstream ss;
		WriteWebPage(ss, true, p_OptionType);
		return ss.str();
	}

	std::string SNI_Thread::StepOutWeb(enum DisplayOptionType p_OptionType)
	{
		m_DebugCommand.StepOut(m_FrameList.size());
		stringstream ss;
		WriteWebPage(ss, true, p_OptionType);
		return ss.str();
	}

	std::string SNI_Thread::StepParamWeb(enum DisplayOptionType p_OptionType)
	{
		m_DebugCommand.StepParam();
		stringstream ss;
		WriteWebPage(ss, true, p_OptionType);
		return ss.str();
	}

	std::string SNI_Thread::GotoStepCountWeb(long p_StepCount, enum DisplayOptionType p_OptionType)
	{
		m_DebugCommand.GotoStepCount(skynet::GotoStepCount, p_StepCount, m_ThreadNum);
		stringstream ss;
		WriteWebPage(ss, true, p_OptionType);
		return ss.str();
	}

	std::string SNI_Thread::SetMaxStackFramesWeb(long p_MaxStackFrame, enum DisplayOptionType p_OptionType)
	{
		stringstream ss;
		m_MaxStackFrames = p_MaxStackFrame;
		cout << "Set max stack frames\n";
		WriteWebPage(ss, true, p_OptionType);
		return ss.str();
	}

	std::string SNI_Thread::SetThreadNumWeb(enum DisplayOptionType p_OptionType)
	{
		stringstream ss;
		WriteWebPage(ss, true, p_OptionType);
		return ss.str();
	}

	std::string SNI_Thread::QuitWeb(enum DisplayOptionType p_OptionType)
	{
		stringstream ss;
		m_DebugCommand.Quit();
		cout << "Quit\n";
		WriteWebPage(ss, true, p_OptionType);
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

	void SNI_Thread::WriteWebPage(std::ostream& p_Stream, bool p_Refresh, DisplayOptionType p_OptionType)
	{
		m_DebugCommand.SetRunning(p_Refresh);
		p_Stream << "<!doctype html>\n";
		p_Stream << "<html lang = \"en\">\n";
		p_Stream << "<head>\n";
		p_Stream << "<meta charset = \"utf-8\">\n";
		if (m_DebugCommand.IsRunning())
		{
			p_Stream << "<meta http-equiv = 'refresh' content = '1;url=/skynet'/>\n";
		}
		p_Stream << "<title>Skynet Dashboard</title>\n";
		p_Stream << "<meta name = \"description\" content = \"Skynet\">\n";
		p_Stream << "<meta name = \"author\" content = \"P.J.Driscoll\">\n";
		p_Stream << "<link rel = 'icon' href = 'favicon.png' sizes = '32x32' type = 'image/png'>\n";
		p_Stream << "<link rel = 'stylesheet' type='text/css' href = 'style.css'>\n";
		p_Stream << "</head>\n";
		p_Stream << "<body>\n";

		p_Stream << "<h1>Skynet Dashboard";
		SNI_Manager *manager = GetTopManager(false);
		if (manager)
		{
			p_Stream << " - " << manager->Description();
			if (m_MaxStackFrames == -1)
			{
				m_MaxStackFrames = manager->MaxStackFrames();
			}
		}
		if (m_DebugCommand.IsQuitting())
		{
			p_Stream << " - Quitting";
		}
		else if (m_DebugCommand.IsRunning())
		{
			p_Stream << " - Running";
		}
		if (m_Ended)
		{
			p_Stream << " - Thread ended";
		}
		p_Stream << "</h1>\n";
		WriteCommands(p_Stream);
		SNI::SNI_User::GetCurrentUser()->WriteStepCounts(p_Stream);
		if (manager)
		{
			SNI_DisplayOptions l_DisplayOptions(p_OptionType);
			p_Stream << "<table class='group'><tr>\n";
			p_Stream << "<td><div class='group'>\n";
			if (!m_DebugCommand.IsQuitting())
			{
				WriteWebStack(p_Stream, m_MaxStackFrames, manager->DebugFieldWidth(), l_DisplayOptions);
			}
			p_Stream << "</div></td>\n";
			p_Stream << "<td><div class='group'>\n";
			SNI_Log::GetLog()->LogTableToStream(p_Stream, m_MaxStackFrames * 4);
			p_Stream << "</div></td>\n";
			p_Stream << "</tr></table>\n";
		}
		WriteW3Credentials(p_Stream);
		p_Stream << "</body>\n";
		p_Stream << "</html>\n";
	}

	void SNI_Thread::WriteShuttingDown(std::ostream & p_Stream)
	{
		p_Stream << "<!doctype html>\n";
		p_Stream << "<html lang = \"en\">\n";
		p_Stream << "<head>\n";
		p_Stream << "<meta charset = \"utf-8\">\n";
		p_Stream << "<title>Skynet Dashboard</title>\n";
		p_Stream << "<meta name = \"description\" content = \"Skynet\">\n";
		p_Stream << "<meta name = \"author\" content = \"P.J.Driscoll\">\n";
		p_Stream << "<link rel = 'icon' href = 'favicon.png' sizes = '32x32' type = 'image/png'>\n";
		p_Stream << "<link rel = 'stylesheet' type='text/css' href = 'style.css'>\n";
		p_Stream << "</head>\n";
		p_Stream << "<body>\n";
		p_Stream << "<h1>Skynet shutting down</h1>\n";
		p_Stream << "</body>\n";
		p_Stream << "</html>\n";
	}

	void SNI_Thread::WriteCommands(std::ostream & p_Stream)
	{
		p_Stream << "<div><table class='command'><tr>\n";
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
		WriteSubmit(p_Stream, "quit", "Quit", "Abort thread");
		p_Stream << "</tr></table></div>\n";
	}

		// dog WriteSubmitJS(p_Stream, "run", "Run", "Run");
		// dog WriteGotoStepCountJS(p_Stream);
		// dog WriteSetMaxStackFramesJS(p_Stream);

	void SNI_Thread::WriteSubmit(std::ostream &p_Stream, const std::string &p_Action, const std::string &p_Name, const std::string &p_Description)
	{
		p_Stream << "<td>\n";
		p_Stream << p_Description << "<br/>\n";
		p_Stream << "<form action = '/" << p_Action << "' method='get'>\n";
		p_Stream << "<input type = 'hidden' name = 'thread' value = '" << m_ThreadNum << "' />\n";
		p_Stream << "<input type = 'hidden' name = 'stackdepth' value = '" << m_FrameList.size() << "' />\n";
		p_Stream << "<input type = 'submit' value = '" << p_Name << "' />\n";
		p_Stream << "</form>\n";
		p_Stream << "</td>\n";
	}

	/*static*/ void SNI_Thread::WriteSubmitJS(std::ostream &p_Stream, const std::string &p_Action, const std::string &p_Name, const std::string &p_Description)
	{
		p_Stream << "<td>\n";
		p_Stream << "<form ng-submit = 'submit(\"" << p_Action << "\")'>\n";
		p_Stream << p_Description << "<br/>\n";
		p_Stream << "<input type = 'submit' value = '" << p_Name << "' />\n";
		p_Stream << "</form>\n";
		p_Stream << "</td>\n";
	}

	void SNI_Thread::WriteGotoStepCount(std::ostream &p_Stream)
	{
		p_Stream << "<td>\n";
		p_Stream << "<form action = '/gotostepcount' method='get'>\n";
		p_Stream << "<details>";
		p_Stream << "<summary>";
		p_Stream << "Goto step count<br/>\n";
		p_Stream << "<input type = 'submit' value = 'Goto' /><br/>\n";
		p_Stream << "</summary>";
		p_Stream << "Step count : <br/>\n";
		p_Stream << "<input type = 'text' name = 'stepcount' />\n";
		p_Stream << "<br/>\n";
		p_Stream << "Thread : <br/>\n";
		p_Stream << "<input type = 'text' name = 'threadnum' value = '0' />\n";
		p_Stream << "</details>";
		p_Stream << "</form>\n";
		p_Stream << "</td>\n";
	}

	/*static*/ void SNI_Thread::WriteGotoStepCountJS(std::ostream &p_Stream)
	{
		p_Stream << "<td>\n";
		p_Stream << "<form ng-submit = 'gotostepcount()'>\n";
		p_Stream << "<details>";
		p_Stream << "<summary>";
		p_Stream << "Goto step count<br/>\n";
		p_Stream << "<input type = 'submit' value = 'Goto' /><br/>\n";
		p_Stream << "</summary>";
		p_Stream << "Step count : <br/>\n";
		p_Stream << "<input type = 'text' ng-model='stepcount' name = 'stepcount' />\n";
		p_Stream << "<br/>\n";
		p_Stream << "</form>\n";
		p_Stream << "</td>\n";
	}

	void SNI_Thread::WriteSetMaxStackFrames(std::ostream &p_Stream)
	{
		p_Stream << "<td>\n";
		p_Stream << "<form action = '/maxstackframes' method='get'>\n";
		p_Stream << "<details>";
		p_Stream << "<summary>";
		p_Stream << "Max stack frames<br/>\n";
		p_Stream << "<input type = 'submit' value = 'Stack depth' /><br/>\n";
		p_Stream << "</summary>";
		p_Stream << "Number of stack frames to display :<br/>\n";
		p_Stream << "<input type = 'text' name = 'maxstackframes' value = '" << m_MaxStackFrames << "' />\n";
		p_Stream << "</details>";
		p_Stream << "</form>\n";
		p_Stream << "</td>\n";
	}

    /*static*/ void SNI_Thread::WriteSetMaxStackFramesJS(std::ostream &p_Stream)
	{
		p_Stream << "<td>\n";
		p_Stream << "<form ng-submit = 'loaddata()'>\n";
		p_Stream << "<details>";
		p_Stream << "<summary>";
		p_Stream << "Max stack frames<br/>\n";
		p_Stream << "<input type = 'submit' value = 'Stack depth' /><br/>\n";
		p_Stream << "</summary>";
		p_Stream << "Number of stack frames to display :<br/>\n";
		p_Stream << "<input type = 'text' ng-model='maxstackframes' name = 'maxstackframes' />\n";
		p_Stream << "</details>";
		p_Stream << "</form>\n";
		p_Stream << "</td>\n";
	}

	void SNI_Thread::WriteWebStack(std::ostream &p_Stream, size_t p_Depth, size_t p_DebugFieldWidth, SNI::SNI_DisplayOptions &p_DisplayOptions)
	{
		p_Stream << "<table class='stack'>\n";
		p_Stream << "<caption>Thread " << m_ThreadNum << "</caption>";
		size_t base = 0;
		if (0 < p_Depth && p_Depth < m_FrameList.size())
		{
			base = m_FrameList.size() - p_Depth;
		}
		Lock();
		for (size_t j = m_FrameList.size(); j > base; j--)
		{
			p_Stream << "<tr><td>\n";
			m_FrameList[j - 1]->WriteWebFrame(p_Stream, j, p_DebugFieldWidth, p_DisplayOptions);
			p_Stream << "</td></tr>\n";
		}
		Unlock();
		p_Stream << "</table>\n";
	}

	void SNI_Thread::WriteLogJS(std::ostream &p_Stream, long p_MaxLogEntries, long p_StartLog)
	{
		p_Stream << "{\n";
		p_Stream << "\"stepcount\":" << m_ThreadStepCount << ",\n";
		p_Stream << "\"records\":[\n";
		SNI_Log::GetLog()->LogTableJS(p_Stream, p_MaxLogEntries, p_StartLog);
		p_Stream << "]}\n";
	}

	size_t SNI_Thread::CountLogEntries()
	{
		return SNI_Log::GetLog()->CountLogEntries();
	}

	size_t SNI_Thread::CountCodeEntries()
	{
		return SNI_Log::GetLog()->CountCodeEntries();
	}

	void SNI_Thread::WriteDerivationJS(std::ostream &p_Stream, long p_MaxLogEntries)
	{
		SNI_Log::GetLog()->DerivationJS(p_Stream, p_MaxLogEntries, this);
	}

	void SNI_Thread::WriteCodeJS(std::ostream &p_Stream, long p_MaxLogEntries, long p_StartCode, SNI_DisplayOptions &p_displayOptions)
	{
		p_Stream << "{\n";
		p_Stream << "\"stepcount\":" << m_ThreadStepCount << ",\n";
		p_Stream << "\"records\":[\n";
		SNI_Log::GetLog()->CodeTableJS(p_Stream, p_MaxLogEntries, p_StartCode, p_displayOptions);
		p_Stream << "]}\n";
	}

	void SNI_Thread::WriteDashboardJS(std::ostream &p_Stream, SNI::SNI_DisplayOptions &p_DisplayOptions)
	{
		p_Stream << "{\n";

		p_Stream << "\t\"threadnum\" : \"" << m_ThreadNum << "\",\n";
		p_Stream << "\t\"breakpoint\" : " << m_BreakPointJS << ",\n";
		p_Stream << "\t\"breakpointdescription\" : \"" << m_DebugCommand.Description() << "\",\n";

		SNI_Manager *manager = GetTopManager(false);
		if (manager)
		{
			p_Stream << "\t\"taskdescription\" : \"" << manager->Description() << "\",\n";
			p_Stream << "\t\"maxstackframes\" : \"" << manager->MaxStackFrames() << "\",\n";
		}
		std::string statusDescription;
		int running = 0;
		int closing = 0;
		if (m_DebugCommand.IsQuitting())
		{
			statusDescription += " - Quitting";
		}
		else if (m_DebugCommand.IsRunning())
		{
			statusDescription += " - Running";
			running = 1;
		}
		else if (m_Closing)
		{
			statusDescription += " - Closing";
			running = 1;
			closing = 1;
		}
		if (m_Ended)
		{
			statusDescription += " - Thread ended";
		}
		p_Stream << "\t\"statusdescription\" : \"" << statusDescription << "\",\n";
		p_Stream << "\t\"running\" : " << running << ",\n";
		p_Stream << "\t\"closing\" : " << closing << ",\n";
		p_Stream << "\t\"currentstepcount\" : " << m_ThreadStepCount << ",\n";
		p_Stream << "\t\"laststepcount\" : " << m_LastThreadStepCount << ",\n";

		p_Stream << "\t\"countcalls\" : " << CountCalls() << ",\n";
		p_Stream << "\t\"countframes\" : " << m_FrameList.size() << ",\n";
		p_Stream << "\t\"countdelayedcalls\" : " << CountDelayedCalls() << ",\n";
		p_Stream << "\t\"countworldsets\" : " << CountWorldSets() << ",\n";
		p_Stream << "\t\"countlogentries\" : " << CountLogEntries() << ",\n";
		p_Stream << "\t\"countcodeentries\" : " << CountCodeEntries() << "\n";
		p_Stream << "}\n";
	}

	void SNI_Thread::WriteStackJS(std::ostream &p_Stream, size_t p_Depth, size_t p_Start, size_t p_DebugFieldWidth, SNI::SNI_DisplayOptions &p_DisplayOptions)
	{
		p_Stream << "{\n";
		p_Stream << "\"stepcount\":" << m_ThreadStepCount << ",\n";
		p_Stream << "\"records\":[\n";
		size_t base = 0;
		std::string delimeter;
		Lock();
		long start = static_cast<long>(m_FrameList.size() - p_Start);
		if (start < 0)
		{
			start = 0;
		}
		size_t effectiveDepth = p_Start + p_Depth;
		if (0 < effectiveDepth && effectiveDepth < m_FrameList.size())
		{
			base = m_FrameList.size() - effectiveDepth;
		}
		for (size_t j = start; j > base; j--)
		{
			p_Stream << delimeter << "\t{\n";
			m_FrameList[j - 1]->WriteJSON(p_Stream, j, p_DebugFieldWidth, p_DisplayOptions);
			delimeter = ",\n";
		}
		Unlock();
		p_Stream << "\n]}\n";
	}

	void SNI_Thread::WriteCallStackJS(std::ostream &p_Stream, size_t p_Depth, size_t p_Start, SNI::SNI_DisplayOptions &p_DisplayOptions)
	{
		namespace pt = boost::property_tree;
		pt::ptree oroot;
		oroot.put("stepcount", m_ThreadStepCount);
		pt::ptree records_node;
		size_t base = 0;
		std::string delimeter;
		size_t start = p_Start + p_Depth;
		if (start <= 0)
		{
			start = m_FrameList.size();
		}
		Lock();

		size_t count = 0;
		for (size_t j = m_FrameList.size(); j > 1 && start > count; j--)
		{
			SNI_Frame * frame = m_FrameList[j - 1];

			if (frame->HasCode())
			{
				count++;
				if (count > p_Start)
				{
					pt::ptree callStackNode;
					frame->WriteCallJS(callStackNode, j, p_DisplayOptions);
					records_node.push_back(std::make_pair("", callStackNode));
				}
			}
		}
		Unlock();

		oroot.add_child("records", records_node);
		pt::write_json(p_Stream, oroot);
	}

	void SNI_Thread::WriteWatchListJS(std::ostream& p_Stream, size_t p_DebugFieldWidth, SNI::SNI_DisplayOptions& p_DisplayOptions)
	{
		Lock();
		SNI_VariablePointerMap watchList;
		for (auto f : m_FrameList)
		{
			f->AddVariables(watchList);
		}
		Unlock();
		p_Stream << "{\n";
		p_Stream << "\"records\":[\n";
		std::string delimeter;
		std::string prefix = "\t";
		for (auto p : watchList)
		{
			SNI_Variable* v = p.second;
			if (v)
			{
				const SNI_Expression *value = v->GetSafeValue();
				p_Stream << delimeter << prefix << "{\n";
				v->WriteJSON(p_Stream, prefix+"\t", p_DebugFieldWidth, p_DisplayOptions);
				if (value)
				{
					std::string card_string = "&infin;";
					size_t card = value->Cardinality();
					if (card < CARDINALITY_MAX)
					{
						card_string = std::to_string(card);
					}
					p_Stream << ",\n" << prefix << "\t\"cardinality\" : \"" << card_string << "\",\n";
					value->WriteJSON(p_Stream, prefix + "\t", p_DebugFieldWidth, p_DisplayOptions);
				}
				p_Stream << "\n" << prefix << "}";
				delimeter = ",\n";
			}
		}
		p_Stream << "\n]}\n";
	}

	void SNI_Thread::WriteStepCountJS(std::ostream &p_Stream, const std::string &p_Delimeter)
	{
		p_Stream << "{\"threadnum\" : \"" << m_ThreadNum << "\", \"stepcount\" : \"" << m_ThreadStepCount << "\"}";
	}

	void SNI_Thread::WriteWorldSetsJS(std::ostream &p_Stream, SNI_DisplayOptions &p_DisplayOptions)
	{
		p_Stream << "{\"records\":[\n";
		if (m_WorldSetProcessMap)
		{
			std::string delimeter = "";
			Lock();
			for (const auto &pair : *m_WorldSetProcessMap)
			{
				SNI_WorldSet *ws = pair.second;
				p_Stream << delimeter << "\t{\n";
				ws->WriteJSON(p_Stream, "\t\t", p_DisplayOptions);
				p_Stream << "\t}";
				delimeter = ",\n";
			}
			Unlock();
		}
		p_Stream << "\n]}\n";
	}

	size_t SNI_Thread::CountDelayedCalls()
	{
		if (m_Processor)
		{
			return m_Processor->CountDelayedCalls();
		}
		return 0;
	}
	
	size_t SNI_Thread::CountWorldSets()
	{
		if (m_WorldSetProcessMap)
		{
			return m_WorldSetProcessMap->size();
		}
		return 0;
	}

	size_t SNI_Thread::CountCalls()
	{
		size_t count = 0;
		for (SNI_Frame * frame : m_FrameList)
		{
			if (frame->HasCode())
			{
				count++;
			}
		}
		return count;
	}

	void SNI::SNI_Thread::DisplayFrameStack(size_t p_Depth)
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

	SNI_Variable *SNI_Thread::LookupVariable(const std::string &p_Name)
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
		LOG(WriteLine(SN::DebugLevel, "Pushing stack frame " + std::to_string(m_FrameList.size()+1)));
		Lock();
		m_FrameList.push_back(p_Frame);
		Unlock();
	}

	void SNI_Thread::PopFrame()
	{
		Lock();
		m_FrameList.pop_back();
		Unlock();
		LOG(WriteLine(SN::DebugLevel, "Popped stack frame " + std::to_string(m_FrameList.size() + 1)));
	}

	size_t SNI_Thread::GetFrameStackDepth()
	{
		return m_FrameList.size();
	}

	void SNI_Thread::SetDeepBreakPoint(const std::string & p_BreakPoint, const std::string & p_BreakPointJS)
	{
		for (auto it = m_FrameList.rbegin(); it != m_FrameList.rend(); it++)
		{
			if ((*it)->HasCode())
			{
				(*it)->SetBreakPoint(p_BreakPoint, p_BreakPointJS);
				return;
			}
		}
	}

	void SNI_Thread::SetThreadBreakPoint(const std::string & p_BreakPoint, const std::string & p_BreakPointJS)
	{
		m_BreakPoint = p_BreakPoint;
		m_BreakPointJS = p_BreakPointJS;

		SNI_Frame *topFrame = SNI_Frame::Top();
		if (topFrame)
		{
			topFrame->SetBreakPoint(m_BreakPoint, m_BreakPointJS);
		}
	}

	SN::SN_Error SNI_Thread::CheckForFails()
	{
		SN::SN_Error result = skynet::OK;
		Lock();
		SNI_WorldSetList *changedList = GetWorldSetChanged();
		SNI_WorldSetMap *processMap = GetWorldSetProcessMap();
		bool changesToProcess = !changedList->empty();
		Unlock();

		while (changesToProcess)
		{
			bool process = false;
			Lock();
			if (!changedList->empty() && processMap->empty())
			{
				process = true;
				for (SNI_WorldSet *ws : *changedList)
				{
					ws->AddRelated(processMap);
				}
				changedList->clear();
			}
			Unlock();

			if (process)
			{
				for (auto pair : *processMap)
				{
					SNI_WorldSet *ws = pair.second;
					ws->CheckEmptyChildren();
				}
				for (auto pair : *processMap)
				{
					SNI_WorldSet *ws = pair.second;
					ws->CheckMissingInResult();
				}
				for (auto pair : *processMap)
				{
					SNI_WorldSet *ws = pair.second;
					ws->CheckAllNegated();
				}

				for (auto pair : *processMap)
				{
					SNI_WorldSet *ws = pair.second;
					ws->BreakPointsForDeletedWorlds();
				}
				Breakpoint(SN::InfoStop, SN::CallId, "", "Check dependencies", NULL, SN::MirrorPoint);

				Lock();
				for (auto pair : *processMap)
				{
					SNI_WorldSet *ws = pair.second;
					SN::SN_Error err = ws->RemoveFailures();
					if (err.IsError())
					{
						result = err;
					}
				}
				processMap->clear();
				changesToProcess = !changedList->empty();
				Unlock();
			}
			else
			{
				changesToProcess = false;
			}
		}
		return result;
	}

	void SNI_Thread::SetDebugAction(enum skynet::DebugAction p_DebugAction)
	{
		m_DebugCommand.ScheduleCommand(p_DebugAction);
	}

	SNI_World * SNI_Thread::ContextWorld()
	{
		if (m_ContextStack.size())
		{
			return m_ContextStack.back();
		}
		return NULL;
	}

	SNI_WorldList & SNI_Thread::ContextWorldList()
	{
		return m_ContextStack;
	}

	void SNI_Thread::PushContextWorld(SNI_World * p_Context)
	{
		m_ContextStack.push_back(p_Context);
	}

	void SNI_Thread::PopContextWorld()
	{
		m_ContextStack.pop_back();
	}

	void SNI_Thread::PromoteExternals(PGC::PGC_Transaction *p_Transaction)
	{
		for (SNI_Frame *f : m_FrameList)
		{
			f->PromoteExternals(p_Transaction);
		}
	}
}
