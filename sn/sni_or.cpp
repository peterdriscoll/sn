#include "sni_or.h"

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
#include "sni_unaryor.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Or::SNI_Or()
		: m_Partial(true)
	{
	}

	SNI_Or::SNI_Or(bool p_Partial)
		: m_Partial(p_Partial)
	{
	}

	SNI_Or::~SNI_Or()
	{
	}

	string SNI_Or::GetTypeName() const
	{
		return "Or";
	}

	string SNI_Or::DisplayCpp() const
	{
		return "SN::Or";
	}

	string SNI_Or::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "(|)";
	}

	long SNI_Or::GetPriority() const
	{
		return 1;
	}

	string SNI_Or::GetOperator() const
	{
		return "|";
	}

	SN::SN_Value SNI_Or::PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Left.DoOr(p_Right);
	}

	SN::SN_Expression SNI_Or::PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return p_Left || p_Right;
	}

	SN::SN_Value SNI_Or::LeftInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Left) const
	{
		return p_Result.DoRevOr(p_Left);
	}

	SN::SN_Expression SNI_Or::LeftInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Left) const
	{
		return p_Result.RevOr(p_Left);
	}

	SN::SN_Value SNI_Or::RightInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Right) const
	{
		return p_Result.DoRevOr(p_Right);
	}

	SN::SN_Expression SNI_Or::RightInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Right) const
	{
		return p_Right.RevOr(p_Result);
	}

	SN::SN_Expression SNI_Or::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context("SNI_Or::PartialCall ( " + DisplayPmExpressionList(p_ParameterList) + " )"));

		SN::SN_Expression left_value = (*p_ParameterList)[1].DoPartialEvaluate(p_MetaLevel);
		SN::SN_Expression right_value = (*p_ParameterList)[0].DoPartialEvaluate(p_MetaLevel);

		if (0 == p_MetaLevel)
		{
			if (SN::Is<SNI_Value *>(left_value) && SN::Is<SNI_Value *>(right_value))
			{
				return LOG_RETURN(context, left_value.DoOr(right_value));
			}
			if (SN::Is<SNI_Bool *>(left_value))
			{
				if (SN::SN_Bool(left_value).GetBool())
				{
					return LOG_RETURN(context, SN::SN_Bool(true));
				}
				else
				{
					return LOG_RETURN(context, right_value);
				}
			}
			if (SN::Is<SNI_Bool *>(right_value))
			{
				if (SN::SN_Bool(right_value).GetBool())
				{
					return LOG_RETURN(context, SN::SN_Bool(true));
				}
				else
				{
					return LOG_RETURN(context, left_value);
				}
			}
		}
		return LOG_RETURN(context, left_value || right_value);
	}

	SN::SN_Expression SNI_Or::UnifyArray(SN::SN_Expression * p_ParameterList, const SNI_Expression *p_Source)
	{
		string debugId = SNI_Thread::GetThread()->GetDebugId();

		SNI_Thread::GetThread()->SetDebugId(debugId);
		SN::SN_Expression* firstParamList = new SN::SN_Expression[2];
		firstParamList[0] = p_ParameterList[PU2_Result];
		firstParamList[1] = p_ParameterList[PU2_First];
		SN::SN_Error e1 = skynet::UnaryOr.GetSNI_FunctionDef()->UnifyArray(firstParamList, p_Source).GetError();

		if (e1.IsSignificantError())
		{
			SNI_CallRecord *callRecord = new SNI_CallRecord("Or of first parameter.", this);
			e1.GetSNI_Error()->AddNote(callRecord);
			return e1;
		}

		SNI_Thread::GetThread()->SetDebugId(debugId);
		SN::SN_Expression* secondParamList = new SN::SN_Expression[3];
		secondParamList[0] = p_ParameterList[PU2_Result];
		secondParamList[1] = firstParamList[1];
		secondParamList[2] = p_ParameterList[PU2_Second];
		SN::SN_Error e2 = SNI_Binary::UnifyArray(secondParamList, p_Source).GetError();
		if (e2.IsSignificantError())
		{
			SNI_CallRecord *callRecord = new SNI_CallRecord("Or of second parameter with first result.", this);
			e2.GetSNI_Error()->AddNote(callRecord);
		}
		return e2;
	}

	SN::SN_Error SNI_Or::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		LOGGING(SN::LogContext context("SNI_Or::PartialUnify ( " + DisplayPmParameterList(p_ParameterList) + " = " + p_Result.DisplaySN() + " )"));

		return LOG_RETURN(context, PartialUnifyInternal((*p_ParameterList)[1].GetValue(), (*p_ParameterList)[0].GetValue(), p_Result));
	}

	SN::SN_Error SNI_Or::PartialUnifyInternal(SN::SN_Expression &p_left, SN::SN_Expression &p_right, SN::SN_Expression &p_Result)
	{
		LOGGING(SN::LogContext context("SNI_Or::PartialUnifyInternal"));

		SN::SN_Expression left_value = p_left.DoPartialEvaluate();
		SN::SN_Expression right_value = p_right.DoPartialEvaluate();
		if (SN::Is<SNI_Value *>(left_value) && SN::Is<SNI_Value *>(right_value))
		{
			return p_Result.PartialAssertValue(left_value.DoOr(right_value));
		}
		SN::SN_Expression result = p_Result.DoPartialEvaluate();
		if (SN::Is<SNI_Value *>(result))
		{
			if (SN::SN_Bool(result).GetBool())
			{
				if (SN::Is<SNI_Bool *>(right_value) && !SN::SN_Bool(right_value).GetBool())
				{
					SN::SN_Error e = p_left.PartialAssertValue(SN::SN_Bool(true));
					if (e.IsSignificantError())
					{
						SNI_CallRecord *callRecord = new SNI_CallRecord("Partial Assert left condition true.", this);
						LOGGING(callRecord->SetLogContext(context));
						e.GetSNI_Error()->AddNote(callRecord);
					}
					return e;
				}
				if (SN::Is<SNI_Bool *>(left_value) && !SN::SN_Bool(left_value).GetBool())
				{
					SN::SN_Error e = p_right.PartialAssertValue(SN::SN_Bool(true));
					if (e.IsSignificantError())
					{
						SNI_CallRecord *callRecord = new SNI_CallRecord("Partial Assert right condition true.", this);
						LOGGING(callRecord->SetLogContext(context));
						e.GetSNI_Error()->AddNote(callRecord);
					}
					return e;
				}
			}
			else
			{
				SN::SN_Error e1 = p_left.PartialAssertValue(SN::SN_Bool(false));
				if (e1.IsError())
				{
					if (e1.IsSignificantError())
					{
						SNI_CallRecord *callRecord = new SNI_CallRecord("Partial Assert left condition false.", this);
						LOGGING(callRecord->SetLogContext(context));
						e1.GetSNI_Error()->AddNote(callRecord);
					}
					return e1;
				}
				SN::SN_Error e2 = p_right.PartialAssertValue(SN::SN_Bool(false));
				if (e2.IsSignificantError())
				{
					SNI_CallRecord *callRecord = new SNI_CallRecord("Partial Assert right condition false.", this);
					LOGGING(callRecord->SetLogContext(context));
					e1.GetSNI_Error()->AddNote(callRecord);
				}
				return e2;
			}
		}
		SN::SN_Error e(false, false, "Partial assert failure.");
		if (e.IsSignificantError())
		{
			SNI_CallRecord *callRecord = new SNI_CallRecord("Could not partial assert a sub expression as either true or false.", this);
			LOGGING(callRecord->SetLogContext(context));
			e.GetSNI_Error()->AddNote(callRecord);
		}
		return e;
	}
	size_t SNI_Or::CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const
	{
		if (p_ParamList[PC2_First].IsNullValue())
		{
			return CARDINALITY_MAX;
		}
		else
		{
			SN::SN_Bool left = p_ParamList[PC2_First].GetVariableValue();
			if (!left.IsNull() && left.GetBool())
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
				if (!right.IsNull() && right.GetBool())
				{
					return 1;
				}
			}
		}
		return MultiplyCardinality(p_ParamList[PC2_First].Cardinality(), p_ParamList[PC2_Second].Cardinality());
	}

	size_t SNI_Or::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (p_TotalCalc <= 2)
		{
			SN::SN_Bool result = p_ParamList[PU2_Result].GetVariableValue();
			SN::SN_Bool left = p_ParamList[PU2_First].GetVariableValue();
			SN::SN_Bool right = p_ParamList[PU2_Second].GetVariableValue();
			switch (p_CalcPos)
			{
			case PU2_Result:
				if (!left.IsNull() && left.GetBool())
				{
					return 1;
				}
				if (!right.IsNull() && right.GetBool())
				{
					return 1;
				}
				return MultiplyCardinality(p_ParamList[PU2_First].Cardinality(), p_ParamList[PU2_Second].Cardinality());
				break;
			case PU2_First:
				if (!result.IsNull() && result.GetBool())
				{
					if (!right.IsNull())
					{
						if (right.GetBool())
						{
							return 0;
						}
						else
						{
							return 1;
						}
					}
				}
				return MultiplyCardinality(p_ParamList[PU2_Result].Cardinality(), p_ParamList[PU2_Second].Cardinality());
				break;
			case PU2_Second:
				if (!result.IsNull() && result.GetBool())
				{
					if (!left.IsNull())
					{
						if (left.GetBool())
						{
							return 0;
						}
						else
						{
							return 1;
						}
					}
				}
				return MultiplyCardinality(p_ParamList[PU2_Result].Cardinality(), p_ParamList[PU2_First].Cardinality());
				break;
			}
		}
		return SNI_Binary::CardinalityOfUnify(p_Depth, p_ParamList, p_CalcPos, p_TotalCalc);
	}

	SN::SN_Error SNI_Or::UnifyElement(long p_Depth, SN::SN_Expression *p_ParamList, SNI_World **p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet *worldSet, const SNI_Expression *p_Source) const
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
				p_ParamList[PU2_First].AddValue(skynet::False, p_Depth, p_WorldList, worldSet);
				p_ParamList[PU2_Second].AddValue(skynet::False, p_Depth, p_WorldList, worldSet);
				return skynet::OK;
			}
		}
		}
		return SN::SN_Error(false, false, GetTypeName() + ": Expression not unified. TotalCalc=" + to_string(p_TotalCalc) + " Calcpos=" + to_string(p_CalcPos));
	}
}
