#include "sni_subtract.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Subtract::SNI_Subtract()
	{
	}

	SNI_Subtract::~SNI_Subtract()
	{
	}

	std::string SNI_Subtract::GetTypeName() const
	{
		return "Subtract";
	}

	std::string SNI_Subtract::DisplayCpp() const
	{
		return "SN::Subtract";
	}

	std::string SNI_Subtract::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "(-)";
	}

	long SNI_Subtract::GetPriority() const
	{
		return 1;
	}

	std::string SNI_Subtract::GetOperator() const
	{
		return "-";
	}

	SN::SN_Value SNI_Subtract::PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Left.GetSNI_Value()->DoSubtract(p_Right.GetSNI_Value());
	}

	SN::SN_Value SNI_Subtract::LeftInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Left) const
	{
		return p_Left.GetSNI_Value()->DoSubtract(p_Result.GetSNI_Value());
	}

	SN::SN_Expression SNI_Subtract::LeftInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Left) const
	{
		return PrimaryFunctionExpression(p_Left, p_Result);
	}

	SN::SN_Value SNI_Subtract::RightInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Right) const
	{
		return p_Result.GetSNI_Value()->DoAdd(p_Right.GetSNI_Value());;
	}

	SN::SN_Expression SNI_Subtract::RightInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Right) const
	{
		return skynet::Add.PrimaryFunctionExpression(p_Result, p_Right);
	}
}
