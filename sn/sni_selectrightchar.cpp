#include "sni_selectrightchar.h"

#include "sn.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_SelectRightChar::SNI_SelectRightChar()
	{
	}

	SNI_SelectRightChar::~SNI_SelectRightChar()
	{
	}

	string SNI_SelectRightChar::GetTypeName() const
	{
		return "SelectRightChar";
	}

	string SNI_SelectRightChar::DisplayCpp() const
	{
		return "SN::SelectRightChar";
	}

	string SNI_SelectRightChar::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "SelectRightChar";
	}

	long SNI_SelectRightChar::GetPriority() const
	{
		return 1;
	}

	SN::SN_Value SNI_SelectRightChar::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoSelectRightChar();
	}

	SN::SN_Expression SNI_SelectRightChar::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.SelectRightChar();
	}

	SN::SN_Value SNI_SelectRightChar::InverseFunctionValue(const SN::SN_Value &p_Param) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_SelectRightChar::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return skynet::Null;
	}

	bool SNI_SelectRightChar::IsKnownValue(const SN::SN_Expression &p_Param, long j) const
	{
		return j == PU1_First ? p_Param.IsRightKnownValue() : p_Param.IsKnownValue();
	}

	size_t SNI_SelectRightChar::ParamCardinality(const SN::SN_Expression &p_Param, long j) const
	{
		return j == PU1_First ? p_Param.RightCardinality() : p_Param.Cardinality();
	}

	size_t SNI_SelectRightChar::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (p_Depth == 2)
		{
			if (p_ParamList[PU1_First].IsLeftKnownValue())
			{
				if (p_ParamList[PU1_Result].IsKnownValue())
				{
					return MultiplyCardinality(p_ParamList[PU1_Result].Cardinality(), p_ParamList[PU1_First].Cardinality());
				}
				return p_ParamList[PU1_First].LeftCardinality();
			}
		}
		return CARDINALITY_MAX;
	}

	size_t SNI_SelectRightChar::CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const
	{
		if (p_ParamList[PU1_First].IsRightKnownValue())
		{
			return p_ParamList[PU1_First].RightCardinality();
		}
		return CARDINALITY_MAX;
	}
}
