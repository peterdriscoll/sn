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

	size_t SNI_FunctionDef::Cardinality(SN::SN_ExpressionList * p_ParameterList) const
	{
		long depth = (long) p_ParameterList->size();
		SN::SN_Expression *paramList = LoadParametersUnify(p_ParameterList);
		long calcPos = -1;
		long totalCalc = depth;
		for (long j = 0; j < depth; j++)
		{
			if (paramList[j].IsKnownValue())
			{
				totalCalc--;
			}
			else
			{
				calcPos = (long) j;
			}
		}
		return CardinalityOfUnify(depth, paramList, calcPos, totalCalc);
	}

	SN::SN_Expression SNI_FunctionDef::Unify(SN::SN_ExpressionList * p_ParameterList)
	{
		SN::SN_Error  e = true;
		long depth = (long) p_ParameterList->size();
		SN::SN_Expression *paramList = LoadParametersUnify(p_ParameterList);
		SN::SN_Expression *inputList = new SN::SN_Expression[depth];
		bool *output = new bool[depth];
		bool allFound = false;
		long totalCalc = depth;
		for (long j = 0; j < depth; j++)
		{
			if (paramList[j].IsKnownValue())
			{
				inputList[j] = paramList[j].GetVariableValue();
				output[j] = false;
				totalCalc--;
				if (AllowDelay())
				{
					allFound |= inputList[j].GetSNI_Expression()->AllValues();
				}
			}
			else
			{
				inputList[j] = paramList[j];
				output[j] = true;
			}
		}
		long calcPos = -1;
		size_t card = 0;
		size_t maxCard = SN::SN_Manager::GetTopManager().MaxCardinalityCall();
		for (long j = 0; j < depth; j++)
		{
			if (!paramList[j].IsKnownValue() && !paramList[j].IsReferableValue())
			{
				card = CardinalityOfUnify(depth, inputList, (long) j, totalCalc);
				if (paramList[j].IsVariable())
				{
					if (allFound || maxCard < card)
					{
						e = inputList[j].GetSNI_Expression()->SelfAssert();
						if (e.IsError())
						{
							break;
						}
					}
				}
				else
				{
					if (allFound || maxCard < card)
					{
						inputList[j] = new SNI_Variable();
						e = paramList[j].AssertValue(inputList[j]);
						if (e.IsError())
						{
							break;
						}
					}
				}
			}
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
				calcPos = (long) j;
			}
		}
		if (e.GetBool())
		{
			card = CardinalityOfUnify(depth, inputList, calcPos, totalCalc);
			if (maxCard < card)
			{
				if (AllowDelay())
				{
					ReplaceParametersUnify(inputList, p_ParameterList);
					delete[] paramList;
					SNI_DelayedProcessor::GetProcessor()->Delay(SN::SN_FunctionDef(dynamic_cast<SNI_FunctionDef*>(this)), p_ParameterList);
				}
				else
				{
					e = SN::SN_Error(true, true);
				}
			}
			else
			{
				e = ForEachUnify(card, depth, inputList, paramList, output, calcPos, totalCalc);
				ReplaceParametersUnify(inputList, p_ParameterList);
			}
		}
		delete[] inputList;
		delete[] output;
		return e;
	}

	SN::SN_Expression SNI_FunctionDef::Evaluate(long  /* = 0 */) const
	{
		return this;
	}

	SN::SN_Expression SNI_FunctionDef::PartialEvaluate(long  /* = 0 */) const
	{
		return this;
	}

	SN::SN_Expression SNI_FunctionDef::Call(SN::SN_ExpressionList * p_ParameterList, long  /* = 0 */) const
	{
		SN::LogContext context(DisplaySN0() + ".SNI_FunctionDef::Call ( " + DisplayPmExpressionList(p_ParameterList) + " )");

		long depth = (long)p_ParameterList->size();
		SN::SN_Expression *paramList = LoadParametersCall(p_ParameterList);
		SN::SN_Expression *inputList = new SN::SN_Expression[depth];
		for (long j = 0; j < depth; j++)
		{
			inputList[j] = paramList[j].GetVariableValue();
		}
		size_t card = 0;
		size_t maxCard = SN::SN_Manager::GetTopManager().MaxCardinalityCall();
		for (long j = 0; j < depth; j++)
		{
			if (!paramList[j].IsKnownValue() && !paramList[j].IsReferableValue())
			{
				card = CardinalityOfCall(depth, inputList);
				if (maxCard < card)
				{
					inputList[j] = paramList[j].Evaluate();
				}
			}
		}
		card = CardinalityOfCall(depth, inputList);
		SN::SN_Value result;
		if (maxCard < card)
		{
			result = SN::SN_Error(true, true, "Cardinality of "+context.GetSimpleDescription()+", "+to_string(card)+" exceeds max cardinality "+to_string(maxCard));
		}
		else
		{
			result = ForEachCall(card, depth, inputList);
		}
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
			delete[] p_ParamList;
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

	void SNI_FunctionDef::ReplaceParametersCall(SN::SN_Expression * p_ParamList, SN::SN_ExpressionList * p_ParameterList) const
	{
		size_t numParams = p_ParameterList->size();
		for (size_t j = 0; j < numParams; j++)
		{
			(*p_ParameterList)[numParams - j - 1] = p_ParamList[j];
		}
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

	void SNI_FunctionDef::ReplaceParametersUnify(SN::SN_Expression * p_ParamList, SN::SN_ExpressionList * p_ParameterList) const
	{
		size_t numParams = p_ParameterList->size();
		(*p_ParameterList)[0] = p_ParamList[PU2_Result];
		for (size_t j = 1; j < numParams; j++)
		{
			(*p_ParameterList)[numParams - j] = p_ParamList[j];
		}
	}

}
