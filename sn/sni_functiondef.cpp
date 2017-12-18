#include "sni_functiondef.h"

#include "logcontext.h"
#include "sn.h"

#include "sni_object.h"
#include "sni_world.h"
#include "sni_worldset.h"
#include "sni_helpers.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_FunctionDef::SNI_FunctionDef()
	{

	}

	SNI_FunctionDef::~SNI_FunctionDef()
	{

	}

	string SNI_FunctionDef::GetTypeName() const
	{
		return "FunctionDef";
	}

	string SNI_FunctionDef::DisplayCpp() const
	{
		return GetTypeName();
	}

	string SNI_FunctionDef::DisplaySN(long /*priority*/, SNI_VariablePointerList & /*p_DisplayVariableList*/) const
	{
		return GetTypeName();
	}

	long SNI_FunctionDef::GetPriority() const
	{
		return 100;
	}

	string SNI_FunctionDef::GetOperator() const
	{
		return "";
	}

	long SNI_FunctionDef::GetNumParameters() const
	{
		return 0;
	}

	bool SNI_FunctionDef::AllowDelay() const
	{
		return true;
	}

	bool SNI_FunctionDef::Equivalent(SNI_Object * p_Other) const
	{
		if (dynamic_cast<SNI_FunctionDef *>(p_Other))
		{
			SNI_FunctionDef * l_function = dynamic_cast<SNI_FunctionDef *>(p_Other);
			return this == l_function;
		}
		return false;
	}

	size_t SNI_FunctionDef::Cardinality(SN::SN_Expression * p_ParamList) const
	{
		long depth = GetNumParameters();
		long calcPos = -1;
		long totalCalc = depth;
		for (long j = 0; j < depth; j++)
		{
			if (p_ParamList[j].IsKnownValue())
			{
				totalCalc--;
			}
			else
			{
				calcPos = (long) j;
			}
		}
		return CardinalityOfUnify(depth, p_ParamList, calcPos, totalCalc);
	}

	string SNI_FunctionDef::DisplayCall(long priority, SNI_VariablePointerList & p_DisplayVariableList, SN::SN_ExpressionList * p_ParameterList) const
	{
		long depth = GetNumParameters()-1;
		string text;
		string delimeter = "";
		for (SN::SN_Expression &p : *p_ParameterList)
		{
			text += delimeter;
			if (GetOperator().empty())
			{
				text += GetTypeName() + "(";
				delimeter = ", ";
			}
			else
			{
				delimeter = GetOperator();
			}
			text += p.DisplayValueSN();
		}
		if (GetOperator().empty())
		{
			text += ")";
		}
		return text;
	}

	string SNI_FunctionDef::GetLogDescription(SN::SN_Expression * p_ParamList) const
	{
		long depth = GetNumParameters();
		string text;
		string delimeter = " == ";
		for (long j = 0; j < depth; j++)
		{
			if (j)
			{
				text += delimeter;
				if (GetOperator().empty())
				{
					text += GetTypeName() + "(";
					delimeter = ", ";
				}
				else
				{
					delimeter = GetOperator();
				}
			}
			if (p_ParamList[j].IsKnownValue())
			{
				text += p_ParamList[j].DisplayValueSN();
			}
			else if (p_ParamList[j].IsVariable())
			{
				SN::SN_Variable v = p_ParamList[j];
				text += v.GetName();
			}
			else
			{
				text += "expression";
			}
		}
		if (GetOperator().empty())
		{
			text += ")";
		}
		return text;
	}

	SN::SN_Expression SNI_FunctionDef::Evaluate(long  /* = 0 */) const
	{
		return this;
	}

	SN::SN_Expression SNI_FunctionDef::PartialEvaluate(long  /* = 0 */) const
	{
		return this;
	}

	SN::SN_Expression SNI_FunctionDef::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		SN::SN_Expression *paramList = LoadParametersCall(p_ParameterList);
		return CallArray(paramList, p_MetaLevel);
	}

	SN::SN_Expression SNI_FunctionDef::CallArray(SN::SN_Expression * p_ParamList, long p_MetaLevel /* = 0 */) const
	{
		SNI_Manager::GetTopManager()->DebugCommand(SN::CallPoint, GetTypeName()+".Call before cardinality check");
		long depth = GetNumParameters() - 1;
		SN::SN_Expression *inputList = new SN::SN_Expression[depth];
		for (long j = 0; j < depth; j++)
		{
			inputList[j] = p_ParamList[j].GetVariableValue();
		}
		size_t card = 0;
		size_t maxCard = SN::SN_Manager::GetTopManager().MaxCardinalityCall();
		for (long j = 0; j < depth; j++)
		{
			if (!p_ParamList[j].IsKnownValue() && !p_ParamList[j].IsReferableValue())
			{
				card = CardinalityOfCall(depth, inputList);
				if (maxCard < card)
				{
					inputList[j] = p_ParamList[j].Evaluate();
				}
				SNI_Manager::GetTopManager()->DebugCommand(SN::ParameterPoint, GetTypeName() + ".Call parameter:" + inputList[j].DisplayValueSN());
			}
			else
			{
				SNI_Manager::GetTopManager()->DebugCommand(SN::ParameterPoint, GetTypeName() + ".Call parameter:" + inputList[j].DisplayValueSN());
			}
		}
		card = CardinalityOfCall(depth, inputList);
		SN::SN_Value result;
		SNI_Manager::GetTopManager()->DebugCommand(SN::ParameterPoint, GetTypeName() + ".Call before calculation");
		if (maxCard < card)
		{
			result = SN::SN_Error(true, true, "Cardinality " + to_string(card) + " exceeds max cardinality " + to_string(maxCard));
		}
		else
		{
			result = ForEachCall(card, depth, inputList);
		}
		SNI_Manager::GetTopManager()->DebugCommand(SN::ParameterPoint, GetTypeName() + ".Call after calculation");
		delete[] inputList;
		return result;
	}

	SN::SN_Expression SNI_FunctionDef::PartialCall(SN::SN_ExpressionList * p_ParameterList, long  /* = 0 */) const
	{
		SN::LogContext context(DisplaySN0() + ".SNI_FunctionDef::PartialCall ( " + DisplayPmExpressionList(p_ParameterList) + " )");

		return skynet::Null;
	}

	SN::SN_Error SNI_FunctionDef::Assert()
	{
		return Unify(NULL);
	}

	SN::SN_Error SNI_FunctionDef::PartialAssert()
	{
		return PartialUnify(NULL, SN::SN_Bool(true));
	}

	SN::SN_Expression SNI_FunctionDef::UnifyArray(SN::SN_Expression * p_ParamList)
	{
		SN::SN_Error  e = true;
		long depth = GetNumParameters();
		SN::SN_Expression *inputList = new SN::SN_Expression[depth];
		bool *output = new bool[depth];
		bool allFound = false;
		long totalCalc = depth;
		for (long j = 0; j < depth; j++)
		{
			if (p_ParamList[j].IsKnownValue())
			{
				inputList[j] = p_ParamList[j].GetVariableValue();
				output[j] = false;
				totalCalc--;
				if (AllowDelay())
				{
					allFound |= inputList[j].GetSNI_Expression()->AllValues();
				}
			}
			else
			{
				inputList[j] = p_ParamList[j];
				output[j] = true;
			}
		}
		long calcPos = -1;
		size_t card = 0;
		size_t maxCard = SN::SN_Manager::GetTopManager().MaxCardinalityCall();
		for (long j = 0; j < depth; j++)
		{
			SN::SN_Variable v = inputList[j];
			if (!p_ParamList[j].IsKnownValue() && !p_ParamList[j].IsReferableValue())
			{
				card = CardinalityOfUnify(depth, inputList, (long)j, totalCalc);
				if (p_ParamList[j].IsVariable())
				{
					if (allFound || maxCard < card)
					{
						LOG(WriteLine(SN::DebugLevel, "Self Assert " + to_string(j) + ": " + v.GetName()));
						e = inputList[j].GetSNI_Expression()->SelfAssert();
						if (e.IsError())
						{
							break;
						}
					}
					else
					{
						LOG(WriteLine(SN::DebugLevel, "Parameter " + to_string(j) + ": " + v.GetName()));
					}
				}
				else
				{
					LOG(WriteLine(SN::DebugLevel, "Parameter " + to_string(j) + ": " + inputList[j].DisplayValueSN()));
					if (allFound || maxCard < card)
					{
						inputList[j] = new SNI_Variable();
						e = p_ParamList[j].AssertValue(inputList[j]);
						LOG(WriteLine(SN::DebugLevel, "Assert " + to_string(j) + ": " + inputList[j].DisplayValueSN()));
						if (e.IsError())
						{
							break;
						}
					}
				}
			}
			else
			{
				if (inputList[j].IsVariable())
				{
					LOG(WriteLine(SN::DebugLevel, "Parameter " + to_string(j) + ": " + v.GetName()));
				}
				else
				{
					LOG(WriteLine(SN::DebugLevel, "Parameter " + to_string(j) + ": " + inputList[j].DisplayValueSN()));
				}
			}
			SNI_Manager::GetTopManager()->DebugCommand(SN::ParameterPoint, GetTypeName() + ".Unify parameter: " + inputList[j].DisplayValueSN());
			if (inputList[j].IsKnownValue())
			{
				inputList[j] = inputList[j].GetVariableValue();
				if (output[j])
				{
					output[j] = false;
					totalCalc--;
				}
			}
			else
			{
				calcPos = (long)j;
			}
		}
		LOG(WriteLine(SN::DebugLevel, GetLogDescription(p_ParamList)));
		LOG(WriteFrame(SN::DebugLevel));
		if (e.GetBool())
		{
			card = CardinalityOfUnify(depth, inputList, calcPos, totalCalc);
			LOG(WriteLine(SN::DebugLevel, "Cardinality " + to_string(card)+ " with total fields "+to_string(totalCalc)));
			if (totalCalc)
			{
				LOG(WriteLine(SN::DebugLevel, inputList[calcPos].DisplayValueSN()));
			}
			SNI_Manager::GetTopManager()->DebugCommand(SN::CallPoint, GetTypeName() + ".Unify before calculation");
			if (maxCard < card)
			{
				if (AllowDelay())
				{
					LOG(WriteLine(SN::DebugLevel, "Delayed Call " + GetLogDescription(inputList)));
					SNI_DelayedProcessor::GetProcessor()->Delay(SN::SN_FunctionDef(dynamic_cast<SNI_FunctionDef*>(this)), inputList);
				}
				else
				{
					e = SN::SN_Error(true, true);
				}
			}
			else
			{
				e = ForEachUnify(card, depth, inputList, p_ParamList, output, calcPos, totalCalc);
				LOG(WriteLine(SN::DebugLevel, GetLogDescription(inputList)));
				LOG(WriteFrame(SN::DebugLevel));
				SNI_Manager::GetTopManager()->DebugCommand(SN::CallPoint, GetTypeName() + ".Unify after calculation");
			}
		}
		for (long j = 0; j < depth; j++)
		{
			p_ParamList[j] = inputList[j];
		}
		delete[] output;
		return e;
	}

	SN::SN_Error SNI_FunctionDef::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result)
	{
		SN::LogContext context(DisplaySN0() + ".SNI_FunctionDef::PartialUnify ( " + DisplayPmParameterList(p_ParameterList) + " = " + p_Result.DisplaySN() + " )");

		return false;
	}

	size_t SNI_FunctionDef::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		return 1;
	}

	SN::SN_Error SNI_FunctionDef::UnifyElement(long p_Depth, SN::SN_Expression * p_InputList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet * worldSet) const
	{
		return false;
	}

	size_t SNI_FunctionDef::CardinalityOfCall(long p_NumParams, SN::SN_Expression * p_ParameterList) const
	{
		return 1;
	}

	SN::SN_Value SNI_FunctionDef::CallElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, SN::SN_ValueSet p_Result) const
	{
		return false;
	}

	/*static*/size_t SNI_FunctionDef::MultiplyCardinality(size_t p_Left, size_t p_Right)
	{
		//size_t left = (p_MaxCardinality < p_Left) ? p_MaxCardinality : p_Left;
		//size_t right = (p_MaxCardinality < p_Right) ? p_MaxCardinality : p_Right;
		if (p_Left < CARDINALITY_SQUARE_ROOT_MAX && p_Right < CARDINALITY_SQUARE_ROOT_MAX)
		{
			return p_Left * p_Right;
		}
		return CARDINALITY_MAX;
	}

	SN::SN_Value SNI_FunctionDef::ForEachCall(size_t p_Card, long p_Depth, SN::SN_Expression * p_InputList) const
	{
		if (p_Card == 1)
		{
			return CallElement(p_Depth, p_InputList, NULL, SN::SN_Value());
		}
		else
		{
			SNI_CartCall cart(this, p_Depth, p_InputList);
			return cart.ForEach();
		}
	}

	SN::SN_Error SNI_FunctionDef::ForEachUnify(size_t p_Card, long p_Depth, SN::SN_Expression * p_InputList, SN::SN_Expression * p_ParamList, bool * p_Output, long p_CalcPos, long p_TotalCalc) const
	{
		if (p_Card == 1)
		{
			if (!AllowDelay())
			{
				for (long j = 0; j < p_Depth; j++)
				{
					if (p_Output[j] && !p_ParamList[j].IsVariable())
					{
						p_InputList[j] = new SNI_Variable;
					}
				}
			}
			SN::SN_Error e = UnifyElement(p_Depth, p_InputList, NULL, p_CalcPos, p_TotalCalc, NULL);
			if (!AllowDelay())
			{
				for (size_t j = 0; j < p_Depth; j++)
				{
					if (p_Output[j] && !p_ParamList[j].IsVariable())
					{
						SN::SN_Value simple = p_InputList[j].SimplifyValue();
						SN::SN_Error  e = p_ParamList[j].AssertValue(simple);
						if (e.IsError())
						{
							return e;
						}
					}
				}
			}
			return e;
		}
		else
		{
			SNI_CartUnify cart(this, p_Depth, p_InputList, p_ParamList, p_Output, p_CalcPos, p_TotalCalc);
			return cart.ForEachUnify();
		}
	}

	SN::SN_Expression * SNI_FunctionDef::LoadParametersCall(SN::SN_ExpressionList * p_ParameterList) const
	{
		size_t numParams = p_ParameterList->size();
		SN::SN_Expression *paramList = new SN::SN_Expression[numParams];
		for (size_t j = 0; j < numParams; j++)
		{
			paramList[j] = (*p_ParameterList)[numParams - j - 1];
		}
		return paramList;
	}

	SN::SN_Expression * SNI_FunctionDef::LoadParametersUnify(SN::SN_ExpressionList * p_ParameterList) const
	{
		size_t numParams = p_ParameterList->size();
		SN::SN_Expression *paramList = new SN::SN_Expression[numParams];
		paramList[PU2_Result] = (*p_ParameterList)[0];
		for (size_t j = 1; j < numParams; j++)
		{
			paramList[j] = (*p_ParameterList)[numParams - j];
		}
		return paramList;
	}
}
