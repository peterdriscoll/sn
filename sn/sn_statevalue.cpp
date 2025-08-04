#include "sn_statevalue.h"

#include "sn.h"
#include "sn_expression.h"
#include "sn_value.h"

#include "sni_statevalue.h"

#include "sn_pch.h"

namespace SN
{
	/*static*/ SN_Class SN_StateValue::Class()
	{
		return SNI_StateValue::Class();
	}

	SN_StateValue::SN_StateValue()
		: SN_Base(NULL)
	{
	}

	SN_StateValue::SN_StateValue(const SN_StateValue &p_StateValue)
		: SN_Base(p_StateValue.m_Expression)
	{
	}

	SN_StateValue::SN_StateValue(const SN_Expression & p_Value, const SN_Expression& p_State)
		: SN_Base(new SNI::SNI_StateValue(p_Value.GetSNI_Expression(), p_State.GetSNI_Expression()))
	{
	}

	SN_StateValue::~SN_StateValue()
	{
	}

	SN_Expression SN_StateValue::operator==(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator==(*this, p_Other);
	}

	SN_Expression SN_StateValue::operator!=(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator!=(*this, p_Other);
	}

	SNI::SNI_StateValue *SN_StateValue::GetSNI_StateValue() const
	{
		return const_cast<SNI::SNI_StateValue *>(m_Expression);
	}

	SNI::SNI_Expression *SN_StateValue::GetSNI_Expression() const
	{
		return const_cast<SNI::SNI_Expression *>(dynamic_cast<const SNI::SNI_Expression *>(m_Expression));
	}

	SN_Expression SN_StateValue::Assert() const
	{
		return SN_Operators::Assert(*this);
	}

	SN_Expression SN_StateValue::PartialAssert() const
	{
		return SN_Operators::PartialAssert(*this);
	}

	SN_Expression SN_StateValue::Evaluate() const
	{
		return SN_Operators::Evaluate(*this);
	}

	SN_Expression SN_StateValue::PartialEvaluate() const
	{
		return SN_Operators::PartialEvaluate(*this);
	}

	SN_Expression SN_StateValue::IsA(const SN_Expression &p_Parent) const
	{
		return SN_Operators::IsA(*this, p_Parent);
	}
}
