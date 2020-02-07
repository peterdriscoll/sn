#include "sni_countall.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

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
	SNI_CountAll::SNI_CountAll()
	{
	}

	SNI_CountAll::~SNI_CountAll()
	{
	}

	string SNI_CountAll::GetTypeName() const
	{
		return "CountAll";
	}

	string SNI_CountAll::DisplayCpp() const
	{
		return "SN::CountAll";
	}

	string SNI_CountAll::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "CountAll";
	}

	long SNI_CountAll::GetPriority() const
	{
		return 0;
	}

	SN::SN_Value SNI_CountAll::PrimaryFunctionValue(const SN::SN_Value &p_Left) const
	{
		return p_Left.GetSNI_Value()->DoCountAll();
	}

	SN::SN_Expression SNI_CountAll::PrimaryFunctionExpression(const SN::SN_Expression &p_Left) const
	{
		return p_Left.CountAll();
	}

	SN::SN_Value SNI_CountAll::InverseFunctionValue(const SN::SN_Value & p_Param) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_CountAll::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return skynet::Null;
	}

	size_t SNI_CountAll::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
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
