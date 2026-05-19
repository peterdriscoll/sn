#include "sni_hasmember.h"

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
	SNI_HasMember::SNI_HasMember()
	{
	}

	SNI_HasMember::~SNI_HasMember()
	{
	}

	std::string SNI_HasMember::GetTypeName() const
	{
		return "HasMember";
	}

	std::string SNI_HasMember::DisplayCpp() const
	{
		return "SN::HasMember";
	}

	std::string SNI_HasMember::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "in";
	}

	long SNI_HasMember::GetPriority() const
	{
		return 0;
	}

	SN::SN_Value SNI_HasMember::PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Left.GetSNI_Value()->DoHasMember(p_Right.GetSNI_Value());
	}

	SN::SN_Value SNI_HasMember::LeftInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Left) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_HasMember::LeftInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return skynet::Null;
	}

	SN::SN_Error SNI_HasMember::AssertValue(const SN::SN_Expression & p_Left, const SN::SN_Expression & p_Right, SN::SN_Expression & p_Result) const
	{
		return p_Left.GetSNI_Expression()->AssertHasMemberValue(p_Right.GetSNI_Expression(), p_Result);
	}

	SN::SN_Error SNI_HasMember::UnifyElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet * worldSet, const SNI_Expression *p_Source) const
	{
		LOGGING(SN::LogContext context("SNI_HasMember::UnifyElement(CalcPos " + std::to_string(p_CalcPos) + " TotalCalc " + std::to_string(p_TotalCalc) + " " + DisplayValues(p_Depth, p_ParamList, p_WorldList) + ")"));
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

	size_t SNI_HasMember::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (p_TotalCalc <= 1)
		{
			if (p_TotalCalc == 0)
			{
				return SNI_Binary::CardinalityOfUnify(p_Depth, p_ParamList, p_CalcPos, p_TotalCalc);
			}
			else if (p_CalcPos == PU2_Result)
			{
				return MultiplyCardinality(p_ParamList[PU2_First].Cardinality(), p_ParamList[PU2_Second].Cardinality());
			}
			else if (p_CalcPos == PU2_Second)
			{
				return MultiplyCardinality(p_ParamList[PU2_Result].Cardinality(), p_ParamList[PU2_First].Cardinality());
			}
		}
		return CARDINALITY_MAX;
	}
}
