#include "sn_state.h"

#include "sni_state.h"

#include "sn_pch.h"

namespace SN
{
	/*static*/ SN_Class SN_State::Class()
	{
		return SNI_State::Class();
	}

	SN_State::SN_State()
		: SN_Base(new SNI::SNI_State())
	{
	}

	SN_State::SN_State(const SN_Value &p_Value)
		: SN_Base(dynamic_cast<SNI::SNI_State *>(p_Value.GetSNI_Value()))
	{
	}

	SN_State::SN_State(const SN_State &p_StateValue)
		: SN_Base(p_StateValue.m_Expression)
	{
	}

	SN_State::~SN_State()
	{
	}

	SNI::SNI_State *SN_State::GetSNI_State() const
	{
		return static_cast<SNI::SNI_State *>(m_Expression);
	}

	SNI::SNI_Expression *SN_State::GetSNI_Expression() const
	{
		return const_cast<SNI::SNI_Expression *>(dynamic_cast<const SNI::SNI_Expression *>(m_Expression));
	}

	SN_Expression SN_State::IsA(const SN_Expression &p_Parent) const
	{
		return SN_Operators::IsA(*this, p_Parent);
	}
}
