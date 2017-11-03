#include "sn_meta.h"

#include "sn_value.h"

#include "sni_meta.h"
#include "sni_value.h"

#include "sn_pch.h"

namespace SN
{
	SN_Meta::SN_Meta(long p_MetaLevel, const SN_Expression &p_Expression)
		: SN_Expression(new SNI::SNI_Meta(p_MetaLevel, p_Expression.GetSNI_Expression()))
	{
	}

	SN_Meta::SN_Meta(const SN_Expression &p_Expression)
		: SN_Expression(new SNI::SNI_Meta(1, p_Expression.GetSNI_Expression()))
	{
	}

	SN_Meta::~SN_Meta()
	{
	}

	SNI::SNI_Meta * SN_Meta::GetSNI_Meta() const
	{
		return dynamic_cast<SNI::SNI_Meta *>(m_Expression);
	}
}
