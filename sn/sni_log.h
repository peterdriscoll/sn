#if !defined(SNI_LOG_H)
#define SNI_LOG_H

#pragma once

#include "sn_manager.h"

#include <string>
#include <vector>
#include <ostream>

#define _LOG
#ifdef _LOG
#define LOG(level, content) Log::GetLog()->content
#else
#define LOG(level, content)
#endif

using namespace std;

namespace SNI
{
	class SNI_Log
	{
	public:
		static SNI_Log *GetLog();

		SNI_Log();
		virtual ~SNI_Log();

		void AddStream(ostream *p_Stream);

		void WriteLine(SN::LoggingLevel p_DebugLevel, const string & p_line);

		void WriteFrame(SN::LoggingLevel p_DebugLevel, SNI_Frame *p_Frame);

	private:
		static SNI_Log *m_Log;

		vector<ostream *> m_StreamList;
	};
}

#endif // !defined(SNI_LOG_H)
