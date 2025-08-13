#include "sni_selectleftchar.h"

#include "sn.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_SelectLeftChar::SNI_SelectLeftChar()
	{
	}

	SNI_SelectLeftChar::~SNI_SelectLeftChar()
	{
	}

	std::string SNI_SelectLeftChar::GetTypeName() const
	{
		return "SelectLeftChar";
	}

	std::string SNI_SelectLeftChar::DisplayCpp() const
	{
		return "SN::SelectLeftChar";
	}

	std::string SNI_SelectLeftChar::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "SelectLeftChar";
	}

	long SNI_SelectLeftChar::GetPriority() const
	{
		return 1;
	}

	SN::SN_Value SNI_SelectLeftChar::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoSelectLeftChar();
	}

	SN::SN_Value SNI_SelectLeftChar::InverseFunctionValue(const SN::SN_Value &p_Param) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_SelectLeftChar::InverseFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return skynet::Null;
	}

	bool SNI_SelectLeftChar::IsKnownValue(const SN::SN_Expression &p_Param, long j) const
	{
		return j == PU1_First ? p_Param.IsLeftKnownValue() : p_Param.IsKnownValue();
	}

	size_t SNI_SelectLeftChar::ParamCardinality(const SN::SN_Expression &p_Param, long j) const
	{
		return j == PU1_First ? p_Param.LeftCardinality() : p_Param.Cardinality();
	}

	size_t SNI_SelectLeftChar::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (p_TotalCalc <= 1)
		{
			if (p_ParamList[PU1_First].IsLeftKnownValue())
			{
				if (p_TotalCalc == 0)
				{
					return MultiplyCardinality(p_ParamList[PU1_First].LeftCardinality(), p_ParamList[PU1_Result].Cardinality());
				}
				return p_ParamList[PU1_First].LeftCardinality();
			}
		}
		return CARDINALITY_MAX;
	}

	size_t SNI_SelectLeftChar::CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const
	{
		if (p_ParamList[PU1_First].IsLeftKnownValue())
		{
			return p_ParamList[PU1_First].LeftCardinality();
		}
		return CARDINALITY_MAX;
	}
}
