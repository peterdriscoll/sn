#include "sni_unaryand.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_UnaryAnd::SNI_UnaryAnd()
	{
	}

	SNI_UnaryAnd::~SNI_UnaryAnd()
	{
	}

	string SNI_UnaryAnd::GetTypeName() const
	{
		return "Not";
	}

	string SNI_UnaryAnd::DisplayCpp() const
	{
		return "SN::UnaryAnd";
	}

	string SNI_UnaryAnd::DisplaySN(long /*priority*/, SNI_VariablePointerList & /*p_DisplayVariableList*/) const
	{
		return "Assert equal";
	}

	long SNI_UnaryAnd::GetPriority() const
	{
		return 4;
	}

	string SNI_UnaryAnd::GetOperator() const
	{
		return "==";
	}

	bool SNI_UnaryAnd::AllowDelay() const
	{
		return false;
	}

	SN::SN_Value SNI_UnaryAnd::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoUnaryAnd();
	}

	SN::SN_Expression SNI_UnaryAnd::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.UnaryAnd();
	}

	SN::SN_Value SNI_UnaryAnd::InverseFunctionValue(const SN::SN_Value & p_Param) const
	{
		return p_Param.GetSNI_Value()->DoUnaryOr();
	}

	SN::SN_Expression SNI_UnaryAnd::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return p_Param.UnaryOr();
	}

	size_t SNI_UnaryAnd::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
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
