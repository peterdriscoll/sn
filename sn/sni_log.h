#if !defined(SNI_LOG_H)
#define SNI_LOG_H

#pragma once

#include "sn_manager.h"
#include "sn_expression.h"

#include <string>
#include <vector>
#include <ostream>

#include "sni_displayoptions.h"

#define _LOG
#ifdef _LOG
#define LOG(content) SNI_Log::GetLog()->content
#else
#define LOG(content)
#endif

using namespace std;

namespace SNI
{
	class SNI_Frame;
	class SNI_Thread;
	class SNI_LogBuffer;

	class SNI_Log
	{
	public:
		static SNI_Log *GetLog();

		SNI_Log();
		virtual ~SNI_Log();

		void AddStream(SN::LoggingLevel p_LoggingLevel, ostream *p_Stream);
		void SetLogBuffer(SN::LoggingLevel p_LoggingLevel, size_t p_Capacity, size_t p_ExpressionCapacity);

		void WriteLine(SN::LoggingLevel p_DebugLevel, const string & p_line, bool p_Heading = false);
		void WriteHeading(SN::LoggingLevel p_DebugLevel, const string & p_line);

		void WriteFrame(SNI_Thread *p_Thread, SN::LoggingLevel p_DebugLevel, SNI_Frame * p_Frame = NULL);
		void WriteExpression(SN::LoggingLevel p_DebugLevel, SN::SN_Expression p_Result, SN::SN_Expression p_Expression);
		void WriteClonedExpression(SN::LoggingLevel p_DebugLevel, const string &p_Text, SN::SN_Expression p_Expression);
		void WriteVariable(SN::LoggingLevel p_DebugLevel, SN::SN_Variable p_Variable);
		void WriteVariableByName(SN::LoggingLevel p_DebugLevel, const string & p_Name);
		void WriteContext(SN::LoggingLevel p_DebugLevel, const string &p_Text);
		void WriteFrameStack(SN::LoggingLevel p_DebugLevel, long p_Depth);
		void WriteFrameStack(SN::LoggingLevel p_DebugLevel, long p_ThreadNum, long p_Depth);

		void LogTableToStream(ostream & p_Stream, long p_MaxLogEntries);
		void LogTableJS(ostream & p_Stream, long p_MaxLogEntries);
		void DerivationJS(ostream & p_Stream, long p_MaxLogEntries);
		void CodeTableJS(ostream & p_Stream, long p_MaxLogEntries, SNI_DisplayOptions &p_DisplayOptions);

		void WriteExp(SN::SN_Expression p_Expression);
		void ClearLogExpressions();

		size_t CountLogEntries();
		size_t CountCodeEntries();
	private:
		static SNI_Log *m_Log;

		vector<ostream *> m_StreamList;
		vector<SN::LoggingLevel> m_LoggingLevelList;

		SN::LoggingLevel m_LogBufferLoggingLevel;
		SNI_LogBuffer *m_LogBuffer;
	};
}

#endif // !defined(SNI_LOG_H)
