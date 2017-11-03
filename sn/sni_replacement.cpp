#include "sni_replacement.h"

#include "sni_variable.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Replacement::SNI_Replacement()
	{
	}

	SNI_Replacement::SNI_Replacement(const SN::SN_Variable &p_From, const SN::SN_Variable &p_To)
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

	SN::SN_Variable SNI_Replacement::ReplaceVariable(const SN::SN_Variable &p_Variable)
	{
		if (p_Variable.GetSNI_Variable() == m_From.GetSNI_Variable())
		{
			return m_To;
		}
		return NULL;
	}
}
