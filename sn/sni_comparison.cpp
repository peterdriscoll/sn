#include "sni_comparison.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sn_error.h"
#include "sn_parameter.h"
#include "sn_functiondef.h"

#include "sni_delayedprocessor.h"
#include "sni_null.h"
#include "sni_error.h"
#include "sni_helpers.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Comparison::SNI_Comparison()
	{
	}

	SNI_Comparison::~SNI_Comparison()
	{
	}

	SN::SN_Value SNI_Comparison::LeftInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return SN::SN_Error(false, false, "Internal error - Left inverse expression not defined.");
	}

	SN::SN_Expression SNI_Comparison::LeftInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const

	{
		return SN::SN_Error(false, false, "Internal error - Left inverse expression not defined.");
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
	SN::SN_Expression SNI_Comparison::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context("SNI_Comparison::PartialCall ( " + DisplayPmExpressionList(p_ParameterList) + " )"));

		SN::SN_Expression left_value = (*p_ParameterList)[1].DoPartialEvaluate(p_MetaLevel);
		SN::SN_Expression right_value = (*p_ParameterList)[0].DoPartialEvaluate(p_MetaLevel);

		if (0 == p_MetaLevel)
		{
			if (SN::Is<SNI_Value *>(left_value) && SN::Is<SNI_Value *>(right_value))
			{
				return LOG_RETURN(context, PrimaryFunctionValue(left_value, right_value));
			}
			return LOG_RETURN(context, PrimaryFunctionExpression(left_value, right_value));
		}
		return LOG_RETURN(context, PrimaryFunctionExpressionOp(left_value, right_value));
	}

	SN::SN_Error SNI_Comparison::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		LOGGING(SN::LogContext context("SNI_Comparison::PartialUnifyInternal ( " + DisplayPmParameterList(p_ParameterList) + " = " + p_Result.DisplaySN() + " )"));

		return LOG_RETURN(context, PartialUnifyInternal((*p_ParameterList)[1].GetValue(), (*p_ParameterList)[0].GetValue(), p_Result));
	}

	SN::SN_Error SNI_Comparison::PartialUnifyInternal(SN::SN_Expression &p_left, SN::SN_Expression &p_right, SN::SN_Expression &p_Result)
	{
		SN::SN_Expression left_evaluate = p_left.DoPartialEvaluate();
		SN::SN_Expression right_evaluate = p_right.DoPartialEvaluate();
		if (SN::Is<SNI_Value *>(left_evaluate) && SN::Is<SNI_Value *>(right_evaluate))
		{
			return p_Result.AssertValue(PrimaryFunctionValue(left_evaluate, right_evaluate));
		}
		return skynet::Fail;
	}

	size_t SNI_Comparison::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
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
		}
		return CARDINALITY_MAX;
	}

	SN::SN_Error SNI_Comparison::UnifyElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet * worldSet, const SNI_Expression *p_Source) const
	{
		LOGGING(SN::LogContext context("SNI_Comparison::UnifyElement(CalcPos " + to_string(p_CalcPos) + " TotalCalc " + to_string(p_TotalCalc) + " " + DisplayValues(p_Depth, p_ParamList, p_WorldList) + ")"));

#ifdef USE_LOGGING
		if (worldSet)
		{
			LOGGING(context.LogText("World set", worldSet->DisplayLong()));
		}
#endif
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
#ifdef USE_LOGGING
					else
					{
						LOGGING(context.LogText("fail", "Value conflict on " + DisplayValues(p_Depth, p_ParamList, p_WorldList)));
					}
#endif
				}
#ifdef USE_LOGGING
				else
				{
					LOGGING(context.LogText("fail", "Join worlds failed on " + DisplayWorlds(p_Depth, p_WorldList)));
				}
#endif
				return LOG_RETURN(context, skynet::OK);
			}
			else
			{
				return LOG_RETURN(context, SN::SN_Error(PrimaryFunctionValue(p_ParamList[PU2_First].GetVariableValue(), p_ParamList[PU2_Second].GetVariableValue()).Equivalent(p_ParamList[PU2_Result].GetVariableValue()), false));
			}
		}
		case 1:
		{
			if (p_CalcPos == PU2_Result)
			{
				return LOG_RETURN(context, p_ParamList[p_CalcPos].AddValue(PrimaryFunctionValue(p_ParamList[PU2_First].GetVariableValue(), p_ParamList[PU2_Second].GetVariableValue()), p_Depth, p_WorldList, worldSet));
			}
		}
		}
		return LOG_RETURN(context, SN::SN_Error(false, false, GetTypeName() + ": Expression not unified. TotalCalc=" + to_string(p_TotalCalc) + " Calcpos=" + to_string(p_CalcPos)));
	}
}
