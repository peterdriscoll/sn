#include "sni_and.h"

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
#include "sni_unaryand.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_And::SNI_And()
	{

	}

	SNI_And::~SNI_And()
	{

	}

	string SNI_And::GetTypeName() const
	{
		return "And";
	}

	string SNI_And::DisplayCpp() const
	{
		return "SN::And";
	}

	string SNI_And::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "(&)";
	}

	long SNI_And::GetPriority() const
	{
		return 2;
	}

	string SNI_And::GetOperator() const
	{
		return "&";
	}

	SN::SN_Value SNI_And::PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Left.DoAnd(p_Right);
	}

	SN::SN_Expression SNI_And::PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return p_Left && p_Right;
	}

	SN::SN_Value SNI_And::LeftInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Left) const
	{
		return p_Result.DoRevAnd(p_Left);
	}

	SN::SN_Expression SNI_And::LeftInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Left) const
	{
		return p_Result.RevAnd(p_Left);
	}

	SN::SN_Value SNI_And::RightInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Right) const
	{
		return p_Result.DoRevAnd(p_Right);
	}

	SN::SN_Expression SNI_And::RightInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Right) const
	{
		return p_Result.RevAnd(p_Right);
	}

	/// \brief Partially evaluate the Boolean conjunction of two booleans, If both are true return true. If either are false return false.
	///
	/// Partial evaluation returns an expression if not all values are known.
	///
	/// - Extract first element from list as left and evaluate it.
	///   + If it is a value,
	///     * If it is not a boolean,
	///       - Return an error
	///     * If false return false 
	/// - Extract second element from list as right and evaluate it.
	///   + If it is a value,
	///     * If it is not a boolean,
	///       - Return an error
	///     * If false return false 
	/// - If meta-level is 0
	///   + If left and right are bool.
	//      * Return the conjunction of left and right.
	///   + If left is a bool.
	///     * left is true
	///       - return right
	///   + If right is a bool.
	///     * right is true
	///       - return left.
	/// - Return the expression for left && right.
	///
	///
	/// @param p_ParameterList List of two parameters to be "anded".
	/// @param p_MetaLevel The meta level; 0 means evaluate the value. > 0 means return the expression for the conjunction of left and right.
	/// @retval An expression or value  for the conjunction of the two values or error.
	SN::SN_Expression SNI_And::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context("SNI_And::PartialCall ( " + DisplayPmExpressionList(p_ParameterList) + " )"));

		SN::SN_Expression left_value = (*p_ParameterList)[1].DoPartialEvaluate(p_MetaLevel);
		if (SN::Is<SNI_Value *>(left_value))
		{
			if (!SN::Is<SNI_Bool *>(left_value))
			{
				return LOG_RETURN(context, new SNI_Error("Invalid type for: " + DisplayPmExpression(left_value)));
			}
			if (!left_value.GetBool())
			{
				return left_value;
			}
		}

		SN::SN_Expression right_value = (*p_ParameterList)[0].DoPartialEvaluate(p_MetaLevel);
		if (SN::Is<SNI_Value *>(right_value))
		{
			if (!SN::Is<SNI_Bool *>(right_value))
			{
				return LOG_RETURN(context, new SNI_Error("Invalid type for: " + DisplayPmExpression(right_value)));
			}
			if (!right_value.GetBool())
			{
				return right_value;
			}
		}

		if (0 == p_MetaLevel)
		{
			if (SN::Is<SNI_Bool *>(left_value) && SN::Is<SNI_Bool *>(right_value))
			{
				return LOG_RETURN(context, left_value.DoAnd(right_value));
			}
			if (SN::Is<SNI_Bool *>(left_value))
			{
				if (SN::SN_Bool(left_value).GetBool())
				{
					return LOG_RETURN(context, right_value);
				}
			}
			if (SN::Is<SNI_Bool *>(right_value))
			{
				if (SN::SN_Bool(right_value).GetBool())
				{
					return LOG_RETURN(context, left_value);
				}
			}
		}
		return LOG_RETURN(context, left_value && right_value);
	}

	/// @brief Extract the left and right values from the parameter list and call UnifyInternal.
	///
	/// @param p_ParameterList List of two parameters to be "anded".
	/// @param p_Result The result of anding the two values.
	/// @retval True if successful, false for failure.
	SN::SN_Expression SNI_And::UnifyArray(SN::SN_Expression * p_ParameterList)
	{
		SNI_Frame::Push(this, NULL);
		SNI_Frame *topFrame = SNI_Frame::Top();
		SNI_Variable* result_param = topFrame->CreateParameterByName("result", p_ParameterList[0]);
		SNI_Variable* left_param = topFrame->CreateParameterByName("left", p_ParameterList[1]);
		SNI_Variable* right_param = topFrame->CreateParameterByName("right", p_ParameterList[2]);
		SNI_Thread::GetThread()->DebugCommand(SN::CallPoint, GetTypeName() + ".Unify before calculation", SN::LeftId);

		SN::SN_Expression * firstParamList = new SN::SN_Expression[2];
		firstParamList[0] = p_ParameterList[0];
		firstParamList[1] = p_ParameterList[1];
		SN::SN_Error e1 = skynet::UnaryAnd.GetSNI_FunctionDef()->UnifyArray(firstParamList);

		left_param->SetValue(p_ParameterList[1]);

		if (e1.IsError())
		{
			SNI_Frame::Pop();
			if (e1.IsError())
			{
				SNI_CallRecord *callRecord = new SNI_CallRecord("And of first parameter.", this);
				e1.GetSNI_Error()->AddNote(callRecord);
			}
			return e1;
		}

		SNI_Thread::GetThread()->DebugCommand(SN::CallPoint, GetTypeName() + ".Unify after calculation", SN::ParameterTwoId);
		SNI_Frame::Pop();

		SN::SN_Expression * secondParamList = new SN::SN_Expression[3];
		secondParamList[0] = p_ParameterList[0];
		secondParamList[1] = firstParamList[1];
		secondParamList[2] = p_ParameterList[2];
		SN::SN_Error e2 = SNI_Binary::UnifyArray(secondParamList);
		if (e2.IsError())
		{
			SNI_CallRecord *callRecord = new SNI_CallRecord("And of second parameter with first result.", this);
			e2.GetSNI_Error()->AddNote(callRecord);
		}

		return e2;
	}

	/// \brief Extract the left and right values from the parameter list and call PartialUnifyInternal.
	///
	///
	/// @param p_ParameterList List of two parameters to be "anded".
	/// @param p_Result The result of anding the two booleans
	/// @retval True if successful, false for failure.
	SN::SN_Error SNI_And::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		LOGGING(SN::LogContext context("SNI_And::PartialUnify ( " + DisplayPmParameterList(p_ParameterList) + " = " + p_Result.DisplaySN() + " )"));

		return LOG_RETURN(context, PartialUnifyInternal((*p_ParameterList)[1].GetValue(), (*p_ParameterList)[0].GetValue(), p_Result));
	}

	/// \brief Calculate expressions for unknown values from known values, in the expression p_left and p_right = p_result..
	///
	/// Partial calculate gives expressions for unknown values from known values.
	///
	/// - Partially evaluate left.
	/// - If left is a boolean.
	///   + If left is false
	///     * Assert result = false.
	/// - Partially evaluate right.
	/// - If left and right are a boolean.
	///   + Assert result = right
	/// - Partially evaluate result.
	/// ???
	/// - If result is a boolean..
	///   + If result is true.
	///     * Assert left and right true.
	///   + If result is false;
	///     * If left is true.
	///       - Assert right is false..
	///     * If right is true.
	///       - Assert left is false..
	/// - Return failure (false).
	///
	/// @param p_left Left parameter to the conjunction.
	/// @param p_right Right parameter to the conjunction.
	/// @param p_result the result of the conjunction.
	/// @retval True if successful, false if failure.
	SN::SN_Error SNI_And::PartialUnifyInternal(SN::SN_Expression &p_left, SN::SN_Expression &p_right, SN::SN_Expression &p_Result)
	{
		LOGGING(SN::LogContext context("SNI_And::PartialUnifyInternal"));

		SN::SN_Expression left_value = p_left.DoPartialEvaluate();
		SN::SN_Expression right_value = p_right.DoPartialEvaluate();
		if (SN::Is<SNI_Value *>(left_value) && SN::Is<SNI_Value *>(right_value))
		{
			SN::SN_Error e = p_Result.PartialAssertValue(SN::SN_Value(left_value).DoAnd(SN::SN_Value(right_value)));
			if (e.IsError())
			{
				SNI_CallRecord *callRecord = new SNI_CallRecord("Partial assert result to conjunction.", this);
				LOGGING(callRecord->SetLogContext(context));
				e.GetSNI_Error()->AddNote(callRecord);
			}
			return e;
		}
		SN::SN_Expression result = p_Result.DoPartialEvaluate();
		if (SN::Is<SNI_Value *>(result))
		{
			if (SN::SN_Bool(result).GetBool())
			{
				SN::SN_Error e1 = p_left.PartialAssertValue(SN::SN_Bool(true));
				if (e1.IsError())
				{
					SNI_CallRecord *callRecord = new SNI_CallRecord("Partial assert left condition true", this);
					LOGGING(callRecord->SetLogContext(context));
					e1.GetSNI_Error()->AddNote(callRecord);
				}
				SN::SN_Error e2 = p_right.PartialAssertValue(SN::SN_Bool(true));
				if (e2.IsError())
				{
					SNI_CallRecord *callRecord = new SNI_CallRecord("Partial assert right condition true", this);
					LOGGING(callRecord->SetLogContext(context));
					e2.GetSNI_Error()->AddNote(callRecord);
				}
				return e2;
			}
			else
			{
				if (SN::Is<SNI_Value *>(left_value) && SN::SN_Bool(left_value).GetBool())
				{
					SN::SN_Error e = p_right.PartialAssertValue(SN::SN_Bool(false));
					if (e.IsError())
					{
						SNI_CallRecord *callRecord = new SNI_CallRecord("Partial assert left condition false", this);
						LOGGING(callRecord->SetLogContext(context));
						e.GetSNI_Error()->AddNote(callRecord);
					}
					return e;
				}
				else if (SN::Is<SNI_Value *>(right_value) && SN::SN_Bool(right_value).GetBool())
				{
					SN::SN_Error e = p_left.PartialAssertValue(SN::SN_Bool(false));
					if (e.IsError())
					{
						SNI_CallRecord *callRecord = new SNI_CallRecord("Partial assert right condition false", this);
						LOGGING(callRecord->SetLogContext(context));
						e.GetSNI_Error()->AddNote(callRecord);
					}
					return e;
				}
			}
		}
		return SN::SN_Error(false, false);
	}

	size_t SNI_And::CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const
	{
		if (p_ParamList[PC2_First].IsNullValue())
		{
			return CARDINALITY_MAX;
		}
		else
		{
			SN::SN_Bool left = p_ParamList[PC2_First].GetVariableValue();
			if (!left.IsNull() && !left.GetBool())
			{
				return 1;
			}
		}
		if (1 < p_Depth)
		{
			if (p_ParamList[PC2_Second].IsNullValue())
			{
				return CARDINALITY_MAX;
			}
			else
			{
				SN::SN_Bool right = p_ParamList[PC2_Second].GetVariableValue();
				if (!right.IsNull() && !right.GetBool())
				{
					return 1;
				}
			}
		}
		return MultiplyCardinality(p_ParamList[PC2_First].Cardinality(), p_ParamList[PC2_Second].Cardinality());
	}

	size_t SNI_And::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (2 <= p_TotalCalc)
		{
			SN::SN_Bool result = p_ParamList[PU2_Result].GetVariableValue();
			if (!result.IsNull() && result.GetBool())
			{
				return 1;
			}
			SN::SN_Bool left = p_ParamList[PU2_First].GetVariableValue();
			if (!left.IsNull() && !left.GetBool())
			{
				return 1;
			}
			SN::SN_Bool right = p_ParamList[PU2_Second].GetVariableValue();
			if (!right.IsNull() && !right.GetBool())
			{
				return 1;
			}
		}
		return SNI_Binary::CardinalityOfUnify(p_Depth, p_ParamList, p_CalcPos, p_TotalCalc);
	}
}
