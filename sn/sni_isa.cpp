#include "sni_isa.h"

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
	SNI_IsA::SNI_IsA()
	{
	}

	SNI_IsA::~SNI_IsA()
	{
	}

	string SNI_IsA::GetTypeName() const
	{
		return "IsA";
	}

	string SNI_IsA::DisplayCpp() const
	{
		return "SN::IsA";
	}

	string SNI_IsA::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "IsA";
	}

	long SNI_IsA::GetPriority() const
	{
		return 0;
	}

	bool SNI_IsA::IsKnownValue(const SN::SN_Expression &p_Param) const
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

	SN::SN_Expression SNI_IsA::PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return p_Left.IsA(p_Right);
	}

	size_t SNI_IsA::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (1 < p_ParamList[PU2_First].Cardinality())
		{
			return CARDINALITY_MAX;
		}
		if (1 < p_ParamList[PU2_Second].Cardinality())
		{
			return CARDINALITY_MAX;
		}
		if (p_TotalCalc <= 1)
		{
			return 1;
		}
		else if (p_TotalCalc <= 1)
		{
			if (p_CalcPos == PU2_Result)
			{
				return 1;
			}
		}
		return CARDINALITY_MAX;
	}

	SN::SN_Error SNI_IsA::UnifyElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet * worldSet) const
	{
		SN::LogContext context("SNI_IsA::UnifyElement(CalcPos " + to_string(p_CalcPos) + " TotalCalc " + to_string(p_TotalCalc) + " " + DisplayValues(p_Depth, p_ParamList, p_WorldList) + ")");
		if (worldSet)
		{
			context.LogText("World set", worldSet->DisplayLong());
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
						context.LogText("fail", "Value conflict on " + DisplayValues(p_Depth, p_ParamList, p_WorldList));
					}
				}
				else
				{
					context.LogText("fail", "Join worlds failed on " + DisplayWorlds(p_Depth, p_WorldList));
				}
				return true;
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
		return false;
	}

	size_t SNI_IsA::CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const
	{
		if (1 < p_ParamList[PC2_First].Cardinality())
		{
			return CARDINALITY_MAX;
		}
		if (1 < p_ParamList[PC2_Second].Cardinality())
		{
			return CARDINALITY_MAX;
		}
		return 1;
	}
}
