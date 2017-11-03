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

	string SNI_RevAnd::DisplaySN(long /*priority*/, SNI_VariablePointerList & /*p_DisplayVariableList*/) const
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

	/// @brief Extract the left and right values from the parameter list and call UnifyInternal.
	///
	/// @param p_ParameterList List of two parameters to be "anded".
	/// @param p_Result The result of anding the two values.
	/// @retval True if successful, false for failure.
	SN::SN_Error SNI_RevAnd::Unify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result)
	{
		SN::LogContext context("SNI_RevAnd::Unify ( " + DisplayPmParameterList(p_ParameterList) + " = " + p_Result.DisplaySN() + " )");

		// Left param. Result, right param.
		// * Left is true. The result is true. Right is undefined.
		// * Result is the most likely to have a value.
		SN::SN_Error e = ((*p_ParameterList)[1].GetValue().CartProd(PU2_First) * p_Result.CartProd(PU2_Result) * (*p_ParameterList)[0].GetValue().CartProd(PU2_Second)).ForEachUnify(this);
		if (e.GetDelay())
		{
			SNI_DelayedProcessor::GetProcessor()->Delay(SN::SN_FunctionDef(dynamic_cast<SNI_FunctionDef*>(this)), p_ParameterList, p_Result);
		}
		if (e.IsError())
		{
			e.AddNote(context, this, "Cartesian product failed");
		}
		return e;
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

	SN::SN_Error SNI_RevAnd::CallElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, SN::SN_ValueSet p_Result) const
	{
		p_Result.AddValue(PrimaryFunctionValue(p_ParamList[PC2_First], p_ParamList[PC2_Second]), p_Depth, p_WorldList, NULL);
		return SN::SN_Error(true);
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
			bool exists = false;
			SNI_World *world = worldSet->JoinWorldsArray(ManualAddWorld, AlwaysCreateWorld, exists, p_Depth, p_WorldList);
			if (exists)
			{
				if (PrimaryFunctionValue(p_ParamList[PU2_First], p_ParamList[PU2_Second]).Equivalent(p_ParamList[PU2_Result]))
				{
					world->AddToSetList();
				}
			}
			return true;
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