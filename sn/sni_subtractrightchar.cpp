#include "sni_subtractrightchar.h"
#include "sn.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_SubtractRightChar::SNI_SubtractRightChar()
	{
	}

	SNI_SubtractRightChar::~SNI_SubtractRightChar()
	{
	}

	string SNI_SubtractRightChar::GetTypeName() const
	{
		return "SubtractRightChar";
	}

	string SNI_SubtractRightChar::DisplayCpp() const
	{
		return "SN::SubtractRightChar";
	}

	string SNI_SubtractRightChar::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "SubtractRightChar";
	}

	long SNI_SubtractRightChar::GetPriority() const
	{
		return 1;
	}

	SN::SN_Value SNI_SubtractRightChar::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoSubtractRightChar();
	}

	SN::SN_Expression SNI_SubtractRightChar::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.SubtractRightChar();
	}

	SN::SN_Value SNI_SubtractRightChar::InverseFunctionValue(const SN::SN_Value &p_Param) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_SubtractRightChar::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return skynet::Null;
	}

	bool SNI_SubtractRightChar::IsKnownValue(const SN::SN_Expression &p_Param, long j) const
	{
		return j == PU1_First ? p_Param.IsRightKnownValue() : p_Param.IsKnownValue();
	}

	size_t SNI_SubtractRightChar::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (p_TotalCalc <= 1)
		{
			if (p_ParamList[PU1_First].IsRightKnownValue())
			{
				if (p_TotalCalc == 0)
				{
					return MultiplyCardinality(p_ParamList[PU1_First].RightCardinality(), p_ParamList[PU1_Result].Cardinality());
				}
				return p_ParamList[PU1_First].RightCardinality();
			}
		}
		return CARDINALITY_MAX;
	}

	size_t SNI_SubtractRightChar::CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const
	{
		if (p_ParamList[PU1_First].IsRightKnownValue())
		{
			return p_ParamList[PU1_First].RightCardinality();
		}
		return CARDINALITY_MAX;
	}
}
