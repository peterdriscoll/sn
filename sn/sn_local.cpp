#include "sn_local.h"

#include "sn_variable.h"

#include "sni_local.h"

#include "sn_pch.h"

namespace SN
{
	SN_Local::SN_Local(SNI::SNI_Local * p_Local)
		: SN_Expression(p_Local)
	{
	}

	SN_Local::SN_Local(const SN_Variable &p_LocalVariable, const SN_Expression &p_Expression)
		: SN_Expression(new SNI::SNI_Local(p_LocalVariable.GetSNI_Variable(), p_Expression.GetSNI_Expression()))
	{
	}

	SN_Local::~SN_Local()
	{
	}

	SNI::SNI_Local * SN_Local::GetSNI_Local() const
	{
		return dynamic_cast<SNI::SNI_Local *>(m_Expression);
	}
}
