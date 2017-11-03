#include "sn_let.h"

#include "sni_let.h"

#include "sn_pch.h"

namespace SN
{
	SN_Let::SN_Let(SNI::SNI_Let *p_Let)
		: SN_Expression(p_Let)
	{
	}

	SN_Let::SN_Let(const SN_Expression &p_Condition, const SN_Expression &p_Expression)
		: SN_Expression(new SNI::SNI_Let(p_Condition.GetSNI_Expression(), p_Expression.GetSNI_Expression()))
	{
	}

	SN_Let::~SN_Let()
	{
	}

	SNI::SNI_Let * SN_Let::GetSNI_Let() const
	{
		return dynamic_cast<SNI::SNI_Let *>(m_Expression);
	}
}
