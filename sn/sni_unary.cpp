#include "sni_unary.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sn_function.h"
#include "sn_cartesian.h"
#include "sn_error.h"

#include "sni_delayedprocessor.h"
#include "sni_value.h"
#include "sni_valueset.h"
#include "sni_function.h"
#include "sni_world.h"
#include "sni_worldset.h"
#include "sni_error.h"
#include "sni_null.h"
#include "sni_helpers.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Unary::SNI_Unary()
	{
	}

	SNI_Unary::~SNI_Unary()
	{
	}

	string SNI_Unary::GetTypeName() const
	{
		return "Unary";
	}

	string SNI_Unary::DisplayCpp() const
	{
		return "SN::Unary";
	}

	string SNI_Unary::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "Unary Op";
	}

	string SNI_Unary::DisplayCall(long priority, SNI_DisplayOptions &p_DisplayOptions, size_t p_NumParams, SN::SN_Expression *p_ParamList, const SNI_Expression *p_DebugSource) const
	{
		string text;
		if (GetOperator().empty())
		{
			text = SetBreakPoint(GetTypeName(), p_DisplayOptions, p_DebugSource, SN::LeftId) + " ";
		}
		else
		{
			text = SetBreakPoint(GetOperator(), p_DisplayOptions, p_DebugSource, SN::LeftId);
		}
		return Bracket(priority, 
			text + " " + p_ParamList[PC1_First].GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions) + SetBreakPoint(";", p_DisplayOptions, p_DebugSource, SN::ParameterOneId+1), p_DisplayOptions, p_DebugSource);
	}

	long SNI_Unary::GetPriority() const
	{
		return 4;
	}

	long SNI_Unary::GetNumParameters() const
	{
		return 2;
	}

	SN::SN_Value SNI_Unary::InverseFunctionValue(const SN::SN_Value & p_Param) const
	{
		return PrimaryFunctionValue(p_Param);
	}

	SN::SN_Expression SNI_Unary::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return PrimaryFunctionExpression(p_Param);
	}

	SN::SN_Expression SNI_Unary::CallArray(SN::SN_Expression * p_ParamList, long p_MetaLevel /* = 0 */) const
	{
		if (0 < p_MetaLevel)
		{
			return PrimaryFunctionExpression(p_ParamList[0].DoEvaluate(p_MetaLevel));
		}

		return SNI_FunctionDef::CallArray(p_ParamList, p_MetaLevel);
	}

	SN::SN_Expression SNI_Unary::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context("SNI_Unary::PartialCall ( " + DisplayPmExpressionList(p_ParameterList) + " )"));

		SN::SN_Expression value = (*p_ParameterList)[0].DoPartialEvaluate(p_MetaLevel);

		if (0 == p_MetaLevel)
		{
			if (SN::Is<SNI_Value *>(value))
			{
				return LOG_RETURN(context, PrimaryFunctionValue(value));
			}
		}
		return LOG_RETURN(context, PrimaryFunctionExpression(value));
	}

	SN::SN_Error SNI_Unary::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		LOGGING(SN::LogContext context("SNI_Unary::Unify ( " + DisplayPmParameterList(p_ParameterList) + " = " + p_Result.DisplaySN() + " )"));

		SN::SN_Error e = LOG_RETURN(context, PartialUnifyInternal((*p_ParameterList)[0].GetValue(), p_Result));
		if (e.IsSignificantError())
		{
			SNI_CallRecord *callRecord = new SNI_CallRecord("Partial unify.", this);
			LOGGING(callRecord->SetLogContext(context));
			e.GetSNI_Error()->AddNote(callRecord);
		}
		return e;
	}

	SN::SN_Error SNI_Unary::PartialUnifyInternal(SN::SN_Expression &p_value, SN::SN_Expression &p_Result)
	{
		LOGGING(SN::LogContext context("SNI_Unary::PartialUnifyInternal ( " + p_value.DisplaySN() + " = " + p_Result.DisplaySN() + " )"));

		SN::SN_Expression value = p_value.DoPartialEvaluate();
		if (SN::Is<SNI_Value *>(value))
		{
			return p_Result.PartialAssertValue(PrimaryFunctionValue(value));
		}
		SN::SN_Expression result = p_Result.DoPartialEvaluate();
		if (SN::Is<SNI_Value *>(result))
		{
			SN::SN_Value newValue = InverseFunctionValue(result);
			if (!newValue.IsNull())
			{
				return p_value.PartialAssertValue(newValue);
			}
		}
		return false;
	}

	size_t SNI_Unary::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (p_Depth == 2)
		{
			if (p_TotalCalc == 0)
			{
				return MultiplyCardinality(p_ParamList[PU1_Result].Cardinality(), p_ParamList[PU1_First].Cardinality());
			}
			if (p_TotalCalc == 1)
			{
				if (p_Depth == 1)
				{
					return p_ParamList[PU1_First].Cardinality();
				}
				switch (p_CalcPos)
				{
				case PU1_First:
					return p_ParamList[PU1_Result].Cardinality();
				case PU1_Result:
					return p_ParamList[PU1_First].Cardinality();
				}
			}
		}
		return CARDINALITY_MAX;
	}

	SN::SN_Error SNI_Unary::UnifyElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet * worldSet, const SNI_Expression *p_Source) const
	{
		LOGGING(SN::LogContext context("SNI_Unary::UnifyElement(CalcPos " + to_string(p_CalcPos) + " TotalCalc " + to_string(p_TotalCalc) + " " + DisplayValues(p_Depth, p_ParamList, p_WorldList) + ")"));

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
					if (PrimaryFunctionValue(p_ParamList[PU1_First].GetVariableValue()).Equivalent(p_ParamList[PU1_Result].GetVariableValue()))
					{
						world->AddToSetList();
					}
					else
					{
						LOGGING(context.LogText("fail", "Value conflict on " + DisplayValues(p_Depth, p_ParamList, p_WorldList)));
					}
				}
				return true;
			}
			else
			{
				return PrimaryFunctionValue(p_ParamList[PU1_First].GetVariableValue()).Equivalent(p_ParamList[PU1_Result].GetVariableValue());
			}
		}
		break;
		case 1:
		{
			switch (p_CalcPos)
			{
			case PU1_First:
			{
				SN::SN_Value value = InverseFunctionValue(p_ParamList[PU1_Result].GetVariableValue());
				return p_ParamList[p_CalcPos].AddValue(value, p_Depth, p_WorldList, worldSet);
			}
			case PU1_Result:
			{
				SN::SN_Value value = PrimaryFunctionValue(p_ParamList[PU1_First].GetVariableValue());
				return p_ParamList[p_CalcPos].AddValue(value, p_Depth, p_WorldList, worldSet);
			}
			}
		}
		}
		return SN::SN_Error(GetTypeName() + ": Expression not unified. TotalCalc=" + to_string(p_TotalCalc) + " Calcpos=" + to_string(p_CalcPos));
	}

	bool SNI_Unary::EvaluateNow(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		return p_TotalCalc <= 1;
	}

	size_t SNI_Unary::CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const
	{
		if (p_ParamList[PC1_First].IsNull())
		{
			return CARDINALITY_MAX;
		}
		return p_ParamList[PC1_First].Cardinality();
	}
	
	SN::SN_Value SNI_Unary::CallElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, SN::SN_ValueSet p_Result) const
	{
		if (p_WorldList)
		{
			p_Result.AddValue(PrimaryFunctionValue(p_ParamList[PC1_First].GetVariableValue()), p_Depth, p_WorldList, NULL);
			return skynet::OK;
		}
		else
		{
			return PrimaryFunctionValue(p_ParamList[PC1_First].GetVariableValue());
		}
	}
}
