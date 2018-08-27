#include "sni_logbuffer.h"
using namespace std;

#include "sn_pch.h"

namespace SNI
{
	SNI_LogBuffer::SNI_LogBuffer(size_t p_Capacity)
		: m_Buffer(p_Capacity)
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
		m_Buffer.push_back(p_Line);
		m_Mutex.unlock();
	}

	void SNI_LogBuffer::LogTableToStream(ostream & p_Stream, long p_MaxLogEntries)
	{
		p_Stream << "<table>\n";
		p_Stream << "<caption>Logging</caption>";
		m_Mutex.lock();
		long entries = 0;
		for (auto it = m_Buffer.rbegin(); it != m_Buffer.rend() && (p_MaxLogEntries <= 0 || entries < p_MaxLogEntries); it++, entries++)
		{
			p_Stream << "<tr><td>" << *it << "</td></tr>";
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
			string quotedString = *it;
			ReplaceAll(quotedString, "\"", "\\\"");
			p_Stream << delimeter << "{\"text\" : \"" << quotedString << "\"}\n";
			delimeter = ",";
		}
		p_Stream << "]}\n";
		m_Mutex.unlock();
	}
}
