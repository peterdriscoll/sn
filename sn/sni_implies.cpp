#include "sni_implies.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sn.h"
#include "sn_cartesian.h"

#include "sni_delayedprocessor.h"
#include "sni_error.h"
#include "sni_worldset.h"
#include "sni_valueset.h"
#include "sni_taggedvalue.h"
#include "sni_bool.h"
#include "sni_null.h"
#include "sni_helpers.h"
#include "utility.h"

#include <algorithm>

#include "sn_pch.h"

namespace SNI
{
	SNI_Implies::SNI_Implies()
	{

	}

	SNI_Implies::~SNI_Implies()
	{

	}

	/// @cond

	std::string SNI_Implies::GetTypeName() const
	{
		return "Implies";
	}

	std::string SNI_Implies::DisplayCpp() const
	{
		return "skynet::Implies";
	}

	std::string SNI_Implies::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "Implies";
	}

	long SNI_Implies::GetPriority() const
	{
		return 100;
	}
	/// @endcond

	SN::SN_Value SNI_Implies::PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Left.DoImplies(p_Right);
	}

	SN::SN_Value SNI_Implies::LeftInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Left) const
	{
		return p_Left.DoImplies(p_Result);
	}

	SN::SN_Expression SNI_Implies::LeftInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Left) const
	{
		return PrimaryFunctionExpression(p_Left, p_Result);
	}

	SN::SN_Value SNI_Implies::RightInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Right) const
	{
		return skynet::Null; // What are these?
	}

	SN::SN_Expression SNI_Implies::RightInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Right) const
	{
		return skynet::Null; // What are these?
	}

	size_t SNI_Implies::CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const
	{
		if (p_Depth >= 1)
		{
			SN::SN_Bool cond = p_ParamList[PC2_First].GetVariableValue();
			if (!cond.IsNull() && !cond.GetBool())
			{
				return 1;
			}
		}
		return SNI_Binary::CardinalityOfCall(p_Depth, p_ParamList);
	}

	size_t SNI_Implies::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (p_TotalCalc >= 1)
		{
			SN::SN_Bool cond = p_ParamList[PU2_First].GetVariableValue();
			if (!cond.IsNull() && cond.GetBool())
			{
				return 1;
			}
		}
		return SNI_Binary::CardinalityOfUnify(p_Depth, p_ParamList, p_CalcPos, p_TotalCalc);
	}

	SN::SN_Error SNI_Implies::UnifyElement(long p_Depth, SN::SN_Expression *p_ParamList, SNI_World **p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet *worldSet, const SNI_Expression *p_Source) const
	{
		// Note the order is: 0:Left param, 1:Result, 2:right param.
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
					if (PrimaryFunctionValue(p_ParamList[PU2_First], p_ParamList[PU2_Second]).Equivalent(p_ParamList[PU2_Result]))
					{
						world->AddToSetList();
					}
				}
				return skynet::OK;
			}
			else
			{
				return SN::SN_Error(PrimaryFunctionValue(p_ParamList[PU2_First].GetVariableValue(), p_ParamList[PU2_Second].GetVariableValue()).Equivalent(p_ParamList[PU2_Result].GetVariableValue()), false);
			}
		}
		break;
		case 1:
		{
			switch (p_CalcPos)
			{
			case PU2_First:
			{
				return p_ParamList[p_CalcPos].AddValue(RightInverseFunctionValue(p_ParamList[PU2_Result], p_ParamList[PU2_Second]), p_Depth, p_WorldList, worldSet);
			}
			case PU2_Second:
			{
				return p_ParamList[p_CalcPos].AddValue(LeftInverseFunctionValue(p_ParamList[PU2_Result], p_ParamList[PU2_First]), p_Depth, p_WorldList, worldSet);
			}
			case PU2_Result:
			{
				return p_ParamList[p_CalcPos].AddValue(PrimaryFunctionValue(p_ParamList[PU2_First], p_ParamList[PU2_Second]), p_Depth, p_WorldList, worldSet);
			}
			}
		}
		case 2:
		{
			SN::SN_Bool cond = p_ParamList[PU2_First].GetVariableValue();
			if (!cond.IsNull() && !cond.GetBool())
			{
				p_ParamList[PU2_Result].AddValue(skynet::True, p_Depth, p_WorldList, worldSet);
				p_ParamList[PU2_Second].AddValue(skynet::Null, p_Depth, p_WorldList, worldSet);
				return skynet::OK;
			}
			SN::SN_Bool result = p_ParamList[PU2_Result].GetVariableValue();
			if (!result.IsNull() && !result.GetBool())
			{
				p_ParamList[PU2_First].AddValue(skynet::True, p_Depth, p_WorldList, worldSet);
				p_ParamList[PU2_Second].AddValue(skynet::False, p_Depth, p_WorldList, worldSet);
				return skynet::OK;
			}
		}
		}
		return SN::SN_Error(false, false, GetTypeName() + ": Expression not unified. TotalCalc=" + std::to_string(p_TotalCalc) + " Calcpos=" + std::to_string(p_CalcPos));
	}
}