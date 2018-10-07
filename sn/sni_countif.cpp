#include "sni_countif.h"

#include "logcontext.h"
#include "sn_parameter.h"
#include "sn_error.h"

#include "sni_helpers.h"
#include "sni_null.h"
#include "sni_error.h"
#include "sni_result.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_CountIf::SNI_CountIf()
	{
	}

	SNI_CountIf::~SNI_CountIf()
	{
	}

	string SNI_CountIf::GetTypeName() const
	{
		return "CountIf";
	}

	string SNI_CountIf::DisplayCpp() const
	{
		return "SN::CountIf";
	}

	string SNI_CountIf::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "CountIf";
	}

	long SNI_CountIf::GetPriority() const
	{
		return 0;
	}

	SN::SN_Value SNI_CountIf::PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Left.GetSNI_Value()->DoCountIf(p_Right.GetSNI_Value());
	}

	SN::SN_Expression SNI_CountIf::PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return p_Left.CountIf(p_Right);
	}

	SN::SN_Value SNI_CountIf::LeftInverseFunctionValue(const SN::SN_Value & p_Left, const SN::SN_Value & p_Right) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_CountIf::LeftInverseFunctionExpression(const SN::SN_Expression & p_Left, const SN::SN_Expression & p_Right) const
	{
		return skynet::Null;
	}

	size_t SNI_CountIf::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (p_TotalCalc <= 1)
		{
			if (p_CalcPos == PU2_Result)
			{
				if (p_ParamList[PU2_First].IsFixed())
				{
					return SNI_Binary::CardinalityOfUnify(p_Depth, p_ParamList, p_CalcPos, p_TotalCalc);
				}
			}
		}
		return CARDINALITY_MAX;
	}
}
