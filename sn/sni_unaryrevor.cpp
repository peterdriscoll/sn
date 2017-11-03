#include "sni_unaryrevor.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_UnaryRevOr::SNI_UnaryRevOr()
	{
	}

	SNI_UnaryRevOr::~SNI_UnaryRevOr()
	{
	}

	string SNI_UnaryRevOr::GetTypeName() const
	{
		return "UnaryRevOr";
	}

	string SNI_UnaryRevOr::DisplayCpp() const
	{
		return "SN::UnaryRevOr";
	}

	string SNI_UnaryRevOr::DisplaySN(long /*priority*/, SNI_VariablePointerList & /*p_DisplayVariableList*/) const
	{
		return "UnaryRevOr";
	}

	long SNI_UnaryRevOr::GetPriority() const
	{
		return 4;
	}

	SN::SN_Value SNI_UnaryRevOr::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoUnaryRevOr();
	}

	SN::SN_Expression SNI_UnaryRevOr::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.UnaryRevOr();
	}

	SN::SN_Value SNI_UnaryRevOr::InverseFunctionValue(const SN::SN_Value & p_Param) const
	{
		return p_Param.GetSNI_Value()->DoUnaryRevAnd();
	}

	SN::SN_Expression SNI_UnaryRevOr::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return p_Param.UnaryRevAnd();
	}

	size_t SNI_UnaryRevOr::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (p_TotalCalc <= 1)
		{
			return 1;
			SN::SN_Bool result = p_ParamList[PU1_Result].GetVariableValue();
			if (!result.IsNull() && !result.GetBool())
			{
				return 1;
			}
			SN::SN_Bool first = p_ParamList[PU1_First].GetVariableValue();
			if (!first.IsNull() && first.GetBool())
			{
				return 1;
			}
			return 2; // MultiplyCardinality(p_ParamList[PU1_First].Cardinality(), p_ParamList[PU1_Result].Cardinality());
		}
		return CARDINALITY_MAX;
	}
}
