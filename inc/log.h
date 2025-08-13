#if !defined(LOG_H)
#define LOG_H

#pragma once

// #define USE_LOGGING

#define MAX_LOG_INDEX 2
#define LOG_STANDARD_INDEX 0
#define LOG_DEBUG_INDEX 1

#include "exp_ctrl_sn.h"
#include <string>
#include <fstream>

namespace SN
{
	class Log
	{
	public:
		static Log * GetLog(long p_LogIndex);

		Log(long p_LogIndex = LOG_STANDARD_INDEX);
		virtual ~Log();

		void WriteLine(const std::string &p_line);
		void Verify(bool p_value, const std::string &p_description);
		void VerifyForce(const std::string &p_description);

	protected:
		void CreateLogFile(long p_LogIndex);
		void WriteLogFile(const std::string &p_Line);
		void CloseLogFile();

	private:

		static Log * m_singleton[MAX_LOG_INDEX];

		bool m_ShowMessageBox;

		std::fstream m_LogFile;
	};
}

#endif // !defined(LOG_H)
