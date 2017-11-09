#include "sni_unaryor.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_UnaryOr::SNI_UnaryOr()
	{
	}

	SNI_UnaryOr::~SNI_UnaryOr()
	{
	}

	string SNI_UnaryOr::GetTypeName() const
	{
		return "UnaryOr";
	}

	string SNI_UnaryOr::DisplayCpp() const
	{
		return "SN::UnaryOr";
	}

	string SNI_UnaryOr::DisplaySN(long /*priority*/, SNI_VariablePointerList & /*p_DisplayVariableList*/) const
	{
		return "UnaryOr";
	}

	long SNI_UnaryOr::GetPriority() const
	{
		return 4;
	}

	bool SNI_UnaryOr::AllowDelay() const
	{
		return false;
	}

	SN::SN_Value SNI_UnaryOr::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoUnaryOr();
	}

	SN::SN_Expression SNI_UnaryOr::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.UnaryOr();
	}

	SN::SN_Value SNI_UnaryOr::InverseFunctionValue(const SN::SN_Value & p_Param) const
	{
		return p_Param.GetSNI_Value()->DoUnaryAnd();
	}

	SN::SN_Expression SNI_UnaryOr::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return p_Param.UnaryAnd();
	}

	size_t SNI_UnaryOr::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (p_TotalCalc <= 1)
		{
			SN::SN_Bool result = p_ParamList[PU1_Result].GetVariableValue();
			if (result.IsKnownValue() && !result.GetBool())
			{
				return 1;
			}
			SN::SN_Bool first = p_ParamList[PU1_First].GetVariableValue();
			if (first.IsKnownValue() && first.GetBool())
			{
				return 1;
			}
			return MultiplyCardinality(p_ParamList[PU1_First].Cardinality(), p_ParamList[PU1_Result].Cardinality(), 2);
		}
		return CARDINALITY_MAX;
	}
}
