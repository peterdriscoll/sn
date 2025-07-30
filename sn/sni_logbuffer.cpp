#include "sni_logbuffer.h"
using namespace std;

#include "sn_pch.h"

namespace SNI
{
	SNI_LogBuffer::SNI_LogBuffer(size_t p_Capacity, size_t p_ExpressionCapacity)
		: m_Buffer(p_Capacity)
		, m_ExpressionBuffer(p_ExpressionCapacity)
	{
	}

	SNI_LogBuffer::~SNI_LogBuffer()
	{
	}

	void SNI_LogBuffer::WriteLine(const string & p_Line, bool p_Heading)
	{
		m_Mutex.lock();
		if (m_Buffer.full())
		{
			m_Buffer.pop_front();
		}
		m_Buffer.push_back(SNI_LogLine(SNI_Thread::GetThread()->GetFrameStackDepth(), SNI_Thread::GetThread()->GetStepCount(), p_Line, p_Heading));
		m_Mutex.unlock();
	}

	void SNI_LogBuffer::WriteExpression(const SN::SN_Expression & p_Expression)
	{
		m_Mutex.lock();
		if (m_ExpressionBuffer.full())
		{
			m_Buffer.pop_front();
		}
		m_ExpressionBuffer.push_back(p_Expression);
		m_Mutex.unlock();
	}

	void SNI_LogBuffer::ClearLogExpressions()
	{
		m_ExpressionBuffer.clear();
	}

	void SNI::SNI_LogBuffer::LogTableToStream(ostream & p_Stream, size_t p_MaxLogEntries)
	{
		p_Stream << "<table class='log'>\n";
		p_Stream << "<caption>Logging</caption>";
		m_Mutex.lock();
		long entries = 0;
		for (auto it = m_Buffer.rbegin(); it != m_Buffer.rend() && (p_MaxLogEntries <= 0 || entries < p_MaxLogEntries); it++, entries++)
		{
			string html = EscapeStringToHTML(it->m_String);
			p_Stream << "<tr><td>" << html << "</td></tr>";
		}
		p_Stream << "</table>\n";
		m_Mutex.unlock();
	}

	void SNI_LogBuffer::LogExpressionTableToStream(ostream & p_Stream, long p_MaxLogEntries, SNI_DisplayOptions &p_DisplayOptions)
	{
		p_Stream << "<table class='log'>\n";
		p_Stream << "<caption>Logging</caption>";
		m_Mutex.lock();
		long entries = 0;
		for (auto it = m_ExpressionBuffer.rbegin(); it != m_ExpressionBuffer.rend() && (p_MaxLogEntries <= 0 || entries < p_MaxLogEntries); it++, entries++)
		{
			p_Stream << "<tr><td>" << it->DisplaySN(p_DisplayOptions) << "</td></tr>";
		}
		p_Stream << "</table>\n";
		m_Mutex.unlock();
	}

	void SNI_LogBuffer::LogTableJS(ostream & p_Stream, long p_MaxLogEntries, long p_StartLog)
	{
		m_Mutex.lock();
		string delimeter = " ";
		long entries = 0;
		for (auto it = m_Buffer.rbegin(); it != m_Buffer.rend() && (p_MaxLogEntries <= 0 || entries < p_StartLog + p_MaxLogEntries); it++, entries++)
		{
			if (p_StartLog <= entries)
			{
				string html = EscapeStringToHTML(it->m_String);
				string quotedString = EscapeStringToJSON(html);
				p_Stream << delimeter << "{\"text\" : \"" << quotedString << "\"}\n";
				delimeter = ",";
			}
		}
		m_Mutex.unlock();
	}

	size_t SNI_LogBuffer::CountLogEntries()
	{
		return m_Buffer.size();
	}

	size_t SNI_LogBuffer::CountCodeEntries()
	{
		return m_ExpressionBuffer.size();
	}

	void SNI_LogBuffer::DerivationJS(ostream & p_Stream, long p_MaxLogEntries)
	{
		p_Stream << "{\"derivationhtml\": \"";
		size_t actualDepth = SNI_Thread::GetThread()->GetFrameStackDepth();
		size_t minDepth = LONG_MAX;
		long entries = 0;
		m_Mutex.lock();
		for (auto it = m_Buffer.rbegin(); it != m_Buffer.rend() && (p_MaxLogEntries <= 0 || entries < p_MaxLogEntries); it++, entries++)
		{
			if (it->m_Depth < minDepth)
			{
				minDepth = it->m_Depth;
			}
		}
		entries = 0;
		size_t currentDepth = minDepth-1;
		vector<SNI_LogLine *> stackLastLines;
		for (auto it = m_Buffer.rbegin(); it != m_Buffer.rend() && ((p_MaxLogEntries <= 0 || entries < p_MaxLogEntries) || currentDepth > minDepth); it++, entries++)
		{
			for (size_t j = currentDepth; j < it->m_Depth; j++)
			{
				ASSERTM(j - minDepth + 1 == stackLastLines.size(), "Internal error 1");
				stackLastLines.push_back(NULL);
				if (j < actualDepth)
				{
					p_Stream << "<details open>\\n";
				}
				else
				{
					p_Stream << "<details>\\n";
				}
			}
			ASSERTM(it->m_Depth - minDepth + 1 <= (long) stackLastLines.size(), "Internal error 2");
			for (size_t j = currentDepth; it->m_Depth < j; j--)
			{
				ASSERTM(j - minDepth + 1 == stackLastLines.size(), "Internal error 3");
				SNI_LogLine *line = stackLastLines.back();
				if (line)
				{
					string html = EscapeStringToHTML(line->m_String);
					p_Stream << "<summary><b>" << line->m_StepCount << ": " << line->m_Depth << ":</b> " << EscapeStringToJSON(html) << "</summary>\\n<hr>\\n</details>\\n";
				}
				else
				{
					p_Stream << "<summary><b>unknown: " << j << ": buffer cutoff</b></summary>\\n<hr>\\n</details>\\n";
				}
				stackLastLines.pop_back();
			}
			currentDepth = it->m_Depth;
			ASSERTM(currentDepth - minDepth + 1 == stackLastLines.size(), "Internal error 4");

			string html = EscapeStringToHTML(it->m_String);
			if (it->m_Heading)
			{
				if (stackLastLines.back() == NULL)
				{
					stackLastLines[stackLastLines.size() - 1] = (&(*it));
				}
				p_Stream << "<p><b>" << it->m_StepCount << ": " << currentDepth << ":</b> " << EscapeStringToJSON(html) << "</p>\\n";
			}
			else
			{
				p_Stream << "<p>" << it->m_StepCount << ": " << EscapeStringToJSON(html) << "</p>\\n";
			}
		}
		for (size_t j = currentDepth; minDepth <= j; j--)
		{
			ASSERTM(j - minDepth + 1 == stackLastLines.size(), "Internal error 5");
			SNI_LogLine *line = stackLastLines.back();

			if (line)
			{
				ASSERTM(j == line->m_Depth, "Internal error 6");
				string html = EscapeStringToHTML(line->m_String);
				p_Stream << "<summary><b>" << line->m_StepCount << ": " << line->m_Depth << ":</b> " << EscapeStringToJSON(html) << "</summary>\\n<hr>\\n</details>\\n";
			}
			else
			{
				p_Stream << "<summary><b>unknown: " << j << ": buffer cutoff</b></summary>\\n<hr>\\n</details>\\n";
			}
			stackLastLines.pop_back();
		}
		p_Stream << "\"}\n";
		m_Mutex.unlock();
	}

	void SNI_LogBuffer::LogExpressionTableJS(ostream & p_Stream, long p_MaxLogEntries, long p_StartCode, SNI_DisplayOptions &p_DisplayOptions)
	{
		m_Mutex.lock();
		string delimeter = " ";
		long entries = 0;
		for (auto it = m_ExpressionBuffer.rbegin(); it != m_ExpressionBuffer.rend() && (p_MaxLogEntries <= 0 || entries < p_StartCode + p_MaxLogEntries); it++, entries++)
		{
			if (p_StartCode <= entries)
			{
				string quotedString = EscapeStringToJSON(it->DisplaySN(p_DisplayOptions));
				p_Stream << delimeter << "{\"expression\" : \"" << quotedString << "\"}\n";
				delimeter = ",";
			}
		}
		m_Mutex.unlock();
	}
}
