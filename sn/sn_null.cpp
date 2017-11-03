#include "sn_null.h"

#include "sni_null.h"

#include "sn_pch.h"

namespace SN
{
	SN_Null::SN_Null()
		: m_Null(NULL)
	{
	}

	SN_Null::SN_Null(const SN_Null &p_Null)
		: m_Null(p_Null.m_Null)
	{
	}

	SN_Null::~SN_Null()
	{
	}

	SNI::SNI_Null * SN_Null::GetSNI_Null() const
	{
		return m_Null;
	}
}
