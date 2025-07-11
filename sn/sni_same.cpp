#include "sni_same.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Same::SNI_Same()
	{
	}

	SNI_Same::~SNI_Same()
	{
	}

	string SNI_Same::GetTypeName() const
	{
		return "Same";
	}

	string SNI_Same::DisplayCpp() const
	{
		return "SN::Same";
	}

	string SNI_Same::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "Assert equal";
	}

	long SNI_Same::GetPriority() const
	{
		return 4;
	}

	string SNI_Same::GetOperator() const
	{
		return "==";
	}

	string SNI_Same::DisplayCall(long priority, SNI_DisplayOptions &p_DisplayOptions, size_t p_NumParams, SN::SN_Expression *p_ParamList, const SNI_Expression *p_DebugSource) const
	{
		return p_ParamList[PC1_First].GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions);
	}

	SN::SN_Value SNI_Same::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param;
	}

	SN::SN_Expression SNI_Same::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param;
	}

	size_t SNI_Same::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		long totalCalc = p_TotalCalc;
		long calcPos = p_CalcPos;
		if (p_ParamList[PU1_Result].IsReferableValue() && p_ParamList[PU1_First].IsNullValue())
		{
			totalCalc--;
			calcPos = PU1_First;
		}
		else if (p_ParamList[PU1_First].IsReferableValue() && p_ParamList[PU1_Result].IsNullValue())
		{
			totalCalc--;
			calcPos = PU1_Result;
		}
		if (totalCalc > 1)
		{
			return CARDINALITY_MAX;
		}
		return SNI_Unary::CardinalityOfUnify(p_Depth, p_ParamList, calcPos, totalCalc);
	}
}
