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
	class SNI_LogBuffer
	{
	public:
		SNI_LogBuffer(size_t p_Capacity, size_t p_ExpressionCapacity);
		virtual ~SNI_LogBuffer();

		void WriteLine(const string &p_Line);
		void WriteExpression(const SN::SN_Expression & p_Expression);
		void LogTableToStream(ostream & p_Stream, long p_MaxLogEntries);
		void LogExpressionTableToStream(ostream & p_Stream, long p_MaxLogEntries, SNI_DisplayOptions &p_DisplayOptions);
		void LogTableJS(ostream & p_Stream, long p_MaxLogEntries);
		void LogExpressionTableJS(ostream & p_Stream, long p_MaxLogEntries, SNI_DisplayOptions &p_DisplayOptions);

	private:
		boost::circular_buffer<string> m_Buffer;
		boost::circular_buffer<SN::SN_Expression> m_ExpressionBuffer;
		mutex m_Mutex;
	};
}

#endif // !defined(SNI_LOGBUFFER_H)
