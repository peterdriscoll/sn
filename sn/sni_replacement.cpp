#include "sni_replacement.h"

#include "sni_variable.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Replacement::SNI_Replacement()
	{
	}

	SNI_Replacement::SNI_Replacement(const SNI_Variable *p_From, SNI_Variable *p_To)
		: m_From(p_From)
		, m_To(p_To)
	{
	}

	SNI_Replacement::~SNI_Replacement()
	{
	}

	void SNI_Replacement::PromoteMembers()
	{
	}

	SNI_Variable * SNI_Replacement::ReplaceVariable(SNI_Variable *p_Variable)
	{
		if (p_Variable == m_From)
		{
			return m_To;
		}
		return NULL;
	}
}
