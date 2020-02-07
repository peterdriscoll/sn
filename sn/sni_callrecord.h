#if !defined(SNI_CALLRECORD_H_INCLUDED)
#define SNI_CALLRECORD_H_INCLUDED

#pragma once

#include <string>

#include "sni_object.h"

namespace SN
{
	LOGGING(class LogContext);

	class SN_Expression;
}

namespace SNI
{
	class SNI_CallRecord : public SNI_Object
	{
		PGC_CLASS(SNI_CallRecord);
	public:
		SNI_CallRecord(const string &p_Purpose, const SNI_Expression *p_Expression);
		virtual ~SNI_CallRecord();

		string GetPurpose();
		SN::SN_Expression GetExpression();
		string GetLogContext();

#ifdef USE_LOGGING
		void SetLogContext(const SN::LogContext &p_LogContext);
#endif

	private:
		string m_Purpose;
		const SNI_Expression *m_Expression;
		string m_LogContext;
	};

	typedef vector<SNI_CallRecord *> SNI_CallRecordList;
}

#endif // !defined(SNI_CALLRECORD_H_INCLUDED)
