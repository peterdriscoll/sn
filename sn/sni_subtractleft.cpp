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

	string SNI_SubtractLeft::GetTypeName() const
	{
		return "SubtractLeft";
	}

	string SNI_SubtractLeft::DisplayCpp() const
	{
		return "SN::SubtractLeft";
	}

	string SNI_SubtractLeft::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "SubtractLeft";
	}

	long SNI_SubtractLeft::GetPriority() const
	{
		return 1;
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

	bool SNI_SubtractLeft::IsKnownValue(const SN::SN_Expression &p_Param, long j) const
	{
		return j == PU1_First ? p_Param.IsLeftKnownValue() : p_Param.IsKnownValue();
	}

	size_t SNI_SubtractLeft::ParamCardinality(const SN::SN_Expression &p_Param, long j) const
	{
		return j == PU1_First ? p_Param.LeftCardinality() : p_Param.Cardinality();
	}

	size_t SNI_SubtractLeft::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (p_TotalCalc <= 1)
		{
			if (p_ParamList[PU2_First].IsLeftKnownValue() && p_ParamList[PU2_Second].IsKnownValue())
			{
				if (p_TotalCalc == 0)
				{
					return MultiplyCardinality(p_ParamList[PU2_First].LeftCardinality(), MultiplyCardinality(p_ParamList[PU2_Second].Cardinality(), p_ParamList[PU2_Result].Cardinality()));
				}
				return MultiplyCardinality(p_ParamList[PU2_First].LeftCardinality(), p_ParamList[PU2_Second].Cardinality());
			}
		}
		return CARDINALITY_MAX;
	}

	size_t SNI_SubtractLeft::CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const
	{
		if (p_ParamList[PC2_First].IsLeftKnownValue() && p_ParamList[PC2_Second].IsKnownValue())
		{
			return MultiplyCardinality(p_ParamList[PC2_First].LeftCardinality(), p_ParamList[PC2_Second].Cardinality());
		}
		return CARDINALITY_MAX;
	}
}
