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

	string SNI_SubtractRight::GetTypeName() const
	{
		return "SubtractRight";
	}

	string SNI_SubtractRight::DisplayCpp() const
	{
		return "SN::SubtractRight";
	}

	string SNI_SubtractRight::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "SubtractRight";
	}

	long SNI_SubtractRight::GetPriority() const
	{
		return 100;
	}

	SN::SN_Value SNI_SubtractRight::PrimaryFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Right) const
	{
		return p_Result.GetSNI_Value()->DoSubtractRight(p_Right.GetSNI_Value());
	}

	SN::SN_Value SNI_SubtractRight::LeftInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Result) const
	{
		return p_Result.GetSNI_Value()->DoSubtractLeft(p_Left.GetSNI_Value());
	}

	SN::SN_Expression SNI_SubtractRight::LeftInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Result) const
	{
		return skynet::SubtractLeft.PrimaryFunctionExpression(p_Result, p_Left);
	}

	SN::SN_Value SNI_SubtractRight::RightInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Left.GetSNI_Value()->DoAdd(p_Right.GetSNI_Value());
	}

	SN::SN_Expression SNI_SubtractRight::RightInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return skynet::Concat.PrimaryFunctionExpression(p_Left, p_Right);
	}

	bool SNI_SubtractRight::IsKnownValue(const SN::SN_Expression &p_Param, long j) const
	{
		return j == PU1_First ? p_Param.IsLeftKnownValue() : p_Param.IsKnownValue();
	}

	size_t SNI_SubtractRight::ParamCardinality(const SN::SN_Expression &p_Param, long j) const
	{
		return j == PU1_First ? p_Param.LeftCardinality() : p_Param.Cardinality();
	}

	size_t SNI_SubtractRight::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (p_TotalCalc <= 1)
		{
			if (p_ParamList[PU2_First].IsRightKnownValue() && p_ParamList[PU2_Second].IsKnownValue())
			{
				if (p_TotalCalc == 0)
				{
					return MultiplyCardinality(p_ParamList[PU2_First].RightCardinality(), MultiplyCardinality(p_ParamList[PU2_Second].Cardinality(), p_ParamList[PU2_Result].Cardinality()));
				}
				return MultiplyCardinality(p_ParamList[PU2_First].RightCardinality(), p_ParamList[PU2_Second].Cardinality());
			}
		}
		return CARDINALITY_MAX;
	}

	size_t SNI_SubtractRight::CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const
	{
		if (p_ParamList[PC2_First].IsRightKnownValue() && p_ParamList[PC2_Second].IsKnownValue())
		{
			return MultiplyCardinality(p_ParamList[PC2_First].RightCardinality(), p_ParamList[PC2_Second].Cardinality());
		}
		return CARDINALITY_MAX;
	}
}
