#include "sni_assign.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sn.h"
#include "sn_cartesian.h"

#include "sni_delayedprocessor.h"
#include "sni_value.h"
#include "sni_string.h"
#include "sni_stringref.h"
#include "sni_world.h"
#include "sni_worldset.h"
#include "sni_valueset.h"
#include "sni_variable.h"
#include "sni_helpers.h"
#include "sni_error.h"
#include "sni_null.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Assign::SNI_Assign()
	{

	}

	SNI_Assign::~SNI_Assign()
	{

	}

	std::string SNI_Assign::GetTypeName() const
	{
		return "Assign";
	}

	std::string SNI_Assign::DisplayCpp() const
	{
		return "SN::Assign";
	}

	std::string SNI_Assign::DisplaySN(long /*priority*/, SNI_DisplayOptions& /*p_DisplayOptions*/) const
	{
		return "assign";
	}

	long SNI_Assign::GetPriority() const
	{
		return 0;
	}

	std::string SNI_Assign::GetOperator() const
	{
		return ":=";
	}

	SN::SN_Expression* SNI_Assign::LoadParametersUnify(SN::SN_ExpressionList* p_ParameterList) const
	{
		return SNI_FunctionDef::LoadParametersUnify(p_ParameterList);
	}

	long SNI_Assign::GetNumParameters() const
	{
		return 4;
	}

	std::string SNI_Assign::DisplayCall(long priority, SNI_DisplayOptions &p_DisplayOptions, size_t p_NumParams, SN::SN_Expression *p_ParamList, const SNI_Expression *p_DebugSource) const
	{
		std::string left = "_";
		if (PCA3_Left < p_NumParams)
		{
			left = p_ParamList[PCA3_Left].DisplaySN(GetPriority(), p_DisplayOptions);
		}
		std::string right = "_";
		if (PCA3_Right < p_NumParams)
		{
			right = p_ParamList[PCA3_Right].DisplaySN(GetPriority(), p_DisplayOptions);
		}
		std::string state = "_";
		if (PCA3_State < p_NumParams)
		{
			state = p_ParamList[PCA3_State].DisplaySN(GetPriority(), p_DisplayOptions);
		}
		return Bracket(priority, left + SetBreakPoint(GetOperator(), p_DisplayOptions, p_DebugSource, SN::ParameterTwoId) + right + SetBreakPoint(";", p_DisplayOptions, p_DebugSource, SN::ParameterThreeId) + state + SetBreakPoint(";", p_DisplayOptions, p_DebugSource, SN::ParameterFourId), p_DisplayOptions, p_DebugSource);
	}

	SN::SN_Expression SNI_Assign::PrimaryFunctionExpressionOp(const SN::SN_Expression &p_Left, const SN::SN_Expression& p_Right, const SN::SN_Expression& p_State) const
	{
		return SN::SN_Operators::FunctionCall(SN::SN_Operators::FunctionCall(SN::SN_Operators::FunctionCall(this, p_Left), p_Right), p_State);
	}

	SN::SN_Expression SNI_Assign::PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right, const SN::SN_Expression& p_State) const
	{
		return SN::SN_Function(SN::SN_Function(SN::SN_Function(this, p_Left), p_Right), p_State);
	}

	SN::SN_Value SNI_Assign::PrimaryFunctionValue(const SN::SN_Value& p_Left, const SN::SN_Value& p_Right, const SN::SN_Expression& p_State) const
	{
		return p_Left.GetSNI_Value()->DoAssign(p_Right.GetSNI_Value(), p_State.GetSNI_Value());
	}

	SN::SN_Value SNI_Assign::RightInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return skynet::Fail;
	}

	SN::SN_Expression SNI_Assign::RightInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const

	{
		return skynet::Fail;
	}

	/// \brief Return an expression or a value for the operation applied to two values.
	///
	/// Partial evaluation returns an expression if not all values are known.
	///
	/// - Extract first element from list as left and evaluate it.
	/// - Extract second element from list as right and evaluate it.
	/// - If meta level is 0
	///   + Perfom the operation on left and right and return the result.
	/// - Return the expression for the operation on left and right.
	///
	/// @param p_ParameterList List of two parameters.
	/// @param p_MetaLevel The meta level; 0 means evaluate as data. > 0 means return the expression to concatenate left and right.
	/// @retval An expression or value for the operation on two values
	SN::SN_Expression SNI_Assign::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context("SNI_Assign::PartialCall ( " + DisplaySnExpressionList(p_ParameterList) + " )"));

		if (p_ParameterList->size() < 3)
		{
			return MakePartialCallExpression(p_ParameterList, p_MetaLevel);
		}

		size_t numStackParams = p_ParameterList->size()-1;
		SN::SN_Expression left_value = (*p_ParameterList)[numStackParams - PCA3_Left].DoPartialEvaluate(p_MetaLevel);
		SN::SN_Expression right_value = (*p_ParameterList)[numStackParams - PCA3_Right].DoPartialEvaluate(p_MetaLevel);
		SN::SN_Expression state = (*p_ParameterList)[numStackParams - PCA3_State].DoPartialEvaluate(p_MetaLevel);

		if (0 == p_MetaLevel)
		{
			if ((p_MetaLevel <= 0) && left_value.IsKnownValue() && right_value.IsKnownValue())
			{
				return LOG_RETURN(context, PrimaryFunctionValue(left_value, right_value, state));
			}
			return LOG_RETURN(context, PrimaryFunctionExpression(left_value, right_value, state));
		}
		return LOG_RETURN(context, PrimaryFunctionExpressionOp(left_value, right_value, state));
	}

	/// \brief Extract the left and right values from the parameter list and call PartialUnifyInternal.
	///
	/// @param p_ParameterList List of two parameters.
	/// @param p_Result The result of operation on the two values
	/// @retval True if successful, false for failure.
	SN::SN_Error SNI_Assign::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		LOGGING(SN::LogContext context("SNI_Assign::PartialUnify ( " + DisplaySnParameterList(p_ParameterList) + " = " + p_Result.DisplaySN() + " )"));

		size_t numStackParams = p_ParameterList->size();

		if (GetNumParameters()-1 != numStackParams)
		{
			return SN::SN_Error(false, false, GetTypeName() + ": " + to_string(numStackParams) + " parameters given, " + to_string(GetNumParameters() - 1) + " expected.");
		}
		numStackParams--;
		SN::SN_Error e = LOG_RETURN(context, PartialUnifyInternal((*p_ParameterList)[numStackParams-PCA3_Left].GetValue(), (*p_ParameterList)[numStackParams - PCA3_Right].GetValue(), (*p_ParameterList)[numStackParams - PCA3_State].GetValue(), p_Result));
		return e;
	}

	/// \brief Calculate expressions for unknown values from known values, in p_left ? p_right = p_result, where ? is the operation
	///
	/// Partial calculate gives expressions for unknown values from known values.  The operation is determined by the inheriting class (SNI_Add or SNI_Multiply)
	///
	/// - Partially evaluate left and right.
	/// - If left and right are values;
	///   + Apply the operation to left and right and partially assert equal the result.
	/// - Partially evaluate the result.
	/// - If the result is a value
	///   + If left is a value
	///     * Apply the inverse operation to the result and left and partially assert equal to the right,
	///   + If right is a value
	///     * Apply the inverse operation to the result and right and partially assert equal to the left,
	/// - If the left is a variable;
	///   + Partially assert the left is equal to the inverse expression for result and right.
	/// - If the right is a variable;
	///   + Partially assert the right is equal to the inverse expression for result and left.
	/// - If the result is a variable;
	///   + Partially assert the result is equal to the expression on left and right.
	/// - Return failure (false).
	///
	/// @param p_left First parameter
	/// @param p_right Second parameter
	/// @param p_result the result of the operation
	/// @retval True if successful, false for failure.
	SN::SN_Error SNI_Assign::PartialUnifyInternal(SN::SN_Expression &p_left, SN::SN_Expression& p_right, SN::SN_Expression& p_state, SN::SN_Expression &p_Result)
	{
		LOGGING(SN::LogContext context("SNI_Assign::PartialUnifyInternal"));

		SN::SN_Expression left_value = p_left.DoPartialEvaluate();
		SN::SN_Expression right_value = p_right.DoPartialEvaluate();
		SN::SN_Expression state = p_state.DoPartialEvaluate();
		if (SN::Is<SNI_Value *>(left_value) && SN::Is<SNI_Value *>(right_value))
		{
			SN::SN_Error e = p_Result.PartialAssertValue(PrimaryFunctionValue(left_value, right_value, state));
			if (e.IsSignificantError())
			{
				SNI_CallRecord *callRecord = new SNI_CallRecord("Assert to primary function value.", this);
				LOGGING(callRecord->SetLogContext(context));
				e.GetSNI_Error()->AddNote(callRecord);
			}
			return e;
		}
		return skynet::Fail;
	}

	size_t SNI_Assign::CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const
	{
		if (p_Depth == 2)
		{
			if (p_ParamList[PCA3_Left].IsNullValue())
			{
				return CARDINALITY_MAX;
			}
			if (p_ParamList[PCA3_Right].IsNullValue())
			{
				return CARDINALITY_MAX;
			}
			if (p_ParamList[PCA3_State].IsNullValue())
			{
				return CARDINALITY_MAX;
			}
			return MultiplyCardinality(p_ParamList[PCA3_Left].Cardinality(), MultiplyCardinality(p_ParamList[PCA3_Right].Cardinality(), p_ParamList[PCA3_State].Cardinality()));
		}
		return CARDINALITY_MAX;
	}

	SN::SN_Value SNI_Assign::CallElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, SN::SN_ValueSet p_Result) const
	{
		if (p_WorldList)
		{
			p_Result.AddValue(PrimaryFunctionValue(p_ParamList[PCA3_Left].GetVariableValue(), p_ParamList[PCA3_Right].GetVariableValue(), p_ParamList[PCA3_State].GetVariableValue()), p_Depth, p_WorldList, NULL);
			return skynet::OK;
		}
		else
		{
			return PrimaryFunctionValue(p_ParamList[PCA3_Left].GetVariableValue(), p_ParamList[PCA3_Right].GetVariableValue(), p_ParamList[PCA3_State].GetVariableValue());
		}
	}

	size_t SNI_Assign::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		ASSERTM(p_TotalCalc >= 0, "Negative number of pararmeters to calculate is impossible.");
		if (p_TotalCalc == 0)
		{
			return MultiplyCardinality(p_ParamList[PUA3_Left].Cardinality(), MultiplyCardinality(p_ParamList[PUA3_Right].Cardinality(), MultiplyCardinality(p_ParamList[PUA3_State].Cardinality(), p_ParamList[PUA3_Result].Cardinality())));
		}
		else if (p_TotalCalc == 1)
		{
			switch (p_CalcPos)
			{
			case PUA3_Right:
				return MultiplyCardinality(p_ParamList[PUA3_Left].Cardinality(), p_ParamList[PUA3_Result].Cardinality());
			case PUA3_Result:
				return MultiplyCardinality(p_ParamList[PUA3_Left].Cardinality(), p_ParamList[PUA3_Right].Cardinality());
			}
		}
		return CARDINALITY_MAX;
	}

	SN::SN_Error SNI_Assign::UnifyElement(long p_Depth, SN::SN_Expression *p_ParamList, SNI_World **p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet *worldSet, const SNI_Expression *p_Source) const
	{
		switch (p_TotalCalc)
		{
		case 0:
		{
			if (p_WorldList)
			{
				bool exists = false;
				SNI_World* world = worldSet->JoinWorldsArray(ManualAddWorld, AlwaysCreateWorld, exists, p_Depth, p_WorldList);
				if (exists)
				{
					if (PrimaryFunctionValue(p_ParamList[PUA3_Left].GetVariableValue(), p_ParamList[PUA3_Right].GetVariableValue(), p_ParamList[PUA3_State].GetVariableValue()).Equivalent(p_ParamList[PUA3_Result].GetVariableValue()))
					{
						world->AddToSetList();
					}
				}
				return skynet::OK;
			}
			else
			{
				SN::SN_Value value = PrimaryFunctionValue(p_ParamList[PUA3_Left].GetVariableValue(), p_ParamList[PUA3_Right].GetVariableValue(), p_ParamList[PUA3_State].GetVariableValue());
				return SN::SN_Error(value.Equivalent(p_ParamList[PUA3_Result].GetVariableValue()), false);
			}
		}
		break;
		case 1:
		{
			switch (p_CalcPos)
			{
			case PUA3_Right:
				return skynet::Fail; // Maybe possible. Dog dog dog.
			case PUA3_Result:
			{
				SN::SN_Value value = PrimaryFunctionValue(p_ParamList[PUA3_Left].GetVariableValue(), p_ParamList[PUA3_Right].GetVariableValue(), p_ParamList[PUA3_State].GetVariableValue());
				return p_ParamList[p_CalcPos].AddValue(value, p_Depth, p_WorldList, worldSet);
			}
			}
		}
		}
		return SN::SN_Error(false, false, GetTypeName() + ": Expression not unified. TotalCalc=" + to_string(p_TotalCalc) + " Calcpos=" + to_string(p_CalcPos));
	}
}
