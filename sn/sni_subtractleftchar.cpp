#include "sni_subtractleftchar.h"

#include "sn.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_SubtractLeftChar::SNI_SubtractLeftChar()
	{
	}

	SNI_SubtractLeftChar::~SNI_SubtractLeftChar()
	{
	}

	string SNI_SubtractLeftChar::GetTypeName() const
	{
		return "SubtractLeftChar";
	}

	string SNI_SubtractLeftChar::DisplayCpp() const
	{
		return "SN::SubtractLeftChar";
	}

	string SNI_SubtractLeftChar::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "SubtractLeftChar";
	}

	long SNI_SubtractLeftChar::GetPriority() const
	{
		return 1;
	}

	SN::SN_Value SNI_SubtractLeftChar::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoSubtractLeftChar();
	}

	SN::SN_Expression SNI_SubtractLeftChar::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.SubtractLeftChar();
	}

	SN::SN_Value SNI_SubtractLeftChar::InverseFunctionValue(const SN::SN_Value &p_Param) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_SubtractLeftChar::InverseFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return skynet::Null;
	}

	bool SNI_SubtractLeftChar::IsKnownValue(const SN::SN_Expression &p_Param, long j) const
	{
		return j == PU1_First ? p_Param.IsLeftKnownValue() : p_Param.IsKnownValue();
	}

	size_t SNI_SubtractLeftChar::ParamCardinality(const SN::SN_Expression &p_Param, long j) const
	{
		return j == PU1_First ? p_Param.LeftCardinality() : p_Param.Cardinality();
	}

	size_t SNI_SubtractLeftChar::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
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

	size_t SNI_SubtractLeftChar::CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const
	{
		if (p_ParamList[PU1_First].IsLeftKnownValue())
		{
			return p_ParamList[PU1_First].LeftCardinality();
		}
		return CARDINALITY_MAX;
	}
}
