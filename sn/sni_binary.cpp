// SNI_Binary.cpp: implementation of the SNI_Binary class.
//
// Description:
// Implement the common logic for operations that are,
// - Commutative
// - Have a well defined inverse.
//
// The class is inherited by,
// - SNI_Add
// - SNI_Multiply
// 
// These functions must be overridden.
// - PrimaryFunctionValue - Apply the operation to values.
// - PrimaryFunctionExpression - Construct an expression for the operation.
// - LeftInverseFunctionValue - Apply the inverse operation to values
// - LeftInverseFunctionValue - Construct an expression for the inverse operation.

//////////////////////////////////////////////////////////////////////

#include "sni_binary.h"

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
	SNI_Binary::SNI_Binary()
	{

	}

	SNI_Binary::~SNI_Binary()
	{

	}

	long SNI_Binary::GetNumParameters() const
	{
		return 3;
	}

	string SNI_Binary::DisplayCall(long priority, SNI_DisplayOptions &p_DisplayOptions, size_t p_NumParams, SN::SN_Expression *p_ParamList, const SNI_Expression *p_DebugSource) const
	{
		if (GetOperator().empty())
		{
			return SNI_FunctionDef::DisplayCall(priority, p_DisplayOptions, p_NumParams, p_ParamList, p_DebugSource);
		}
		return Bracket(priority, p_ParamList[PC2_First].DisplaySN(GetPriority(), p_DisplayOptions) + SetBreakPoint(GetOperator(), p_DisplayOptions, p_DebugSource, SN::ParameterTwoId) + p_ParamList[PC2_Second].DisplaySN(GetPriority(), p_DisplayOptions) + SetBreakPoint(";", p_DisplayOptions, p_DebugSource, SN::ParameterThreeId), p_DisplayOptions, p_DebugSource);
	}

	SN::SN_Value SNI_Binary::RightInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return LeftInverseFunctionValue(p_Left, p_Right);
	}

	SN::SN_Expression SNI_Binary::RightInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const

	{
		return LeftInverseFunctionExpression(p_Left, p_Right);
	}

	/// - Extract second element from list as right and evaluate it.
	///   + If unknown then return unknown
	/// - If meta level is greater than 0
	///   + Return the expression for the operation (but dont evaluate it).
	/// - Perfom the operation on the left and right values and return the result.
	///
	/// @param p_ParameterList List of two parameters to be concatenated.
	/// @param p_MetaLevel The meta level; 0 means evaluate the value. > 0 means return the expression to concatenate left and right.
	/// @retval A value for the operation applied to two values.
	SN::SN_Expression SNI_Binary::CallArray(SN::SN_Expression * p_ParamList, long p_MetaLevel /* = 0 */) const
	{
		if (0 < p_MetaLevel)
		{
			return PrimaryFunctionExpression(p_ParamList[0].DoEvaluate(p_MetaLevel), p_ParamList[1].DoEvaluate(p_MetaLevel));
		}

		return SNI_FunctionDef::CallArray(p_ParamList, p_MetaLevel);
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
	SN::SN_Expression SNI_Binary::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context("SNI_Binary::PartialCall ( " + DisplayPmExpressionList(p_ParameterList) + " )"));

		SN::SN_Expression left_value = (*p_ParameterList)[1].DoPartialEvaluate(p_MetaLevel);
		SN::SN_Expression right_value = (*p_ParameterList)[0].DoPartialEvaluate(p_MetaLevel);

		if (0 == p_MetaLevel)
		{
			if ((p_MetaLevel <= 0) && SN::Is<SNI_Value *>(left_value) && SN::Is<SNI_Value *>(right_value))
			{
				return LOG_RETURN(context, PrimaryFunctionValue(left_value, right_value));
			}
		}
		return LOG_RETURN(context, PrimaryFunctionExpression(left_value, right_value));
	}

	/// \brief Extract the left and right values from the parameter list and call PartialUnifyInternal.
	///
	/// @param p_ParameterList List of two parameters.
	/// @param p_Result The result of operation on the two values
	/// @retval True if successful, false for failure.
	SN::SN_Error SNI_Binary::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		LOGGING(SN::LogContext context("SNI_Binary::PartialUnify ( " + DisplayPmParameterList(p_ParameterList) + " = " + p_Result.DisplaySN() + " )"));

		SN::SN_Error e = LOG_RETURN(context, PartialUnifyInternal((*p_ParameterList)[1].GetValue(), (*p_ParameterList)[0].GetValue(), p_Result));
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
	SN::SN_Error SNI_Binary::PartialUnifyInternal(SN::SN_Expression &p_left, SN::SN_Expression &p_right, SN::SN_Expression &p_Result)
	{
		LOGGING(SN::LogContext context("SNI_Binary::PartialUnifyInternal"));

		SN::SN_Expression left_value = p_left.DoPartialEvaluate();
		SN::SN_Expression right_value = p_right.DoPartialEvaluate();
		if (SN::Is<SNI_Value *>(left_value) && SN::Is<SNI_Value *>(right_value))
		{
			SN::SN_Error e = p_Result.PartialAssertValue(PrimaryFunctionValue(left_value, right_value));
			if (e.IsSignificantError())
			{
				SNI_CallRecord *callRecord = new SNI_CallRecord("Assert to primary function value.", this);
				LOGGING(callRecord->SetLogContext(context));
				e.GetSNI_Error()->AddNote(callRecord);
			}
			return e;
		}
		SN::SN_Expression result = p_Result.DoPartialEvaluate();
		if (SN::Is<SNI_Value *>(result))
		{
			if (SN::Is<SNI_Variable *>(left_value) && SN::Is<SNI_Variable *>(right_value))
			{
				if (SN::Is<SNI_String *>(result) || SN::Is<SNI_StringRef *>(result))
				{
					SN::SN_Variable splitPoint;
					SN::SN_String s_result = result;
					p_left = SN::SN_StringRef(result, s_result.GetSNI_String()->GetStart(), splitPoint);
					p_right = SN::SN_StringRef(result, splitPoint, s_result.GetSNI_String()->GetEnd());
					return skynet::OK;
				}
			}
			else if (SN::Is<SNI_Variable *>(left_value))
			{
				if (SN::Is<SNI_Value *>(right_value))
				{
					SN::SN_Error e = left_value.PartialAssertValue(RightInverseFunctionValue(result, right_value));
					if (e.IsSignificantError())
					{
						SNI_CallRecord *callRecord = new SNI_CallRecord("Partial assert to right inverse function value.", this);
						LOGGING(callRecord->SetLogContext(context));
						e.GetSNI_Error()->AddNote(callRecord);
					}
					return e;
				}
				SN::SN_Error e = left_value.PartialAssertValue(RightInverseFunctionExpression(result, right_value));
				if (e.IsSignificantError())
				{
					SNI_CallRecord *callRecord = new SNI_CallRecord("Partial assert to right inverse function expression.", this);
					LOGGING(callRecord->SetLogContext(context));
					e.GetSNI_Error()->AddNote(callRecord);
				}
				return e;
			}
			else if (SN::Is<SNI_Variable *>(right_value))
			{
				if (SN::Is<SNI_Value *>(left_value))
				{
					SN::SN_Error e = right_value.PartialAssertValue(LeftInverseFunctionValue(result, left_value));
					if (e.IsSignificantError())
					{
						SNI_CallRecord *callRecord = new SNI_CallRecord("Partial assert to left inverse function value.", this);
						LOGGING(callRecord->SetLogContext(context));
						e.GetSNI_Error()->AddNote(callRecord);
					}
					return e;
				}
				SN::SN_Error e = right_value.PartialAssertValue(LeftInverseFunctionExpression(result, left_value));
				if (e.IsSignificantError())
				{
					SNI_CallRecord *callRecord = new SNI_CallRecord("Partial assert to left inverse function expression.", this);
					LOGGING(callRecord->SetLogContext(context));
					e.GetSNI_Error()->AddNote(callRecord);
				}
				return e;
			}
		}
		bool ok = false;
		if (SN::Is<SNI_Variable *>(left_value))
		{
			ok |= left_value.PartialAssertValue(RightInverseFunctionExpression(result, right_value)).GetBool();
		}
		if (SN::Is<SNI_Variable *>(right_value))
		{
			ok |= right_value.PartialAssertValue(LeftInverseFunctionExpression(result, left_value)).GetBool();
		}
		if (SN::Is<SNI_Variable *>(result))
		{
			ok |= result.PartialAssertValue(PrimaryFunctionExpression(left_value, right_value)).GetBool();
		}
		return SN::SN_Error(ok, false);
	}

	size_t SNI_Binary::CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const
	{
		if (p_Depth == 2)
		{
			if (p_ParamList[PC2_First].IsNullValue())
			{
				return CARDINALITY_MAX;
			}
			if (p_ParamList[PC2_Second].IsNullValue())
			{
				return CARDINALITY_MAX;
			}
			return MultiplyCardinality(p_ParamList[PC2_First].Cardinality(), p_ParamList[PC2_Second].Cardinality());
		}
		return CARDINALITY_MAX;
	}

	SN::SN_Value SNI_Binary::CallElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, SN::SN_ValueSet p_Result) const
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

	size_t SNI_Binary::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		ASSERTM(p_TotalCalc >= 0, "Negative number of pararmeters to calculate is impossible.");
		if (p_TotalCalc == 0)
		{
			return MultiplyCardinality(p_ParamList[PU2_First].Cardinality(), MultiplyCardinality(p_ParamList[PU2_Second].Cardinality(), p_ParamList[PU2_Result].Cardinality()));
		}
		else if (p_TotalCalc == 1)
		{
			switch (p_CalcPos)
			{
			case PU2_First:
				return MultiplyCardinality(p_ParamList[PU2_Second].Cardinality(), p_ParamList[PU2_Result].Cardinality());
			case PU2_Second:
				return MultiplyCardinality(p_ParamList[PU2_First].Cardinality(), p_ParamList[PU2_Result].Cardinality());
			case PU2_Result:
				return MultiplyCardinality(p_ParamList[PU2_First].Cardinality(), p_ParamList[PU2_Second].Cardinality());
			}
		}
		else if (p_Depth == 3)
		{  // This is for SNI_Add. Move. ????
			SN::SN_Value result = p_ParamList[PU2_Result].GetVariableValue();
			if (result.IsStringValue())
			{
				if (result.IsReferableValue() && result.IsNull())
				{   // A string ref can still be used in a split, even if it is null.
					if (p_TotalCalc <= 3)
					{
						return 1;
					}
				}
				else if (p_TotalCalc <= 2)
				{
					return 1;
				}
			}
		}
		return CARDINALITY_MAX;
	}

	SN::SN_Error SNI_Binary::UnifyElement(long p_Depth, SN::SN_Expression *p_ParamList, SNI_World **p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet *worldSet, const SNI_Expression *p_Source) const
	{
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
				return skynet::OK;
			}
			else
			{
				SN::SN_Value value = PrimaryFunctionValue(p_ParamList[PU2_First].GetVariableValue(), p_ParamList[PU2_Second].GetVariableValue());
				return SN::SN_Error(value.Equivalent(p_ParamList[PU2_Result].GetVariableValue()), false);
			}
		}
		break;
		case 1:
		{
			switch (p_CalcPos)
			{
			case PU2_First:
			{
				SN::SN_Value value = RightInverseFunctionValue(p_ParamList[PU2_Result].GetVariableValue(), p_ParamList[PU2_Second].GetVariableValue());
				return p_ParamList[p_CalcPos].AddValue(value, p_Depth, p_WorldList, worldSet);
			}
			case PU2_Second:
			{
				SN::SN_Value value = LeftInverseFunctionValue(p_ParamList[PU2_Result].GetVariableValue(), p_ParamList[PU2_First].GetVariableValue());
				return p_ParamList[p_CalcPos].AddValue(value , p_Depth, p_WorldList, worldSet);
			}
			case PU2_Result:
			{
				SN::SN_Value value = PrimaryFunctionValue(p_ParamList[PU2_First].GetVariableValue(), p_ParamList[PU2_Second].GetVariableValue());
				return p_ParamList[p_CalcPos].AddValue(value, p_Depth, p_WorldList, worldSet);
			}
			}
		}
		case 2:
		case 3:
		{
			SN::SN_Value result = p_ParamList[PU2_Result].GetVariableValue();
			if (result.IsStringValue())
			{
				if (result.IsReferableValue())
				{
					if (p_ParamList[PU2_First].IsStringValue())
					{
						return p_ParamList[PU2_Second].AddValue(LeftInverseFunctionValue(result, p_ParamList[PU2_First]), p_Depth, p_WorldList, worldSet);
					}
					else if (p_ParamList[PU2_Second].IsStringValue())
					{
						return p_ParamList[PU2_First].AddValue(RightInverseFunctionValue(result, p_ParamList[PU2_Second]), p_Depth, p_WorldList, worldSet);
					}
				}
				SNI_Frame *topFrame = SNI_Frame::Top();
				SN::SN_String s_result = result;
				result.ForEach(
				[topFrame, &p_ParamList, p_Depth, p_WorldList, worldSet](const SN::SN_Expression &p_Param, SNI::SNI_World *p_World) -> SN::SN_Error
				{
					if (p_Param.IsKnownValue() && p_Param.GetString() == "")
					{
						p_ParamList[PU2_First].AddValue(p_Param, p_Depth, p_WorldList, worldSet);
						p_ParamList[PU2_Second].AddValue(p_Param, p_Depth, p_WorldList, worldSet);
					}
					else
					{
						SN::SN_Variable splitPoint = topFrame->CreateSplitVariable();
						p_ParamList[PU2_First].AddValue(SN::SN_StringRef(p_Param, p_Param.GetSNI_String()->GetStart(), splitPoint), p_Depth, p_WorldList, worldSet);
						p_ParamList[PU2_Second].AddValue(SN::SN_StringRef(p_Param, splitPoint, p_Param.GetSNI_String()->GetEnd()), p_Depth, p_WorldList, worldSet);
						return skynet::OK;
					}
				});
				return skynet::OK;
			}
		}
		}
		return SN::SN_Error(false, false, GetTypeName() + ": Expression not unified. TotalCalc=" + to_string(p_TotalCalc) + " Calcpos="+ to_string(p_CalcPos));
	}
}
