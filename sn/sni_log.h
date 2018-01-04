#if !defined(SNI_LOG_H)
#define SNI_LOG_H

#pragma once

#include "sn_manager.h"

#include <string>
#include <vector>
#include <ostream>

//#define _LOG
#ifdef _LOG
#define LOG(content) SNI_Log::GetLog()->content
#else
#define LOG(content)
#endif

using namespace std;

namespace SNI
{
	class SNI_Frame;

	class SNI_Log
	{
	public:
		static SNI_Log *GetLog();

		SNI_Log();
		virtual ~SNI_Log();

		void AddStream(SN::LoggingLevel p_LoggingLevel, ostream *p_Stream);

		void WriteLine(SN::LoggingLevel p_DebugLevel, const string & p_line);

		void WriteFrame(SN::LoggingLevel p_DebugLevel, SNI_Frame *p_Frame = NULL);

		void WriteExpression(SN::LoggingLevel p_DebugLevel, SN::SN_Expression p_Result, SN::SN_Expression p_Expression);

		void WriteContext(SN::LoggingLevel p_DebugLevel, const string &p_Text);
		void WriteFrameStack(SN::LoggingLevel p_DebugLevel, long p_Depth);
	private:
		static SNI_Log *m_Log;

		vector<ostream *> m_StreamList;
		vector<SN::LoggingLevel> m_LoggingLevelList;
	};
}

#endif // !defined(SNI_LOG_H)
