#include "sni_isa.h"

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
	SNI_IsA::SNI_IsA()
	{
	}

	SNI_IsA::~SNI_IsA()
	{
	}

	std::string SNI_IsA::GetTypeName() const
	{
		return "IsA";
	}

	std::string SNI_IsA::DisplayCpp() const
	{
		return "SN::IsA";
	}

	std::string SNI_IsA::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "IsA";
	}

	long SNI_IsA::GetPriority() const
	{
		return 0;
	}

	std::string SNI_IsA::GetOperator() const
	{
		return " is a ";
	}

	bool SNI_IsA::IsKnownValue(const SN::SN_Expression &p_Param, long) const
	{
		return p_Param.IsKnownTypeValue();
	}

	SN::SN_Error SNI_IsA::AssertValue(const SN::SN_Value & p_Left, const SN::SN_Value & p_Right, SN::SN_Expression & p_Result) const
	{
		return p_Left.GetSNI_Value()->AssertIsAValue(p_Right.GetSNI_Value(), p_Result);
	}

	SN::SN_Value SNI_IsA::PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Left.GetSNI_Value()->DoIsA(p_Right.GetSNI_Value());
	}

	size_t SNI_IsA::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		size_t card = 1;
		if (p_Depth < 2)
		{
			return CARDINALITY_MAX;
		}
		if (p_ParamList[PU2_First].IsKnownValue())
		{
			card = p_ParamList[PU2_First].Cardinality();
		}
		else if (p_ParamList[PU2_First].IsKnownTypeValue())
		{
			card = 1;
		}
		else
		{
			return CARDINALITY_MAX;
		}
		if (p_ParamList[PU2_Second].IsKnownValue())
		{
			card = MultiplyCardinality(card, p_ParamList[PU2_Second].Cardinality());
		}
		else if (!p_ParamList[PU2_Second].IsKnownTypeValue())
		{
			return CARDINALITY_MAX;
		}
		if (p_Depth >= 3)
		{
			if (p_ParamList[PU2_Result].IsKnownValue())
			{
				card = MultiplyCardinality(card, p_ParamList[PU2_Result].Cardinality());
			}
		}
		return card;
	}

	SN::SN_Error SNI_IsA::UnifyElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet * worldSet, const SNI_Expression *p_Source) const
	{
		LOGGING(SN::LogContext context("SNI_IsA::UnifyElement(CalcPos " + std::to_string(p_CalcPos) + " TotalCalc " + std::to_string(p_TotalCalc) + " " + DisplayValues(p_Depth, p_ParamList, p_WorldList) + ")"));
		if (worldSet)
		{
			LOGGING(context.LogText("World set", worldSet->DisplayLong()));
		}
		if (p_ParamList[PU2_Result].IsKnownValue())
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
		else
		{
			return AssertValue(p_ParamList[PU2_First].GetVariableValue(), p_ParamList[PU2_Second].GetVariableValue(), p_ParamList[PU2_Result]);
		}
		return SN::SN_Error(false, false, GetTypeName() + ": Expression not unified. TotalCalc=" + std::to_string(p_TotalCalc) + " Calcpos=" + std::to_string(p_CalcPos));
	}

	size_t SNI_IsA::CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const
	{
		size_t card = 1;
		if (p_Depth < 2)
		{
			return CARDINALITY_MAX;
		}
		if (p_ParamList[PC2_First].IsKnownValue())
		{
			card = p_ParamList[PC2_First].Cardinality();
		}
		else if (p_ParamList[PC2_First].IsKnownTypeValue())
		{
			card = 1;
		}
		else
		{
			return CARDINALITY_MAX;
		}
		if (p_ParamList[PC2_Second].IsKnownValue())
		{
			card = MultiplyCardinality(card, p_ParamList[PC2_Second].Cardinality());
		}
		else if (!p_ParamList[PC2_Second].IsKnownTypeValue())
		{
			return CARDINALITY_MAX;
		}
		return card;
	}
}
