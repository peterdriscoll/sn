#include "sni_negative.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Negative::SNI_Negative()
	{
	}

	SNI_Negative::~SNI_Negative()
	{
	}

	void SNI_Negative::PromoteMembers()
	{
	}

	string SNI_Negative::GetTypeName() const
	{
		return "Negative";
	}

	string SNI_Negative::DisplayCpp() const
	{
		return "SN::Negative";
	}

	string SNI_Negative::DisplaySN(long /*priority*/, SNI_VariablePointerList & /*p_DisplayVariableList*/) const
	{
		return "!";
	}

	long SNI_Negative::GetPriority() const
	{
		return 4;
	}

	string SNI_Negative::GetOperator() const
	{
		return "-";
	}

	SN::SN_Value SNI_Negative::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoNegative();
	}

	SN::SN_Expression SNI_Negative::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return -p_Param;
	}
}
