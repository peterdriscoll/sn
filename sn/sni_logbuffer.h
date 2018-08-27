#if !defined(SNI_LOGBUFFER_H)
#define SNI_LOGBUFFER_H

#pragma once

#include "sn_manager.h"

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
		SNI_LogBuffer(size_t p_Capacity);
		virtual ~SNI_LogBuffer();

		void WriteLine(const string &p_Line);
		void LogTableToStream(ostream & p_Stream, long p_MaxLogEntries);
		void LogTableJS(ostream & p_Stream, long p_MaxLogEntries);

	private:
		boost::circular_buffer<string> m_Buffer;
		mutex m_Mutex;
	};
}

#endif // !defined(SNI_LOGBUFFER_H)
