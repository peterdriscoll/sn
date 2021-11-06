#include "sn_define.h"

#include "sn_variable.h"

#include "sni_define.h"

#include "sn_pch.h"

namespace SN
{
	/*static*/ SN_Class SN_Define::Class()
	{
		return SNI_Define::Class();
	}

	SN_Define::SN_Define()
	{
	}

	SN_Define::SN_Define(const SN_Variable &p_Variable, enum skynet::DefineType p_DefineType)
		: SN_Expression(new SNI::SNI_Define(p_Variable.GetSNI_Variable(), p_DefineType))
	{
	}

	SN_Define::~SN_Define()
	{
	}

	SNI::SNI_Define * SN_Define::GetSNI_Define() const
	{
		return dynamic_cast<SNI::SNI_Define *>(m_Expression);
	}
}
