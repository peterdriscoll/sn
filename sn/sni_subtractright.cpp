#include "sni_subtractright.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_SubtractRight::SNI_SubtractRight()
	{
	}

	SNI_SubtractRight::~SNI_SubtractRight()
	{
	}

	void SNI_SubtractRight::PromoteMembers()
	{
	}

	string SNI_SubtractRight::GetTypeName() const
	{
		return "SubtractRight";
	}

	string SNI_SubtractRight::DisplayCpp() const
	{
		return "SN::SubtractRight";
	}

	string SNI_SubtractRight::DisplaySN(long /*priority*/, SNI_VariablePointerList & /*p_DisplayVariableList*/) const
	{
		return "SubtractRight";
	}

	long SNI_SubtractRight::GetPriority() const
	{
		return 100;
	}

	string SNI_SubtractRight::GetOperator() const
	{
		return "SubtractRight";
	}

	SN::SN_Value SNI_SubtractRight::PrimaryFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Right) const
	{
		return p_Result.GetSNI_Value()->DoSubtractRight(p_Right.GetSNI_Value());
	}

	SN::SN_Expression SNI_SubtractRight::PrimaryFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Right) const
	{
		return p_Result.SubtractRight(p_Right);
	}

	SN::SN_Value SNI_SubtractRight::LeftInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Result) const
	{
		return p_Result.GetSNI_Value()->DoSubtractLeft(p_Left.GetSNI_Value());
	}

	SN::SN_Expression SNI_SubtractRight::LeftInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Result) const
	{
		return p_Result.SubtractLeft(p_Left);
	}

	SN::SN_Value SNI_SubtractRight::RightInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Left.GetSNI_Value()->DoAdd(p_Right.GetSNI_Value());
	}

	SN::SN_Expression SNI_SubtractRight::RightInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return p_Left.Concat(p_Right);
	}
}
