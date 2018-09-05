#include "sni_divide.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Divide::SNI_Divide()
	{
	}

	SNI_Divide::~SNI_Divide()
	{
	}

	string SNI_Divide::GetTypeName() const
	{
		return "Divide";
	}

	string SNI_Divide::DisplayCpp() const
	{
		return "SN::Divide";
	}

	string SNI_Divide::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "(/)";
	}

	long SNI_Divide::GetPriority() const
	{
		return 2;
	}

	string SNI_Divide::GetOperator() const
	{
		return "/";
	}

	SN::SN_Value SNI_Divide::PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Left.GetSNI_Value()->DoDivide(p_Right.GetSNI_Value());
	}

	SN::SN_Expression SNI_Divide::PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return p_Left / p_Right;
	}

	SN::SN_Value SNI_Divide::LeftInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Left) const
	{
		return p_Left.GetSNI_Value()->DoDivide(p_Result.GetSNI_Value());
	}

	SN::SN_Expression SNI_Divide::LeftInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Left) const
	{
		return p_Left / p_Result;
	}

	SN::SN_Value SNI_Divide::RightInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Right) const
	{
		return p_Result.GetSNI_Value()->DoMultiply(p_Right.GetSNI_Value());
	}

	SN::SN_Expression SNI_Divide::RightInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Right) const
	{
		return p_Result * p_Right;
	}

}
