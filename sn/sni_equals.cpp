#include "sni_equals.h"

#include "logcontext.h"
#include "sn.h"
#include "sn_valueset.h"
#include "sn_bool.h"
#include "sn_cartesian.h"
#include "sn_error.h"

#include "sni_delayedprocessor.h"
#include "sni_delayedcall.h"
#include "sni_bool.h"
#include "sni_world.h"
#include "sni_worldset.h"
#include "sni_taggedvalue.h"
#include "sni_valueset.h"
#include "sni_stringref.h"
#include "sni_vardef.h"
#include "sni_error.h"
#include "sni_null.h"
#include "sni_helpers.h"
#include "utility.h"

#include "sni_valueset.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Equals::SNI_Equals()
	{

	}

	SNI_Equals::~SNI_Equals()
	{

	}

	/// @cond
	void SNI_Equals::PromoteMembers()
	{
	}

	string SNI_Equals::GetTypeName() const
	{
		return "Equals";
	}

	string SNI_Equals::DisplayCpp() const
	{
		return "SN::Equals";
	}

	string SNI_Equals::DisplaySN(long /*priority*/, SNI_VariablePointerList & /*p_DisplayVariableList*/) const
	{
		return "=";
	}

	long SNI_Equals::GetPriority() const
	{
		return 0;
	}

	string SNI_Equals::GetOperator() const
	{
		return "=";
	}
	/// @endcond

	SN::SN_Value SNI_Equals::PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Left.GetSNI_Value()->DoEquals(p_Right.GetSNI_Value());
	}

	SN::SN_Expression SNI_Equals::PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return p_Left == p_Right;
	}

	SN::SN_Value SNI_Equals::LeftInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Left) const
	{
		if (p_Result.GetBool())
		{
			return p_Left;
		}
		return skynet::Null;
	}

	SN::SN_Value SNI_Equals::RightInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Right) const
	{
		if (p_Result.GetBool())
		{
			return p_Right;
		}
		return skynet::Null;
	}

	SN::SN_Expression SNI_Equals::LeftInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Left) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_Equals::RightInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Right) const
	{
		return skynet::Null;
	}

	/// \brief Return an expression or a value for the result of comparing two values.
	///
	/// Partial evaluation returns an expression if not all values are known.
	///
	/// - Extract first element from list as left and evaluate it.
	/// - Extract second element from list as right and evaluate it.
	/// - If meta level is 0
	///   + If left and right are values
	//      * Return true if the values are the same false otherwise.
	//    + If left and right are the same expression.
	//      * Return true
	/// - Return the expression for left = right.
	///
	/// @param p_ParameterList List of two parameters to be compared.
	/// @param p_MetaLevel The meta level; 0 means evaluate as data. > 0 means return the expression for left = right.
	/// @retval An expression or value for the equality of two values
	SN::SN_Expression SNI_Equals::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		SN::LogContext context("SNI_Equals::PartialCall ( " + DisplayPmExpressionList(p_ParameterList) + " )");

		SN::SN_Expression left_value = (*p_ParameterList)[1].PartialEvaluate(p_MetaLevel);
		SN::SN_Expression right_value = (*p_ParameterList)[0].PartialEvaluate(p_MetaLevel);

		if (0 == p_MetaLevel)
		{
			if (SN::Is<SNI_Value *>(left_value) && SN::Is<SNI_Value *>(right_value))
			{
				return LOG_RETURN(context, left_value.GetSNI_Expression()->DoEquals(right_value.GetSNI_Value()));
			}
			if (left_value.Equivalent(right_value))
			{
				return LOG_RETURN(context, skynet::True);
			}
		}

		return LOG_RETURN(context, left_value == right_value);
	}

	/// \brief Extract the left and right values from the parameter list and call PartialUnifyInternal.
	///
	/// @param p_ParameterList List of two parameters to be compared.
	/// @param p_result The result of comparing the two values
	/// @retval True for success,false for failure.
	SN::SN_Error SNI_Equals::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_result)
	{
		SN::LogContext context("SNI_Equals::PartialUnify ( " + DisplayPmParameterList(p_ParameterList) + " = " + p_result.DisplaySN() + " )");

		return PartialUnifyInternal((*p_ParameterList)[1].GetValue(), (*p_ParameterList)[0].GetValue(), p_result);
	}

	/// \brief Calculate expressions for unknown values from known values, in the expression (p_left = p_right) = p_result.
	///
	/// Partial calculate gives expressions for unknown values from known values.
	///
	/// - Partially evaluate the left and the right.
	/// - If the left and the right are values
	///   + Return the status of partially asserting the result is the value of left = right
	/// - If left and right are the same expression.
	///   + Return the status of partially asserting the result is true
	/// - Partially evaluate the result.
	/// - If the result is a boolean.
	///   + If result is true
	///     * If left is a function call or a variable.
	///       - Partially assert the left is right.
	///     * If right is a function call or a variable.
	///       - Partially assert the rght is left
	///     * If either of the above partial asserts is successful return success..
	/// - Return failure (false)
	///
	/// @param p_left value before =
	/// @param p_right value after =
	/// @param p_result the result of the comparison
	/// @retval True if successful, false for failure.
	SN::SN_Error SNI_Equals::PartialUnifyInternal(SN::SN_Expression &p_left, SN::SN_Expression &p_right, SN::SN_Expression &p_result)
	{
		SN::SN_Expression left_value = p_left.PartialEvaluate();
		SN::SN_Expression right_value = p_right.PartialEvaluate();

		SN::LogContext context2("SNI_Equals::PartialUnifyInternal 2 ( " + left_value.DisplaySN() + ", " + right_value.DisplaySN() + " )");

		if (SN::Is<SNI_Value *>(left_value) && SN::Is<SNI_Value *>(right_value))
		{
			return SNI_Helpers::PartialAssertValue(p_result, left_value.GetSNI_Value()->DoEquals(right_value.GetSNI_Value()));
		}
		if (left_value.Equivalent(right_value))
		{
			return SNI_Helpers::PartialAssertValue(p_result, skynet::True);
		}
		SN::SN_Expression result = p_result.PartialEvaluate();
		if (SN::Is<SNI_Bool *>(result))
		{
			bool ok = false;
			if (SN::SN_Bool(result).GetBool())
			{
				if (SN::Is<SNI_VarDef *>(left_value))
				{
					ok |= SNI_Helpers::PartialAssertValue(left_value, right_value).GetBool();
				}
				else if (!ok && SN::Is<SNI_VarDef *>(right_value))
				{
					ok = SNI_Helpers::PartialAssertValue(right_value, left_value).GetBool();
				}
			}
			return ok;
		}
		return false;
	}

	size_t SNI_Equals::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (p_TotalCalc <= 2)
		{
			long totalCalc = p_TotalCalc;
			long calcPos = p_CalcPos;
			if (p_ParamList[PU2_Result].GetBool())
			{
				if (p_ParamList[PU2_Second].IsReferableValue() && p_ParamList[PU2_Second].IsNullValue())
				{
					totalCalc--;
					calcPos = PU2_First;
				}
				else if (p_ParamList[PU2_First].IsReferableValue() && p_ParamList[PU2_First].IsNullValue())
				{
					totalCalc--;
					calcPos = PU2_Second;
				}
				if (totalCalc > 1)
				{
					return CARDINALITY_MAX;
				}
			}
			else if (!p_ParamList[PU2_Result].IsKnownValue())
			{
				if (p_TotalCalc > 1)
				{
					return CARDINALITY_MAX;
				}
			}
			else
			{
				if (p_TotalCalc > 0)
				{
					return CARDINALITY_MAX;
				}
			}
			return SNI_Binary::CardinalityOfUnify(p_Depth, p_ParamList, calcPos, totalCalc);
		}
		return CARDINALITY_MAX;
	}

	SN::SN_Error SNI_Equals::UnifyElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet * worldSet) const
	{
		SN::LogContext context("SNI_Equals::UnifyElement(CalcPos " + to_string(p_CalcPos) + " TotalCalc " + to_string(p_TotalCalc) + " " + DisplayValues(p_Depth, p_ParamList, p_WorldList) + ")");
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
					if (PrimaryFunctionValue(p_ParamList[PU2_First].GetVariableValue(), p_ParamList[PU2_Second].GetVariableValue()).Equivalent(p_ParamList[PU2_Result].GetVariableValue()))
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
				SN::SN_Error e;
				if (p_ParamList[PU2_Result].GetBool())
				{
					SN::SN_Value value = RightInverseFunctionValue(p_ParamList[PU2_Result].GetVariableValue(), p_ParamList[PU2_Second].GetVariableValue());
					e = p_ParamList[p_CalcPos].AddValue(value, p_Depth, p_WorldList, worldSet);
				}
				else
				{
					SN::SN_Variable varLeft;
					e = p_ParamList[p_CalcPos].AddValue(varLeft, p_Depth, p_WorldList, worldSet);

					SN::SN_ExpressionList *l_ParameterList = new SN::SN_ExpressionList();
					l_ParameterList->push_back(p_ParamList[PU2_Result]);
					l_ParameterList->push_back(varLeft);
					l_ParameterList->push_back(p_ParamList[PU2_Second]);
					SNI_DelayedProcessor::GetProcessor()->Delay(this, l_ParameterList);
				}
				return e;
			}
			case PU2_Second:
			{
				SN::SN_Error e;
				if (p_ParamList[PU2_Result].GetBool())
				{
					SN::SN_Value value = LeftInverseFunctionValue(p_ParamList[PU2_Result].GetVariableValue(), p_ParamList[PU2_First].GetVariableValue());
					e = p_ParamList[p_CalcPos].AddValue(value, p_Depth, p_WorldList, worldSet);
				}
				else
				{
					SN::SN_Variable varRight;
					e = p_ParamList[p_CalcPos].AddValue(varRight, p_Depth, p_WorldList, worldSet);

					SN::SN_ExpressionList *l_ParameterList = new SN::SN_ExpressionList();
					l_ParameterList->push_back(varRight);
					l_ParameterList->push_back(p_ParamList[PU2_Result]);
					l_ParameterList->push_back(p_ParamList[PU2_First]);
					SNI_DelayedProcessor::GetProcessor()->Delay(this, l_ParameterList);
				}
				return e;
			}
			case PU2_Result:
			{
				SN::SN_Value value = PrimaryFunctionValue(p_ParamList[PU2_First].GetVariableValue(), p_ParamList[PU2_Second].GetVariableValue());
				return p_ParamList[p_CalcPos].AddValue(value, p_Depth, p_WorldList, worldSet);
			}
			}
		}
		case 2:
		{
			if (p_ParamList[PU2_Result].GetBool())
			{
				if (p_ParamList[PU2_Second].IsReferableValue() && p_ParamList[PU2_Second].IsNullValue())
				{
					return p_ParamList[PU2_First].AddValue(RightInverseFunctionValue(p_ParamList[PU2_Result].GetVariableValue(), p_ParamList[PU2_Second].GetVariableValue()), p_Depth, p_WorldList, worldSet);
				}
				else if (p_ParamList[PU2_First].IsReferableValue() && p_ParamList[PU2_First].IsNullValue())
				{
					return p_ParamList[PU2_Second].AddValue(LeftInverseFunctionValue(p_ParamList[PU2_Result].GetVariableValue(), p_ParamList[PU2_First].GetVariableValue()), p_Depth, p_WorldList, worldSet);
				}
			}
		}
		}
		return SN::SN_Error("SNI_Equals::UnifyElement: Nothing to unify.");
	}
}
