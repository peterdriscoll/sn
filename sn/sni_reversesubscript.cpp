#include "sni_reversesubscript.h"

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
	SNI_ReverseSubscript::SNI_ReverseSubscript()
	{
	}

	SNI_ReverseSubscript::~SNI_ReverseSubscript()
	{
	}

	std::string SNI_ReverseSubscript::GetTypeName() const
	{
		return "ReverseSubscript";
	}

	std::string SNI_ReverseSubscript::DisplayCpp() const
	{
		return "SN::ReverseSubscript";
	}

	std::string SNI_ReverseSubscript::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "ReverseSubscript";
	}

	long SNI_ReverseSubscript::GetPriority() const
	{
		return 0;
	}

	SN::SN_Error SNI_ReverseSubscript::AssertValue(const SN::SN_Value & p_Left, const SN::SN_Value & p_Right, SN::SN_Expression & p_Result) const
	{
		return p_Left.GetSNI_Value()->AssertSubscriptValue(p_Right.GetSNI_Value(), p_Result);
	}

	SN::SN_Value SNI_ReverseSubscript::PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Left.GetSNI_Value()->DoReverseSubscript(p_Right.GetSNI_Value());
	}

	SN::SN_Value SNI_ReverseSubscript::LeftInverseFunctionValue(const SN::SN_Value & p_Left, const SN::SN_Value & p_Right) const
	{
		return p_Left.GetSNI_Value()->DoSubscriptCall(p_Right.GetSNI_Value());
	}

	SN::SN_Expression SNI_ReverseSubscript::LeftInverseFunctionExpression(const SN::SN_Expression & p_Left, const SN::SN_Expression & p_Right) const
	{
		return skynet::Subscript.PrimaryFunctionExpression(p_Left, p_Right);
	}

	SN::SN_Value SNI_ReverseSubscript::RightInverseFunctionValue(const SN::SN_Value & p_Left, const SN::SN_Value & p_Right) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_ReverseSubscript::RightInverseFunctionExpression(const SN::SN_Expression & p_Left, const SN::SN_Expression & p_Right) const
	{
		return skynet::Null;
	}

	size_t SNI_ReverseSubscript::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (!p_ParamList[PU2_Result].IsKnownValue())
		{
			return CARDINALITY_MAX;
		}
		return SNI_Binary::CardinalityOfUnify(p_Depth, p_ParamList, p_CalcPos, p_TotalCalc);
	}

	SN::SN_Error SNI_ReverseSubscript::UnifyElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet * worldSet, const SNI_Expression *p_Source) const
	{
		LOGGING(SN::LogContext context("SNI_IsA::UnifyElement(CalcPos " + std::to_string(p_CalcPos) + " TotalCalc " + std::to_string(p_TotalCalc) + " " + DisplayValues(p_Depth, p_ParamList, p_WorldList) + ")"));
		if (worldSet)
		{
			LOGGING(context.LogText("World set", worldSet->DisplayLong()));
		}
		switch (p_TotalCalc)
		{
		case 0:
		{
			if (p_WorldList)
			{
				bool exists = false;
				SNI_World *world = worldSet->JoinWorldsArray(ManualAddWorld, AlwaysCreateWorld, exists, p_Depth, p_WorldList);
				if (exists)
				{
					SN::SN_Error result = AssertValue(p_ParamList[PU2_First].GetVariableValue(), p_ParamList[PU2_Second].GetVariableValue(), p_ParamList[PU2_Result]);
					if (result.GetBool())
					{
						world->AddToSetList();
					}
					else
					{
						LOGGING(context.LogText("fail", "Value conflict on " + DisplayValues(p_Depth, p_ParamList, p_WorldList)));
					}
				}
				else
				{
					LOGGING(context.LogText("fail", "Join worlds failed on " + DisplayWorlds(p_Depth, p_WorldList)));
				}
				return skynet::OK;
			}
			else
			{
				return AssertValue(p_ParamList[PU2_First].GetVariableValue(), p_ParamList[PU2_Second].GetVariableValue(), p_ParamList[PU2_Result]);
			}
		}
		case 1:
		{
			if (p_CalcPos == PU2_Result)
			{
				return AssertValue(p_ParamList[PU2_First].GetVariableValue(), p_ParamList[PU2_Second].GetVariableValue(), p_ParamList[PU2_Result]);
			}
		}
		}
		return SN::SN_Error(false, false, GetTypeName() + ": Expression not unified. TotalCalc=" + std::to_string(p_TotalCalc) + " Calcpos=" + std::to_string(p_CalcPos));
	}
}
