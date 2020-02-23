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

	void SNI_LogBuffer::WriteLine(const string & p_Line)
	{
		m_Mutex.lock();
		if (m_Buffer.full())
		{
			m_Buffer.pop_front();
		}
		m_Buffer.push_back(SNI_LogLine(SNI_Thread::GetThread()->GetFrameStackDepth(), p_Line));
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

	void SNI_LogBuffer::LogTableToStream(ostream & p_Stream, long p_MaxLogEntries)
	{
		p_Stream << "<table class='log'>\n";
		p_Stream << "<caption>Logging</caption>";
		m_Mutex.lock();
		long entries = 0;
		for (auto it = m_Buffer.rbegin(); it != m_Buffer.rend() && (p_MaxLogEntries <= 0 || entries < p_MaxLogEntries); it++, entries++)
		{
			p_Stream << "<tr><td>" << it->m_String << "</td></tr>";
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

	void SNI_LogBuffer::LogTableJS(ostream & p_Stream, long p_MaxLogEntries)
	{
		p_Stream << "{\"records\":[\n";
		m_Mutex.lock();
		string delimeter = " ";
		long entries = 0;
		for (auto it = m_Buffer.rbegin(); it != m_Buffer.rend() && (p_MaxLogEntries <= 0 || entries < p_MaxLogEntries); it++, entries++)
		{
			string quotedString = EscapeStringToJSON(it->m_String);
			p_Stream << delimeter << "{\"text\" : \"" << quotedString << "\"}\n";
			delimeter = ",";
		}
		p_Stream << "]}\n";
		m_Mutex.unlock();
	}

	void SNI_LogBuffer::DerivationJS(ostream & p_Stream, long p_MaxLogEntries)
	{
		p_Stream << "{\"derivationhtml\": \"";
		size_t minDepth = ULONG_MAX;
		m_Mutex.lock();
		for (const SNI_LogLine &line : m_Buffer)
		{
			if (line.m_Depth < minDepth)
			{
				minDepth = line.m_Depth;
			}
		}
		long entries = 0;
		size_t currentDepth = minDepth-1;
		vector<string> stackLastLines;
		for (auto it = m_Buffer.rbegin(); it != m_Buffer.rend() && (p_MaxLogEntries <= 0 || entries < p_MaxLogEntries); it++, entries++)
		{
			for (size_t j = currentDepth; j < it->m_Depth; j++)
			{
				stackLastLines.push_back("Log cutoff");
				p_Stream << "<details open>\\n";
			}
			for (size_t j = currentDepth; it->m_Depth < j; j--)
			{
				p_Stream << "<summary>" << EscapeStringToJSON(stackLastLines.back()) << "</summary>\\n</details>\\n";
				stackLastLines.pop_back();
			}
			currentDepth = it->m_Depth;
			stackLastLines.emplace_back(it->m_String);
			p_Stream << "<p>" << currentDepth << ": " << EscapeStringToJSON(it->m_String) << "</p>\\n";
		}
		for (size_t j = currentDepth; minDepth <= j; j--)
		{
			p_Stream << "<summary>" << EscapeStringToJSON(stackLastLines.back()) << "</summary>\\n</details>\\n";
			stackLastLines.pop_back();
		}
		p_Stream << "\"}\n";
		m_Mutex.unlock();
	}

	void SNI_LogBuffer::LogExpressionTableJS(ostream & p_Stream, long p_MaxLogEntries, SNI_DisplayOptions &p_DisplayOptions)
	{
		p_Stream << "{\"records\":[\n";
		m_Mutex.lock();
		string delimeter = " ";
		long entries = 0;
		for (auto it = m_ExpressionBuffer.rbegin(); it != m_ExpressionBuffer.rend() && (p_MaxLogEntries <= 0 || entries < p_MaxLogEntries); it++, entries++)
		{
			string quotedString = EscapeStringToJSON(it->DisplaySN(p_DisplayOptions));
			p_Stream << delimeter << "{\"expression\" : \"" << quotedString << "\"}\n";
			delimeter = ",";
		}
		p_Stream << "]}\n";
		m_Mutex.unlock();
	}
}
