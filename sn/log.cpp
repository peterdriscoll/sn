#include "log.h"
#include "logcontext.h"
#include "sn_manager.h"
#include "utility.h"
#include "ref.h"

#include <time.h> // time_t, tm, time, localtime, strftime
#include <direct.h>
#include <string>
using namespace std;

#include "sn_pch.h"

namespace SN
{
	string current_working_directory()
	{
		char* cwd = _getcwd(0, 0); // **** microsoft specific ****
		string working_directory(cwd);
		free(cwd);
		return working_directory;
	}

	string ExtractDirPath(const string& str)
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

	void Log::WriteLine(const string &p_line)
	{
		vector<string> arrLines;
		SNI::Split(p_line, "\n", arrLines);
		size_t num_lines = arrLines.size();
		string delimeter;
		for (size_t i = 0; i<num_lines; i++)
		{
			string line = arrLines[i];
			WriteLogFile(delimeter + line);
			delimeter = "\n";
			//        cout << line << endl;
			//        if (m_external_log)
			//        {
			//            m_external_log->WriteLine(string2BSTR(line));
			//        }
		}
	}

	//void Log::AddExternalLog(IExternalLog *p_external_log)
	//{
	//    m_external_log = p_external_log;
	//}


	Log * Log::GetLog(long p_LogIndex)
	{
		if (!m_singleton[p_LogIndex])
		{
			m_singleton[p_LogIndex] = new Log(p_LogIndex);
		}
		return m_singleton[p_LogIndex];
	}

	void Log::Verify(bool p_value, const string &p_description)
	{
		if (!p_value)
		{
			VerifyForce(p_description);
		}
	}

	void Log::VerifyForce(const string &p_description)
	{
		string title = "DAGEN Error Found";
		string description = p_description + SN::LogContext::GetContextDescription(LOG_STANDARD_INDEX);
		WriteLine("---------- Error found ----------");
		//    if (!p_symbol.IsEndOfFile())
		//    {
		//        if (m_external_log)
		//        {
		//            m_external_log->SelectDocumentText(string2BSTR(p_path), p_symbol.GetLineNum()+1, p_symbol.GetCharNum()+1, p_symbol.GetWord().GetLength());
		//        }
		//    }
		WriteLine(description);
		if (GetShowMessageBox())
		{
			//        ::MessageBox(0,description.data(),title.data(),MB_OK | MB_ICONERROR);
		}
		ASSERTM(false, "description");
	}

	void Log::Warning(const string &p_description)
	{
		string title = "DAGEN Warning";
		//    if (!p_symbol.IsEndOfFile())
		//    {
		//        if (m_external_log)
		//        {
		//            m_external_log->SelectDocumentText(string2BSTR(p_path), p_symbol.GetLineNum()+1, p_symbol.GetCharNum()+1, p_symbol.GetWord().GetLength());
		//        }
		//    }
		string description = p_description + SN::LogContext::GetContextDescription(LOG_STANDARD_INDEX);
		WriteLine("---------- Warning ----------");
		WriteLine(description);
		if (GetShowMessageBox())
		{
			//        ::MessageBox(0,description.data(),title.data(),MB_OK | MB_ICONERROR);
		}
	}

	bool Log::GetShowMessageBox()
	{
		return m_ShowMessageBox;
	}

	void Log::SetShowMessageBox(bool p_value)
	{
		m_ShowMessageBox = p_value;
	}

	void Log::CreateLogFile(long p_LogIndex)
	{
		string currentDirectory = current_working_directory();

		string timeId = getFormattedTime();
		string logIndex = to_string(p_LogIndex);
		string fileName = SN::SN_Manager::GetTopManager().LogFilePath() + logIndex + /*"_" + timeId +*/ ".log"; //" + timeId + "
		m_LogFile.open(fileName.data(), ios::out | ios::trunc);
		if (!m_LogFile.is_open())
		{
			throw SN::SN_Error("Log file " + fileName + " not opened. Check folders exist in path from " + currentDirectory);
		}
	}

	void Log::WriteLogFile(const string &p_Line)
	{
		m_LogFile.write(p_Line.data(), p_Line.length());
		m_LogFile.flush();
	}

	void Log::CloseLogFile()
	{
		m_LogFile.close();
	}

}
