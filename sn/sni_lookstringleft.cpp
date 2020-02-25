#include "sni_lookstringleft.h"

#include "sn.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_LookStringLeft::SNI_LookStringLeft()
	{
	}

	SNI_LookStringLeft::~SNI_LookStringLeft()
	{
	}

	string SNI_LookStringLeft::GetTypeName() const
	{
		return "LookStringLeft";
	}

	string SNI_LookStringLeft::DisplayCpp() const
	{
		return "SN::LookStringLeft";
	}

	string SNI_LookStringLeft::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "LookStringLeft";
	}

	long SNI_LookStringLeft::GetPriority() const
	{
		return 1;
	}

	SN::SN_Value SNI_LookStringLeft::PrimaryFunctionValue(const SN::SN_Value & p_Left, const SN::SN_Value & p_Right) const
	{
		return p_Left.GetSNI_Value()->DoLookStringLeft(p_Right.GetSNI_Value());
	}

	SN::SN_Expression SNI_LookStringLeft::PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return p_Left.LookStringLeft(p_Right);
	}

	SN::SN_Value SNI_LookStringLeft::LeftInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_LookStringLeft::LeftInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return skynet::Null;
	}

	SN::SN_Value SNI_LookStringLeft::RightInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_LookStringLeft::RightInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return skynet::Null;
	}

	bool SNI_LookStringLeft::IsKnownValue(const SN::SN_Expression &p_Param, long j) const
	{
		return j == PU2_First ? p_Param.IsLeftKnownValue() : p_Param.IsKnownValue();
	}

	size_t SNI_LookStringLeft::ParamCardinality(const SN::SN_Expression &p_Param, long j) const
	{
		return j == PU2_First ? p_Param.LeftCardinality() : p_Param.Cardinality();
	}

	size_t SNI_LookStringLeft::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (p_Depth == 3)
		{
			if (p_ParamList[PU2_First].IsLeftKnownValue() && p_ParamList[PU2_Second].IsKnownValue())
			{
				return MultiplyCardinality(p_ParamList[PU2_First].LeftCardinality(), p_ParamList[PU2_Result].Cardinality());
			}
		}
		return CARDINALITY_MAX;
	}

	size_t SNI_LookStringLeft::CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const
	{
		if (p_ParamList[PC2_First].IsLeftKnownValue() && p_ParamList[PC2_Second].IsKnownValue())
		{
			return MultiplyCardinality(p_ParamList[PC2_First].LeftCardinality(), p_ParamList[PC2_Second].Cardinality());
		}
		return CARDINALITY_MAX;
	}
}
