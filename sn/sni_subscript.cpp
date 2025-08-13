#include "sni_subscript.h"
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
	SNI_Subscript::SNI_Subscript()
	{
	}

	SNI_Subscript::~SNI_Subscript()
	{
	}

	std::string SNI_Subscript::GetTypeName() const
	{
		return "Subscript";
	}

	std::string SNI_Subscript::DisplayCpp() const
	{
		return "SN::Subscript";
	}

	std::string SNI_Subscript::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "Subscript";
	}

	std::string SNI_Subscript::DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, size_t p_NumParams, SN::SN_Expression *p_ParamList, const SNI_Expression *p_DebugSource) const
	{
		return Bracket(priority, p_ParamList[PC2_First].DisplaySN(GetPriority(), p_DisplayOptions) + SetBreakPoint("[", p_DisplayOptions, p_DebugSource, SN::LeftId) + p_ParamList[PC2_Second].DisplaySN(GetPriority(), p_DisplayOptions) + SetBreakPoint("]", p_DisplayOptions, p_DebugSource, SN::RightId), p_DisplayOptions, p_DebugSource);
	}

	long SNI_Subscript::GetPriority() const
	{
		return 0;
	}

	SN::SN_Error SNI_Subscript::AssertValue(const SN::SN_Value & p_Left, const SN::SN_Value & p_Right, SN::SN_Expression & p_Result) const
	{
		return p_Left.GetSNI_Value()->AssertSubscriptValue(p_Right.GetSNI_Value(), p_Result);
	}

	SN::SN_Value SNI_Subscript::PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Left.GetSNI_Value()->DoSubscriptCall(p_Right.GetSNI_Value());
	}

	SN::SN_Value SNI_Subscript::LeftInverseFunctionValue(const SN::SN_Value & p_Left, const SN::SN_Value & p_Right) const
	{
		return p_Right.GetSNI_Value()->DoReverseSubscript(p_Left.GetSNI_Value());
	}

	SN::SN_Expression SNI_Subscript::LeftInverseFunctionExpression(const SN::SN_Expression & p_Left, const SN::SN_Expression & p_Right) const
	{
		return skynet::ReverseSubscript.PrimaryFunctionExpression(p_Left, p_Right);
	}

	SN::SN_Value SNI_Subscript::RightInverseFunctionValue(const SN::SN_Value & p_Left, const SN::SN_Value & p_Right) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_Subscript::RightInverseFunctionExpression(const SN::SN_Expression & p_Left, const SN::SN_Expression & p_Right) const
	{
		return skynet::Null;
	}

	size_t SNI_Subscript::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (!p_ParamList[PU2_First].IsKnownValue())
		{
			return CARDINALITY_MAX;
		}
		return SNI_Binary::CardinalityOfUnify(p_Depth, p_ParamList, p_CalcPos, p_TotalCalc);
	}

	SN::SN_Error SNI_Subscript::UnifyElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet * worldSet, const SNI_Expression *p_Source) const
	{
		LOGGING(SN::LogContext context("SNI_Subscript::UnifyElement(CalcPos " + to_string(p_CalcPos) + " TotalCalc " + to_string(p_TotalCalc) + " " + DisplayValues(p_Depth, p_ParamList, p_WorldList) + ")"));
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
					LOGGING(context.LogText("fail", "Join worlds failed on " + DisplayWorlds(p_Depth, p_WorldList, LDO)));
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
			switch (p_CalcPos)
			{
			case PU2_First:
			{
				SN::SN_Value value = RightInverseFunctionValue(p_ParamList[PU2_Result].GetVariableValue(), p_ParamList[PU2_Second].GetVariableValue());
				return p_ParamList[p_CalcPos].AddValue(value, p_Depth, p_WorldList, worldSet);
			}
			case PU2_Second:
			{
				SN::SN_Value value = LeftInverseFunctionValue(p_ParamList[PU2_Result].GetVariableValue(), p_ParamList[PU2_First].GetVariableValue());
				return p_ParamList[p_CalcPos].AddValue(value, p_Depth, p_WorldList, worldSet);
			}
			case PU2_Result:
			{
				SN::SN_Value value = PrimaryFunctionValue(p_ParamList[PU2_First].GetVariableValue(), p_ParamList[PU2_Second].GetVariableValue());
				return p_ParamList[p_CalcPos].AddValue(value, p_Depth, p_WorldList, worldSet);
			}
			}
		}
		}
		return SN::SN_Error(false, false, GetTypeName() + ": Expression not unified. TotalCalc=" + to_string(p_TotalCalc) + " Calcpos=" + to_string(p_CalcPos));
	}
}
