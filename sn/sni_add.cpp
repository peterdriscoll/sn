#include "sni_add.h"

#include "sni_string.h"
#include "sni_null.h"
#include "sni_error.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Add::SNI_Add()
	{

	}

	SNI_Add::~SNI_Add()
	{

	}

	string SNI_Add::GetTypeName() const
	{
		return "Add";
	}

	string SNI_Add::DisplayCpp() const
	{
		return "SN::Add";
	}

	string SNI_Add::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "(+)";
	}

	long SNI_Add::GetPriority() const
	{
		return 1;
	}

	string SNI_Add::GetOperator() const
	{
		return "+";
	}
	/// @endcond

	bool SNI_Add::SupportsMultipleOutputs() const
	{
		return true;
	}

	SN::SN_Value SNI_Add::PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		if (p_Left.IsStringValue())
		{
			return p_Left.GetSNI_Value()->DoConcat(p_Right.GetSNI_Value());
		}
		return p_Left.GetSNI_Value()->DoAdd(p_Right.GetSNI_Value());
	}

	SN::SN_Value SNI_Add::LeftInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Left) const
	{
		if (p_Result.IsString())
		{
			return p_Result.GetSNI_Value()->DoSubtractLeft(p_Left.GetSNI_Value());
		}
		return p_Result.GetSNI_Value()->DoSubtract(p_Left.GetSNI_Value());
	}

	SN::SN_Value SNI_Add::RightInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Right) const
	{
		if (p_Result.IsString())
		{
			return p_Result.GetSNI_Value()->DoSubtractRight(p_Right.GetSNI_Value());
		}
		return p_Result.GetSNI_Value()->DoSubtract(p_Right.GetSNI_Value());
	}

	SN::SN_Expression SNI_Add::LeftInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Right) const
	{
		return skynet::Subtract.PrimaryFunctionExpression(p_Result, p_Right);
	}

	SN::SN_Expression SNI_Add::RightInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Right) const
	{
		return skynet::Subtract.PrimaryFunctionExpression(p_Result, p_Right);
	}
}
