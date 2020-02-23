#if !defined(SNI_LOGBUFFER_H)
#define SNI_LOGBUFFER_H

#pragma once

#include "sn_manager.h"
#include "sn_expression.h"

#include <string>
#include <mutex>
#include <ostream>
#include <boost/circular_buffer.hpp>

using namespace std;

namespace SNI
{
	class SNI_LogLine {
	public:
		size_t m_Depth;
		string m_String;

		SNI_LogLine(size_t p_Depth, const string &p_String)
			: m_Depth(p_Depth)
			, m_String(p_String)
		{}
		SNI_LogLine(const SNI_LogLine &p_Source)
			: m_Depth(p_Source.m_Depth)
			, m_String(p_Source.m_String)
		{}
		SNI_LogLine &operator =(const SNI_LogLine &p_Source)
		{
			m_Depth = p_Source.m_Depth;
			m_String = p_Source.m_String;
			return *this;
		}
	};

	class SNI_LogBuffer
	{
	public:
		SNI_LogBuffer(size_t p_Capacity, size_t p_ExpressionCapacity);
		virtual ~SNI_LogBuffer();

		void WriteLine(const string &p_Line);
		void WriteExpression(const SN::SN_Expression & p_Expression);
		void ClearLogExpressions();
		void LogTableToStream(ostream & p_Stream, long p_MaxLogEntries);
		void LogExpressionTableToStream(ostream & p_Stream, long p_MaxLogEntries, SNI_DisplayOptions &p_DisplayOptions);
		void LogTableJS(ostream & p_Stream, long p_MaxLogEntries);
		void DerivationJS(ostream & p_Stream, long p_MaxLogEntries);
		void LogExpressionTableJS(ostream & p_Stream, long p_MaxLogEntries, SNI_DisplayOptions &p_DisplayOptions);

	private:
		boost::circular_buffer<SNI_LogLine> m_Buffer;
		boost::circular_buffer<SN::SN_Expression> m_ExpressionBuffer;
		mutex m_Mutex;
	};
}

#endif // !defined(SNI_LOGBUFFER_H)
