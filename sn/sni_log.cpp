#include "sni_log.h"
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
	{
	}

	SNI_Log::~SNI_Log()
	{
	}

	void SNI_Log::AddStream(ostream * p_Stream)
	{
		m_StreamList.push_back(p_Stream);
	}
	void SNI_Log::WriteLine(SN::LoggingLevel p_DebugLevel, const string &p_line)
	{
		vector<string> arrLines;
		SNI::Split(p_line, "\n", arrLines);
		size_t num_lines = arrLines.size();
		string timestamp = GetFormattedTime();
		for (ostream *stream : m_StreamList)
		{
			string delimeter;
			string prefix = timestamp;
			string blank(prefix.size(), ' ');
			for (string & line : arrLines)
			{
				*stream << prefix << line;
				stream->flush();
				delimeter = "\n";
				prefix = blank;
			}
		}
	}

	void SNI_Log::WriteFrame(SN::LoggingLevel p_DebugLevel, SNI_Frame * p_Frame)
	{
		if (p_Frame)
		{
			WriteLine(p_DebugLevel, p_Frame->GetLogDescription());
		}
	}
}
