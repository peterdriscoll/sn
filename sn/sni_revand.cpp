#include "sni_revand.h"

#include "logcontext.h"
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
using namespace std;

#include "sn_pch.h"

namespace SNI
{
	SNI_RevAnd::SNI_RevAnd()
	{

	}

	SNI_RevAnd::~SNI_RevAnd()
	{

	}

	/// @cond
	string SNI_RevAnd::GetTypeName() const
	{
		return "RevAnd";
	}

	string SNI_RevAnd::DisplayCpp() const
	{
		return "skynet::RevAnd";
	}

	string SNI_RevAnd::DisplaySN(long /*priority*/, SNI_VariableConstPointerList & /*p_DisplayVariableList*/) const
	{
		return "RevAnd";
	}

	long SNI_RevAnd::GetPriority() const
	{
		return 100;
	}

	/// @endcond

	SN::SN_Value SNI_RevAnd::PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Left.DoRevAnd(p_Right);
	}

	SN::SN_Expression SNI_RevAnd::PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return p_Left.RevAnd(p_Right);
	}

	SN::SN_Value SNI_RevAnd::LeftInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Left) const
	{
		return p_Left.DoRevAnd(p_Result);
	}

	SN::SN_Expression SNI_RevAnd::LeftInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Left) const
	{
		return p_Left.RevAnd(p_Result);
	}

	SN::SN_Value SNI_RevAnd::RightInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Right) const
	{
		return p_Right.DoAnd(p_Result);
	}

	SN::SN_Expression SNI_RevAnd::RightInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Right) const
	{
		return p_Right && p_Result;
	}

	size_t SNI_RevAnd::CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const
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

	SN::SN_Value SNI_RevAnd::CallElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, SN::SN_ValueSet p_Result) const
	{
		if (p_WorldList)
		{
			p_Result.AddValue(PrimaryFunctionValue(p_ParamList[PC2_First].GetVariableValue(), p_ParamList[PC2_Second].GetVariableValue()), p_Depth, p_WorldList, NULL);
			return skynet::OK;
		}
		else
		{
			return PrimaryFunctionValue(p_ParamList[PC2_First].GetVariableValue(), p_ParamList[PC2_Second].GetVariableValue());
		}
	}

	size_t SNI_RevAnd::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (p_TotalCalc >= 1)
		{
			SN::SN_Bool cond = p_ParamList[PU2_First].GetVariableValue();
			if (!cond.IsNull() && !cond.GetBool())
			{
				return 1;
			}
		}
		return SNI_Binary::CardinalityOfUnify(p_Depth, p_ParamList, p_CalcPos, p_TotalCalc);
	}

	SN::SN_Error SNI_RevAnd::UnifyElement(long p_Depth, SN::SN_Expression *p_ParamList, SNI_World **p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet *worldSet) const
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
					if (PrimaryFunctionValue(p_ParamList[PU2_First].GetVariableValue(), p_ParamList[PU2_Second].GetVariableValue()).Equivalent(p_ParamList[PU2_Result].GetVariableValue()))
					{
						world->AddToSetList();
					}
				}
				return true;
			}
			else
			{
				return PrimaryFunctionValue(p_ParamList[PU2_First].GetVariableValue(), p_ParamList[PU2_Second].GetVariableValue()).Equivalent(p_ParamList[PU2_Result].GetVariableValue());
			}
		}
		break;
		case 1:
		{
			switch (p_CalcPos)
			{
			case PU2_First:
			{
				return p_ParamList[p_CalcPos].AddValue(RightInverseFunctionValue(p_ParamList[PU2_Result].GetVariableValue(), p_ParamList[PU2_Second].GetVariableValue()), p_Depth, p_WorldList, worldSet);
			}
			case PU2_Second:
			{
				return p_ParamList[p_CalcPos].AddValue(LeftInverseFunctionValue(p_ParamList[PU2_Result].GetVariableValue(), p_ParamList[PU2_First].GetVariableValue()), p_Depth, p_WorldList, worldSet);
			}
			case PU2_Result:
			{
				return p_ParamList[p_CalcPos].AddValue(PrimaryFunctionValue(p_ParamList[PU2_First].GetVariableValue(), p_ParamList[PU2_Second].GetVariableValue()), p_Depth, p_WorldList, worldSet);
			}
			}
		}
		case 2:
		{
			SN::SN_Bool first = p_ParamList[PU2_First].GetVariableValue();
			if (!first.IsNull() && first.GetBool())
			{
				p_ParamList[PU2_Result].AddValue(skynet::True, p_Depth, p_WorldList, worldSet);
				p_ParamList[PU2_Second].AddValue(skynet::Null, p_Depth, p_WorldList, worldSet);
				return true;
			}
			SN::SN_Bool second = p_ParamList[PU2_Second].GetVariableValue();
			if (!second.IsNull() && second.GetBool())
			{
				p_ParamList[PU2_First].AddValue(skynet::True, p_Depth, p_WorldList, worldSet);
				p_ParamList[PU2_Result].AddValue(skynet::True, p_Depth, p_WorldList, worldSet);
				return true;
			}
		}
		}
		return false;
	}
}