#include "sn_stateful.h"

#include "sni_stateful.h"

#include "sn_pch.h"

namespace SN
{
	/*static*/ SN_Class SN_Stateful::Class()
	{
		return SNI_Stateful::Class();
	}

	SN_Stateful::SN_Stateful(enum skynet::Mutability p_Mutability, const SN_Expression &p_Expression)
		: SN_Expression(new SNI::SNI_Stateful(p_Mutability, p_Expression.GetSNI_Expression()))
	{
	}

	SN_Stateful::SN_Stateful(const SN_Expression &p_Expression)
		: SN_Expression(dynamic_cast<SNI::SNI_Stateful *>(p_Expression.GetSNI_Expression()))
	{
	}

	SN_Stateful::~SN_Stateful()
	{
	}

	SNI::SNI_Stateful * SN_Stateful::GetSNI_Stateful() const
	{
		return dynamic_cast<SNI::SNI_Stateful *>(m_Expression);
	}

	SNI::SNI_Expression * SN_Stateful::GetSNI_Expression() const
	{
		return dynamic_cast<SNI::SNI_Expression *>(m_Expression);
	}

	SN_Expression SN_Stateful::IsA(const SN_Expression &p_Parent) const
	{
		return SN_Operators::IsA(*this, p_Parent);
	}
}
