#include "sni_sum.h"

#include "sn_error.h"

#include "sni_helpers.h"
#include "sni_null.h"
#include "sni_error.h"
#include "sni_result.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Sum::SNI_Sum()
	{
	}

	SNI_Sum::~SNI_Sum()
	{
	}

	string SNI_Sum::GetTypeName() const
	{
		return "Sum";
	}

	string SNI_Sum::DisplayCpp() const
	{
		return "SN::Sum";
	}

	string SNI_Sum::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "Sum";
	}

	long SNI_Sum::GetPriority() const
	{
		return 0;
	}

	SN::SN_Value SNI_Sum::PrimaryFunctionValue(const SN::SN_Value &p_Left) const
	{
		return p_Left.GetSNI_Value()->DoSum();
	}

	SN::SN_Expression SNI_Sum::PrimaryFunctionExpression(const SN::SN_Expression &p_Left) const
	{
		return p_Left.Sum();
	}

	SN::SN_Value SNI_Sum::InverseFunctionValue(const SN::SN_Value & p_Param) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_Sum::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return skynet::Null;
	}

	size_t SNI_Sum::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (p_TotalCalc <= 1)
		{
			if (p_CalcPos == PU2_Result)
			{
				if (p_ParamList[PU2_First].IsFixed())
				{
					return SNI_Unary::CardinalityOfUnify(p_Depth, p_ParamList, p_CalcPos, p_TotalCalc);
				}
			}
		}
		return CARDINALITY_MAX;
	}
}
