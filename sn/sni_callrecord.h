#if !defined(SNI_CALLRECORD_H_INCLUDED)
#define SNI_CALLRECORD_H_INCLUDED

#pragma once

#include <string>

#include "sni_object.h"

namespace SN
{
	class LogContext;

	class SN_Expression;
}

namespace SNI
{
	class SNI_CallRecord : public SNI_Object
	{
		PGC_CLASS(SNI_CallRecord);
	public:
		SNI_CallRecord(const std::string &p_Purpose, const SNI_Expression *p_Expression);
		virtual ~SNI_CallRecord();

		std::string GetPurpose();
		SN::SN_Expression GetExpression();
		std::string GetLogContext();

#ifdef USE_LOGGING
		void SetLogContext(const SN::LogContext &p_LogContext);
#endif

	private:
		std::string m_Purpose;
		const SNI_Expression *m_Expression;
		std::string m_LogContext;
	};
}

#endif // !defined(SNI_CALLRECORD_H_INCLUDED)
