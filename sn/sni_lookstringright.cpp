#include "sni_lookstringright.h"

#include "sn.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_LookStringRight::SNI_LookStringRight()
	{
	}

	SNI_LookStringRight::~SNI_LookStringRight()
	{
	}

	string SNI_LookStringRight::GetTypeName() const
	{
		return "LookStringRight";
	}

	string SNI_LookStringRight::DisplayCpp() const
	{
		return "SN::LookStringRight";
	}

	string SNI_LookStringRight::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "LookStringRight";
	}

	long SNI_LookStringRight::GetPriority() const
	{
		return 1;
	}

	SN::SN_Value SNI_LookStringRight::PrimaryFunctionValue(const SN::SN_Value & p_Left, const SN::SN_Value & p_Right) const
	{
		return p_Left.GetSNI_Value()->DoLookStringRight(p_Right.GetSNI_Value());
	}

	SN::SN_Expression SNI_LookStringRight::PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return p_Left.LookStringRight(p_Right);
	}

	SN::SN_Value SNI_LookStringRight::LeftInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_LookStringRight::LeftInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return skynet::Null;
	}

	SN::SN_Value SNI_LookStringRight::RightInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_LookStringRight::RightInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return skynet::Null;
	}

	bool SNI_LookStringRight::IsKnownValue(const SN::SN_Expression &p_Param, long j) const
	{
		return j == PU1_First ? p_Param.IsRightKnownValue() : p_Param.IsKnownValue();
	}

	size_t SNI_LookStringRight::ParamCardinality(const SN::SN_Expression &p_Param, long j) const
	{
		return j == PU1_First ? p_Param.RightCardinality() : p_Param.Cardinality();
	}

	size_t SNI_LookStringRight::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
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

	size_t SNI_LookStringRight::CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const
	{
		if (p_ParamList[PC2_First].IsRightKnownValue() && p_ParamList[PC2_Second].IsKnownValue())
		{
			return MultiplyCardinality(p_ParamList[PC2_First].RightCardinality(), p_ParamList[PC2_Second].Cardinality());
		}
		return CARDINALITY_MAX;
	}
}
