#include "sni_same.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Same::SNI_Same()
	{
	}

	SNI_Same::~SNI_Same()
	{
	}

	void SNI_Same::PromoteMembers()
	{
	}

	string SNI_Same::GetTypeName() const
	{
		return "Not";
	}

	string SNI_Same::DisplayCpp() const
	{
		return "SN::Same";
	}

	string SNI_Same::DisplaySN(long /*priority*/, SNI_VariablePointerList & /*p_DisplayVariableList*/) const
	{
		return "Assert equal";
	}

	long SNI_Same::GetPriority() const
	{
		return 4;
	}

	string SNI_Same::GetOperator() const
	{
		return "==";
	}

	SN::SN_Value SNI_Same::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param;
	}

	SN::SN_Expression SNI_Same::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param;
	}

	size_t SNI_Same::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (p_Depth == 2)
		{
			return MultiplyCardinality(p_ParamList[PU1_First].Cardinality(), p_ParamList[PU1_Result].Cardinality());
		}
		return CARDINALITY_MAX;
	}
}
