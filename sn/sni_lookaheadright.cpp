#include "sni_lookaheadright.h"

#include "sn.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_LookaheadRight::SNI_LookaheadRight()
	{
	}

	SNI_LookaheadRight::~SNI_LookaheadRight()
	{
	}

	string SNI_LookaheadRight::GetTypeName() const
	{
		return "LookaheadRight";
	}

	string SNI_LookaheadRight::DisplayCpp() const
	{
		return "SN::LookaheadRight";
	}

	string SNI_LookaheadRight::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "LookaheadRight";
	}

	long SNI_LookaheadRight::GetPriority() const
	{
		return 1;
	}

	SN::SN_Value SNI_LookaheadRight::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoLookaheadRight();
	}

	SN::SN_Expression SNI_LookaheadRight::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.LookaheadRight();
	}

	SN::SN_Value SNI_LookaheadRight::InverseFunctionValue(const SN::SN_Value &p_Param) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_LookaheadRight::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return skynet::Null;
	}

	bool SNI_LookaheadRight::IsKnownValue(const SN::SN_Expression &p_Param, long j) const
	{
		return j == PU1_First ? p_Param.IsRightKnownValue() : p_Param.IsKnownValue();
	}

	size_t SNI_LookaheadRight::ParamCardinality(const SN::SN_Expression &p_Param, long j) const
	{
		return j == PU1_First ? p_Param.RightCardinality() : p_Param.Cardinality();
	}

	size_t SNI_LookaheadRight::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
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

	size_t SNI_LookaheadRight::CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const
	{
		if (p_ParamList[PU1_First].IsRightKnownValue())
		{
			return p_ParamList[PU1_First].RightCardinality();
		}
		return CARDINALITY_MAX;
	}
}
