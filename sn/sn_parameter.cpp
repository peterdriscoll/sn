#include "sn_parameter.h"

#include "sni_helpers.h"

#include "sn_pch.h"

namespace SN
{
	SN_Parameter::SN_Parameter()
	{
	}

	SN_Parameter::SN_Parameter(const SN_Expression &p_Value, const SN_Expression &p_Condition)
		: m_Value(p_Value)
		, m_Condition(p_Condition)
	{
	}

	SN_Parameter::SN_Parameter(const SN_Expression &p_Value)
		: m_Value(p_Value)
	{
	}

	SN_Parameter::~SN_Parameter()
	{

	}

	SN_Expression &SN_Parameter::GetValue()
	{
		return m_Value;
	}

	SN_Expression &SN_Parameter::GetCondition()
	{
		return m_Condition;
	}

	void SN_Parameter::UpdateValue()
	{
		m_Value = m_Value.Evaluate();
	}
}
