#include "sn_error.h"

#include "sn_expression.h"
#include "sn_value.h"

#include "sni_error.h"
#include "sni_expression.h"

#include "sn_pch.h"

namespace SN
{
	SN_Error::SN_Error()
		: m_Error(NULL)
	{
	}

	SN_Error::SN_Error(string p_String)
		: m_Error(new SNI::SNI_Error(p_String))
	{
	}

	SN_Error::SN_Error(bool p_Success, bool p_Delay, const string & p_Description)
		: m_Error(new SNI::SNI_Error(p_Success, p_Delay, p_Description))
	{
	}

	SN_Error::SN_Error(const SN_Error &p_Error)
		: m_Error(p_Error.m_Error)
	{
	}

	SN_Error::SN_Error(const SN_Expression &p_Expression)
		: m_Error(p_Expression.GetSNI_Error())
	{
	}

	SN_Error::~SN_Error()
	{
	}

	//////////////////////////////////////////////////////////////////////
	// Query Classes
	//////////////////////////////////////////////////////////////////////


	SNI::SNI_Error * SN_Error::GetSNI_Error() const
	{
		return m_Error;
	}

	bool SN_Error::IsError()
	{
		return m_Error && m_Error->IsError();
	}

	bool SN_Error::IsNull()
	{
		return !m_Error || m_Error->IsNull();
	}

	bool SN_Error::GetBool()
	{
		return m_Error->GetBool();
	}

	bool SN_Error::GetDelay()
	{
		return m_Error->GetDelay();
	}
	
	string SN_Error::GetDescription()
	{
		return m_Error->GetDescription();
	}

	void SN_Error::Log()
	{
		m_Error->Log();
	}
}
