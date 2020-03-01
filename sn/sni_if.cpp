#include "sni_if.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sn.h"

#include "sni_delayedprocessor.h"
#include "sni_world.h"
#include "sni_worldset.h"
#include "sni_taggedvalue.h"
#include "sni_valueset.h"
#include "sni_bool.h"
#include "sni_error.h"
#include "sni_null.h"
#include "sni_helpers.h"
#include "sni_splitter.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_If::SNI_If()
	{

	}

	SNI_If::~SNI_If()
	{

	}

	/// @cond
	string SNI_If::GetTypeName() const
	{
		return "If";
	}

	string SNI_If::DisplayCpp() const
	{
		return "SN::If";
	}

	string SNI_If::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "if";
	}

	long SNI_If::GetPriority() const
	{
		return 0;
	}

	string SNI_If::GetOperator() const
	{
		return "?";
	}

	long SNI_If::GetNumParameters() const
	{
		return 4;
	}

	string SNI_If::DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, size_t p_NumParams, SN::SN_Expression * p_ParamList, const SNI_Expression *p_DebugSource) const
	{
		return SetBreakPoint("if", p_DisplayOptions, p_DebugSource, SN::LeftId) + " " + p_ParamList[PC3_Condition].DisplaySN(GetPriority(), p_DisplayOptions) + " " +
			   SetBreakPoint("then", p_DisplayOptions, p_DebugSource, SN::PositiveId)+ " " + p_ParamList[PC3_Positive].DisplaySN(GetPriority(), p_DisplayOptions) + " " +
			   SetBreakPoint("else", p_DisplayOptions, p_DebugSource, SN::NegativeId) + " " + p_ParamList[PC3_Negative].DisplaySN(GetPriority(), p_DisplayOptions) + " " +
			   SetBreakPoint("end", p_DisplayOptions, p_DebugSource, SN::RightId);
	}

	/// @endcond

	/// \brief Evaluate if bool then value else value, and return the result.
	///
	/// - Extract the first parameter and evaluate as the condition.
	/// - If meta code
	///   + Extract the second parameter and partial evaluate as the positive case.
	///   + Extract the third parameter and partial evaluate as the negative case.
	///   + Return meta code for the if then else condition.
	/// - Extract the second parameter as the positive case.
	/// - Extract the third parameter as the negative case.
	/// - Call DoIf to evaluate the if then else condition.
	///
	/// @param p_ParameterList List of three parameters (condition, positive case, negative case).
	/// @param p_MetaLevel The meta level; 0 means evaluate the value. > 0 means return the expression to concatenate left and right.
	/// @retval A value, expression (for meta code) or null.
	SN::SN_Expression SNI_If::CallArray(SN::SN_Expression * p_ParamList, long p_MetaLevel /* = 0 */) const
	{
		SNI_Thread::GetThread()->DebugCommand(SN::CallPoint, GetTypeName() + ".Call", SN::LeftId);

		SN::SN_Value condition = p_ParamList[0].DoEvaluate(p_MetaLevel);
		if (condition.IsNull())
		{
			return condition;
		}

		if (0 < p_MetaLevel)
		{
			SN::SN_Expression positiveCase = p_ParamList[1].DoPartialEvaluate(p_MetaLevel);
			SN::SN_Expression negativeCase = p_ParamList[2].DoPartialEvaluate(p_MetaLevel);
			return condition.If(positiveCase, negativeCase);
		}

		SN::SN_Value result = condition.DoIf(p_ParamList[1], p_ParamList[2]);
		result.GetSNI_Expression()->Validate();

		SNI_Thread::GetThread()->DebugCommand(SN::CallPoint, GetTypeName() + ".Unify after all values", SN::RightId);

		return result;
	}

	/// \brief Return an expression or value for if bool then value else value.
	///
	/// Partial evaluation returns an expression if not all values are known.
	///
	/// - Extract first element from list as left and partially evaluate it.
	/// - If not meta code and condtion is a bool.
	///   + If condition is true
	///     * Return the partial evaluation of the positive case
	///   + else
	///     * Return the partial evaluation of the negative case
	/// - Partially evaluate the second parameter as the positive case.
	/// - Partially evaluate the third parameter as the negative case.
	/// - If the positive case is the same as the negative case.
	///   + Return the positive case
	/// - If not meta code
	///   + If the positive case and the negative case are bool.
	///     * They must be different values.
	///       - Return the condition or the negation of the condition as an expression.
	/// - Return the expression for "if condition then positive case else negative case".
	///
	/// @param p_ParameterList List of three parameters (condition, positive case, negative case).
	/// @param p_MetaLevel The meta level; 0 means evaluate the value. > 0 means return the expression to concatenate left and right.
	/// @retval An expression or value.
	SN::SN_Expression SNI_If::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context("SNI_If::PartialCall ( " + DisplayPmExpressionList(p_ParameterList) + " )"));
		
		SN::SN_Expression condition = (*p_ParameterList)[2].DoPartialEvaluate(p_MetaLevel);

		if (0 == p_MetaLevel)
		{
			if (SN::Is<SNI_Bool *>(condition))
			{
				if (SN::SN_Bool(condition).GetBool())
				{
					return LOG_RETURN(context, (*p_ParameterList)[1].DoPartialEvaluate(p_MetaLevel));
				}
				else
				{
					return LOG_RETURN(context, (*p_ParameterList)[0].DoPartialEvaluate(p_MetaLevel));
				}
			}
		}

		SN::SN_Expression positiveCase = (*p_ParameterList)[1].DoPartialEvaluate(p_MetaLevel);
		SN::SN_Expression negativeCase = (*p_ParameterList)[0].DoPartialEvaluate(p_MetaLevel);

		if (positiveCase.Equivalent(negativeCase))
		{
			return LOG_RETURN(context, positiveCase);
		}

		if (0 == p_MetaLevel)
		{
			if (SN::Is<SNI_Bool *>(positiveCase) && SN::Is<SNI_Bool *>(negativeCase))
			{
				if (dynamic_cast<SNI_Bool *>(positiveCase.GetSNI_Expression())->GetBool())
				{
					return LOG_RETURN(context, condition);
				}
				else
				{
					return LOG_RETURN(context, !condition);
				}
			}
		}
		return LOG_RETURN(context, condition.If(positiveCase, negativeCase));
	}

	SN::SN_Expression SNI_If::UnifyArray(SN::SN_Expression * p_ParameterList, const SNI_Expression *p_Source)
	{
		SNI_Frame::Push(this, NULL);
		SNI_Frame *topFrame = SNI_Frame::Top();
		SNI_Variable* result_param = topFrame->CreateParameterByName("result", p_ParameterList[0]);
		SNI_Variable* condition_param = topFrame->CreateParameterByName("condition", p_ParameterList[1]);
		SNI_Variable* positive_param = topFrame->CreateParameterByName("positive", p_ParameterList[2]);
		SNI_Variable* negative_param = topFrame->CreateParameterByName("negative", p_ParameterList[3]);
		result_param->SetValue(p_ParameterList[0]);
		SN::SN_ValueSet condition;
		SNI_WorldSet *condition_worldSet = new SNI_WorldSet();
		condition.SetWorldSet(condition_worldSet);
		condition.AddTaggedValue(skynet::True, condition_worldSet->CreateWorld());
		condition.AddTaggedValue(skynet::False, condition_worldSet->CreateWorld());
		condition_worldSet->Complete();

		condition_param->SetValue(condition);
		SNI_Thread::GetThread()->DebugCommand(SN::CallPoint, GetTypeName() + ".Unify after all values", SN::LeftId);

		SN::SN_Error e1 = p_ParameterList[1].AssertValue(condition);
		if (e1.IsError())
		{
			return e1;
		}

		SN::SN_Expression sCondition = condition.SimplifyValue();

		condition_param->SetValue(sCondition);

		SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
		SN::SN_Expression *parameterList = p_ParameterList;
		bool success = false;
		string typeName = GetTypeName();
		const SNI_Expression *source = p_Source;
		sCondition.ForEach(
			[contextWorld, parameterList, &success, &typeName, source](const SN::SN_Expression &p_Param, SNI::SNI_World *p_World) -> SN::SN_Error
			{
				if (p_World)
				{
					SNI_Thread::GetThread()->PushContextWorld(p_World);
				}
				SNI_Thread::GetThread()->SetDebugId("");
				SN::SN_Error e = skynet::OK;
				if (p_Param.GetBool())
				{
					SN::SN_Expression *paramList = new SN::SN_Expression[2];
					paramList[0] = parameterList[0];
					paramList[1] = parameterList[2];

					SNI_Thread::GetThread()->DebugCommand(SN::CallPoint, typeName + ".Unify after all values", SN::PositiveId);
					e = skynet::Same.GetSNI_FunctionDef()->UnifyArray(paramList, source).GetError();
				}
				else
				{
					SN::SN_Expression *paramList = new SN::SN_Expression[2];
					paramList[0] = parameterList[0];
					paramList[1] = parameterList[3];

					SNI_Thread::GetThread()->DebugCommand(SN::CallPoint, typeName + ".Unify after all values", SN::NegativeId);
					e = skynet::Same.GetSNI_FunctionDef()->UnifyArray(paramList, source).GetError();
				}
				if (p_World)
				{
					SNI_Thread::GetThread()->PopContextWorld();
				}
				if (e.IsError())
				{
					if (p_World)
					{
						SN::SN_Error err = p_World->Fail(IncompatibleValue);
						if (err.IsError())
						{
							return err;
						}
					}
				}
				else
				{
					success = true;
				}
				return skynet::OK;
			}
		);

		SNI_Thread::GetThread()->DebugCommand(SN::CallPoint, typeName + ".Unify after all values", SN::RightId);
		SNI_Frame::Pop();
		if (!success)
		{
			return SN::SN_Error(false, false, "Both the positive and negative cases failed in if statement.");
		}

		SN::SN_Error e = condition_worldSet->CheckDependentWorlds();
		if (e.IsError())
		{
			return e;
		}

		return skynet::OK;
	}

	/// @brief Extract the condition, positive case, and negative case from the parameter list and call PartialUnifyInternal.
	///
	/// @param p_ParameterList List of two parameters to be concatenated.
	/// @param p_Result The result of concatenating the two strings
	/// @retval True if successful.
	SN::SN_Error SNI_If::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		LOGGING(SN::LogContext context("SNI_If::PartialUnify ( " + DisplayPmParameterList(p_ParameterList) + " = " + p_Result.DisplaySN() + " )"));
		return LOG_RETURN(context, PartialUnifyInternal((*p_ParameterList)[2].GetValue(), (*p_ParameterList)[1].GetValue(), (*p_ParameterList)[0].GetValue(), p_Result));
	}

	/// @brief Derive expressions for unknown values from known values in "if p_condition then p_PositiveCase else p_NegativeCase = p_result".
	///
	/// - Partially evaluate the condition.
	/// - If the condition is a boolean value.
	///   + If the condition is true.
	///     * Partially evaluate the positive case.
	///     * If the positive case is a value.
	///       - Return the true/false status of partially asserting the result is equal to the positive case
	///     * If the positive case is not a value.
	///       - Return the true/false status of partially asserting the positive case is equal to the result
	///   + If the condition is false.
	///     * Partially evaluate the negative case.
	///     * If the negative case is a value.
	///       - Return the true/false status of partially asserting the result is equal to the negative case
	///     * If the negative case is not a value.
	///       - Return the true/false status of partially asserting the negative case is equal to the result
	/// - If the condition is not a boolean.
	///   + Partially evaluate the positive case.
	///   + Partially evaluate the negative case. 
	///   + If the positive and negative cases are the same expression.
	///     * Return the true/false status of partially asserting the condition is equal to the result
	///   + Otherwise
	///     * Partially evaluate the result.
	///     * If the positive case is the same expression as the negative case
	///       - Return the true/false status of partially asserting the result.
	///     * Otherwise if the positive and the negative cases are the same expression
	///       - If the positive case is true
	///         + Return the true/false status of partially asserting the condition is equal to the result
	///       - otherwise
	///         + Return the true/false status of partially asserting the condition is equal to the not the result
	///
	/// - HasMember all other case return false.
	///
	/// @param p_Condition The if condition
	/// @param p_PositiveCase The value to return if condition is true.
	/// @param p_NegativeCase The value to return if condition is false.
	/// @param p_Result The result
	/// @retval True if successful.
	SN::SN_Error SNI_If::PartialUnifyInternal(SN::SN_Expression &p_Condition, SN::SN_Expression &p_PositiveCase, SN::SN_Expression &p_NegativeCase, SN::SN_Expression &p_Result)
	{
		SN::SN_Expression condition = p_Condition.DoPartialEvaluate();
		if (SN::Is<SNI_Bool *>(condition))
		{
			if (SN::SN_Bool(condition).GetBool())
			{
				SN::SN_Expression positiveCase = p_PositiveCase.DoPartialEvaluate();
				if (SN::Is<SNI_Bool *>(positiveCase))
				{
					return p_Result.PartialAssertValue(SN::SN_Value(positiveCase));
				}
				else
				{
					SN::SN_Expression result = p_Result.DoPartialEvaluate();
					if (SN::Is<SNI_Bool *>(result))
					{
						return p_PositiveCase.PartialAssertValue(SN::SN_Value(result));
					}
				}
			}
			else
			{
				SN::SN_Expression negativeCase = p_NegativeCase.DoPartialEvaluate();
				if (SN::Is<SNI_Bool *>(negativeCase))
				{
					return p_Result.PartialAssertValue(negativeCase);
				}
				else
				{
					SN::SN_Expression result = p_Result.DoPartialEvaluate();
					if (SN::Is<SNI_Bool *>(result))
					{
						return p_NegativeCase.PartialAssertValue(SN::SN_Value(result));
					}
				}
			}
		}
		else
		{
			SN::SN_Expression positiveCase = p_PositiveCase.DoPartialEvaluate();
			SN::SN_Expression negativeCase = p_NegativeCase.DoPartialEvaluate();
			if (SN::Is<SNI_Bool *>(positiveCase) && SN::Is<SNI_Bool *>(negativeCase))
			{
				if (SN::SN_Bool(positiveCase).GetBool() == SN::SN_Bool(negativeCase).GetBool())
				{
					return p_Result.PartialAssertValue(positiveCase);
				}
				else
				{
					SN::SN_Expression result = p_Result.DoPartialEvaluate();
					if (SN::SN_Bool(positiveCase).GetBool())
					{
						return p_Condition.PartialAssertValue(SN::SN_Expression(result));
					}
					else
					{
						return p_Condition.PartialAssertValue(SN::SN_Expression(result.DoNot()));
					}
				}
			}
		}
		return skynet::Fail;
	}

	size_t SNI_If::CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const
	{
		if (p_Depth >= 2)
		{
			if (!p_ParamList[0].IsKnownValue())
			{
				return CARDINALITY_MAX;
			}
			if (p_ParamList[0].GetBool())
			{
				return MultiplyCardinality(p_ParamList[0].Cardinality(2), p_ParamList[1].Cardinality());
			}
			else if (p_Depth == 3)
			{
				return MultiplyCardinality(p_ParamList[0].Cardinality(2), p_ParamList[2].Cardinality());
 			}
			return p_ParamList[1].Cardinality() + p_ParamList[2].Cardinality();
		}
		return CARDINALITY_MAX;
	}

	SN::SN_Value SNI_If::CallElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, SN::SN_ValueSet p_Result) const
	{
		if (p_WorldList)
		{
			p_Result.AddValue(p_ParamList[0].GetVariableValue().DoIf(p_ParamList[1].GetVariableValue(), p_ParamList[2].GetVariableValue()), p_Depth, p_WorldList, NULL);
			return skynet::OK;
		}
		else
		{
			return p_ParamList[0].GetVariableValue().DoIf(p_ParamList[1].GetVariableValue(), p_ParamList[2].GetVariableValue());
		}
	}
}
