#include "sni_unaryrevand.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_UnaryRevAnd::SNI_UnaryRevAnd()
	{
	}

	SNI_UnaryRevAnd::~SNI_UnaryRevAnd()
	{
	}

	string SNI_UnaryRevAnd::GetTypeName() const
	{
		return "Not";
	}

	string SNI_UnaryRevAnd::DisplayCpp() const
	{
		return "SN::UnaryRevAnd";
	}

	string SNI_UnaryRevAnd::DisplaySN(long /*priority*/, SNI_VariablePointerList & /*p_DisplayVariableList*/) const
	{
		return "Assert equal";
	}

	long SNI_UnaryRevAnd::GetPriority() const
	{
		return 4;
	}

	string SNI_UnaryRevAnd::GetOperator() const
	{
		return "==";
	}

	SN::SN_Value SNI_UnaryRevAnd::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoUnaryRevAnd();
	}

	SN::SN_Expression SNI_UnaryRevAnd::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.UnaryRevAnd();
	}

	SN::SN_Value SNI_UnaryRevAnd::InverseFunctionValue(const SN::SN_Value & p_Param) const
	{
		return p_Param.GetSNI_Value()->DoUnaryRevOr();
	}

	SN::SN_Expression SNI_UnaryRevAnd::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return p_Param.UnaryRevOr();
	}

	size_t SNI_UnaryRevAnd::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (p_TotalCalc <= 1)
		{
			SN::SN_Bool result = p_ParamList[PU1_Result].GetVariableValue();
			if (!result.IsNull() && result.GetBool())
			{
				return 1;
			}
			SN::SN_Bool first = p_ParamList[PU1_First].GetVariableValue();
			if (!first.IsNull() && !first.GetBool())
			{
				return 1;
			}
			return MultiplyCardinality(p_ParamList[PU1_First].Cardinality(), p_ParamList[PU1_Result].Cardinality());
		}
		return CARDINALITY_MAX;
	}
}
