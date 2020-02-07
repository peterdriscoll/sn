#include "sni_callrecord.h"
#include "sni_expression.h"
#include "logcontext.h"
#include "sn_pch.h"

namespace SNI
{
	SNI_CallRecord::SNI_CallRecord(const string & p_Purpose, const SNI_Expression * p_Expression)
		: m_Purpose(p_Purpose)
		, m_Expression(p_Expression)
	{
	}

	SNI_CallRecord::~SNI_CallRecord()
	{
	}

	string SNI_CallRecord::GetPurpose()
	{
		return m_Purpose;
	}

	SN::SN_Expression SNI_CallRecord::GetExpression()
	{
		return SN::SN_Expression();
	}

	string SNI_CallRecord::GetLogContext()
	{
		return m_LogContext;
	}

#ifdef USE_LOGGING
	void SNI_CallRecord::SetLogContext(const SN::LogContext &p_LogContext)
	{
		m_LogContext = p_LogContext.GetSimpleDescription();
	}
#endif
}