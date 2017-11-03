#include "sni_subtractleft.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_SubtractLeft::SNI_SubtractLeft()
	{
	}

	SNI_SubtractLeft::~SNI_SubtractLeft()
	{
	}

	void SNI_SubtractLeft::PromoteMembers()
	{
	}

	string SNI_SubtractLeft::GetTypeName() const
	{
		return "SubtractLeft";
	}

	string SNI_SubtractLeft::DisplayCpp() const
	{
		return "SN::SubtractLeft";
	}

	string SNI_SubtractLeft::DisplaySN(long /*priority*/, SNI_VariablePointerList & /*p_DisplayVariableList*/) const
	{
		return "SubtractLeft";
	}

	long SNI_SubtractLeft::GetPriority() const
	{
		return 1;
	}

	string SNI_SubtractLeft::GetOperator() const
	{
		return "SubtractLeft";
	}

	SN::SN_Value SNI_SubtractLeft::PrimaryFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Left) const
	{
		return p_Result.GetSNI_Value()->DoSubtractLeft(p_Left.GetSNI_Value());
	}

	SN::SN_Expression SNI_SubtractLeft::PrimaryFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Left) const
	{
		return p_Result.SubtractLeft(p_Left);
	}

	SN::SN_Value SNI_SubtractLeft::LeftInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Right) const
	{
		return p_Right.GetSNI_Value()->DoSubtractRight(p_Result.GetSNI_Value());
	}

	SN::SN_Expression SNI_SubtractLeft::LeftInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Right) const
	{
		return p_Right.SubtractRight(p_Result);
	}

	SN::SN_Value SNI_SubtractLeft::RightInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Right) const
	{
		return p_Right.GetSNI_Value()->DoConcat(p_Result.GetSNI_Value());
	}

	SN::SN_Expression SNI_SubtractLeft::RightInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Right) const
	{
		return p_Right.Concat(p_Result);
	}
}
