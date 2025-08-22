#include "log.h"
#include "logcontext.h"
#include "sn_manager.h"
#include "sni_thread.h"
#include "utility.h"
#include "ref.h"

#include <time.h> // time_t, tm, time, localtime, strftime
#include <direct.h>
#include <string>

#include "sn_pch.h"

namespace SN
{
	#define BUFFER_SIZE 100
	std::string current_working_directory()
	{
		char buffer[BUFFER_SIZE];
		char* cwd = _getcwd(buffer, BUFFER_SIZE); // **** microsoft specific ****
		std::string working_directory(cwd);
		return working_directory;
	}

	std::string ExtractDirPath(const std::string& str)
	{
		size_t found = str.find_last_of("/\\");
		return str.substr(0, found);
	}

	// Returns the local date/time formatted as 2014-03-19 11:11:52
	char* getFormattedTime(void) {

		time_t rawtime;
		struct tm timeinfo;

		time(&rawtime);
		localtime_s(&timeinfo, &rawtime);

		// Must be static, otherwise won't work
		static char _retval[20];
		strftime(_retval, sizeof(_retval), "%Y%m_%d_%H_%M_%S", &timeinfo);

		return _retval;
	}

	Log * Log::m_singleton[MAX_LOG_INDEX];

	Log::Log(long p_LogIndex)
		: m_ShowMessageBox(true)
	{
		CreateLogFile(p_LogIndex);
	}

	Log::~Log()
	{
		CloseLogFile();
	}

	void Log::WriteLine(const std::string &p_line)
	{
		std::vector<std::string> arrLines;
		SNI::Split(p_line, "\n", arrLines);
		size_t num_lines = arrLines.size();
		std::string delimeter;
		for (size_t i = 0; i<num_lines; i++)
		{
			std::string line = arrLines[i];
			WriteLogFile(delimeter + line);
			delimeter = "\n";
		}
	}

	Log * Log::GetLog(long p_LogIndex)
	{
		if (!m_singleton[p_LogIndex])
		{
			m_singleton[p_LogIndex] = new Log(p_LogIndex);
		}
		return m_singleton[p_LogIndex];
	}

	void Log::Verify(bool p_value, const std::string &p_description)
	{
		if (!p_value)
		{
			VerifyForce(p_description);
		}
	}

	void Log::VerifyForce(const std::string &p_description)
	{
		std::string title = "DAGEN Error Found";
		std::string description = p_description;
		LOGGING(description += SN::LogContext::GetContextDescription(LOG_STANDARD_INDEX));
		WriteLine("---------- Error found ----------");
		WriteLine(description);
		ASSERTM(false, "description");
	}

	void Log::CreateLogFile(long p_LogIndex)
	{
		std::string currentDirectory = current_working_directory();
		SNI_Thread *l_thread = SNI_Thread::GetThread();
		std::string timeId = getFormattedTime();
		std::string logIndex = std::to_string(p_LogIndex);
		std::string fileName = l_thread->GetTopManager()->LogFilePath() + std::to_string(l_thread->GetThreadNum()) + "_" + logIndex + /*"_" + timeId +*/ ".log"; //" + timeId + "
		m_LogFile.open(fileName.data(), ios::out | ios::trunc);
		if (!m_LogFile.is_open())
		{
			throw new SNI_Error(false, false, "Log file " + fileName + " not opened. Check folders exist in path from " + currentDirectory);
		}
	}

	void Log::WriteLogFile(const std::string &p_Line)
	{
		m_LogFile << p_Line;
		m_LogFile.flush();
	}

	void Log::CloseLogFile()
	{
		m_LogFile.close();
	}

}
