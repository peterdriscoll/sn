#include "sni_log.h"
#include "sni_frame.h"
#include "logcontext.h"
#include "sn_manager.h"
#include "utility.h"
#include "ref.h"

#include <time.h> // time_t, tm, time, localtime, strftime
#include <direct.h>
#include <string>
using namespace std;

#include "sn_pch.h"

namespace SNI
{
	/*static*/ SNI_Log *SNI_Log::m_Log = NULL;

	/*static*/ SNI_Log *SNI_Log::GetLog()
	{
		if (!m_Log)
		{
			m_Log = new SNI_Log;
		}
		return m_Log;
	}

	SNI_Log::SNI_Log()
		: m_LogBuffer(NULL)
	{
	}

	SNI_Log::~SNI_Log()
	{
	}

	void SNI_Log::AddStream(SN::LoggingLevel p_LoggingLevel, ostream * p_Stream)
	{
		m_StreamList.push_back(p_Stream);
		m_LoggingLevelList.push_back(p_LoggingLevel);
	}

	void SNI_Log::SetLogBuffer(SN::LoggingLevel p_LoggingLevel, size_t p_Capacity, size_t p_ExpressionCapacity)
	{
		m_LogBuffer = new SNI_LogBuffer(p_Capacity, p_ExpressionCapacity);
		m_LogBufferLoggingLevel = p_LoggingLevel;
	}

	void SNI_Log::WriteLine(SN::LoggingLevel p_DebugLevel, const string &p_line)
	{
		vector<string> arrLines;
		SNI::Split(p_line, "\n", arrLines);
		size_t num_lines = arrLines.size()-1;
		string timestamp = GetFormattedTime();
		size_t j = 0;
		for (ostream *stream : m_StreamList)
		{
			if (p_DebugLevel <= m_LoggingLevelList[j++])
			{
				string delimeter;
				string prefix = timestamp;
				string blank(prefix.size(), ' ');
				for (string & line : arrLines)
				{
					*stream << prefix << " | " << line << "\n";
					stream->flush();
					delimeter = "\n";
					prefix = blank;
				}
			}
		}
		if (m_LogBuffer)
		{
			if (p_DebugLevel <= m_LogBufferLoggingLevel)
			{
				for (string & line : arrLines)
				{
					m_LogBuffer->WriteLine(line);
				}
			}
		}
	}

	void SNI_Log::WriteFrame(SNI_Thread *p_Thread, SN::LoggingLevel p_DebugLevel, SNI_Frame * p_Frame)
	{
		SNI_Frame *frame = p_Frame;
		if (!frame)
		{
			frame = p_Thread->Top();
		}
		if (frame)
		{
			WriteLine(p_DebugLevel, frame->GetLogDescription(p_Thread->GetTopManager()));
		}
	}

	void SNI_Log::WriteExpression(SN::LoggingLevel p_DebugLevel, SN::SN_Expression p_Result, SN::SN_Expression p_Expression)
	{
		WriteLine(p_DebugLevel, p_Result.DisplayValueSN() + " == " + p_Expression.DisplayValueSN());
	}

	void SNI_Log::WriteVariable(SN::LoggingLevel p_DebugLevel, SN::SN_Variable p_Variable)
	{
		WriteLine(p_DebugLevel, p_Variable.GetName() + " == " + p_Variable.GetVariableValue().DisplaySN());
	}

	void SNI_Log::WriteVariableByName(SN::LoggingLevel p_DebugLevel, const string &p_Name)
	{
		SNI_Variable * v = SNI_Thread::GetThread()->LookupVariable(p_Name);
		if (v)
		{
			WriteVariable(p_DebugLevel, v);
		}
		else
		{
			WriteLine(p_DebugLevel, p_Name + " not found");
		}
	}

	void SNI_Log::WriteContext(SN::LoggingLevel p_DebugLevel, const string & p_Text)
	{
		SNI_World *world = SNI_World::ContextWorld();
		if (world)
		{
			WriteLine(p_DebugLevel, p_Text + world->DisplayShort());
		}
		else
		{
			WriteLine(p_DebugLevel, p_Text + "global context");
		}

	}

	void SNI_Log::WriteFrameStack(SN::LoggingLevel p_DebugLevel, long p_Depth)
	{
		SNI_Frame::DisplayFrameStack(p_Depth);
	}

	void SNI_Log::WriteFrameStack(SN::LoggingLevel p_DebugLevel, long p_ThreadNum, long p_Depth)
	{
		SNI_Frame::DisplayFrameStack(p_Depth);
	}

	void SNI_Log::LogTableToStream(ostream & p_Stream, long p_MaxLogEntries)
	{
		if (m_LogBuffer)
		{
			m_LogBuffer->LogTableToStream(p_Stream, p_MaxLogEntries);
		}
	}

	void SNI_Log::LogTableJS(ostream & p_Stream, long p_MaxLogEntries)
	{
		if (m_LogBuffer)
		{
			m_LogBuffer->LogTableJS(p_Stream, p_MaxLogEntries);
		}
	}

	void SNI_Log::LogExpTableJS(ostream & p_Stream, long p_MaxLogEntries, SNI_DisplayOptions &p_DisplayOptions)
	{
		if (m_LogBuffer)
		{
			m_LogBuffer->LogExpressionTableJS(p_Stream, p_MaxLogEntries, p_DisplayOptions);
		}
	}

	void SNI_Log::WriteExp(SN::SN_Expression p_Expression)
	{
		if (m_LogBuffer)
		{
			m_LogBuffer->WriteExpression(p_Expression);
		}
	}

	void SNI_Log::ClearLogExpressions()
	{
		if (m_LogBuffer)
		{
			m_LogBuffer->ClearLogExpressions();
		}
	}
}
