#include "sni_functiondef.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sn.h"

#include "sni_object.h"
#include "sni_world.h"
#include "sni_worldset.h"
#include "sni_helpers.h"
#include "utility.h"

#include "sn_pch.h"

#pragma warning (disable: 4018)

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

	string SNI_FunctionDef::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
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

	string SNI_FunctionDef::GetDebugId() const
	{
		return GetTypeName();
	}

	long SNI_FunctionDef::GetNumParameters() const
	{
		return 0;
	}

	bool SNI_FunctionDef::AllowDelay() const
	{
		return true;
	}

	bool SNI_FunctionDef::SupportsMultipleOutputs() const
	{
		return false;
	}

	bool SNI_FunctionDef::IgnoreNoConstraint() const
	{
		return false;
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

	size_t SNI_FunctionDef::Hash() const
	{
		return _Hash_representation(this);
	}

	bool SNI_FunctionDef::IsKnownValue(const SN::SN_Expression &p_Param, long) const
	{
		return p_Param.IsKnownValue();
	}

	size_t SNI_FunctionDef::ParamCardinality(const SN::SN_Expression &p_Param, long j) const
	{
		return p_Param.Cardinality();
	}

	bool SNI_FunctionDef::GetBoolResult() const
	{
		return false;
	}

	SN::SN_Expression SNI_FunctionDef::MakeCallExpression(SN::SN_ExpressionList* p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		SN::SN_Expression result = this;
		for (SN::SN_ExpressionList::reverse_iterator paramIt = p_ParameterList->rbegin(); paramIt != p_ParameterList->rend(); paramIt++)
		{
			SN::SN_Expression param = paramIt->DoEvaluate(p_MetaLevel);
			if (!param.IsKnownValue())
			{
				param = paramIt->DoPartialEvaluate(p_MetaLevel);
			}
			result = SN::SN_Function(result, param);
		}
		p_ParameterList->clear();
		return new SNI_IncompleteFunction(result);
	}

	size_t SNI_FunctionDef::Cardinality(SN::SN_Expression * p_ParamList) const
	{
		long depth = GetNumParameters();
		long calcPos = -1;
		long totalCalc = depth;
		for (long j = 0; j < depth; j++)
		{
			if (IsKnownValue(p_ParamList[j], j))
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

	void SNI_FunctionDef::ExpandedBooleanResult(SN::SN_Expression* p_ParamList) const
	{
		size_t maxCard = SNI_Thread::TopManager()->MaxCardinalityCall();
		size_t card = Cardinality(p_ParamList);
		if (GetBoolResult() && maxCard < card && !IsKnownValue(p_ParamList[PU2_Result], PU2_Result))
		{
			if (SNI_Thread::TopManager()->AutoExpandNull())
			{
				SN::SN_ValueSet condition;
				SNI_WorldSet* condition_worldSet = new SNI_WorldSet();
				condition.SetWorldSet(condition_worldSet);
				condition.AddTaggedValue(skynet::True, condition_worldSet->CreateWorld());
				condition.AddTaggedValue(skynet::False, condition_worldSet->CreateWorld());
				condition_worldSet->Complete();
				p_ParamList[PU2_Result].GetSNI_Variable()->SetValue(condition);
				condition.GetSNI_ValueSet()->AssignToVariable(p_ParamList[PU2_Result].GetSNI_Variable());
			}
			else
			{
				SNI_Thread::TopManager()->Breakpoint("Expand boolean result.");
			}
		}
	}

	string SNI_FunctionDef::DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, size_t p_NumParams, SN::SN_Expression * p_ParamList, const SNI_Expression *p_DebugSource) const
	{
		long depth = GetNumParameters()-1;
		string text;
		string delimeter = "";
		long param = SN::ParameterOneId;
		for (long j=0; j < depth; j++)
		{
			SN::SN_Expression &p = p_ParamList[j];
			string del = delimeter;
			if (GetOperator().empty())
			{
				delimeter = SetBreakPoint(";", p_DisplayOptions, p_DebugSource, param++) + " ";
			}
			else
			{
				delimeter = SetBreakPoint(GetOperator(), p_DisplayOptions, p_DebugSource, param++);
			}
			text += del + p.GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions);
		}

		if (GetOperator().empty())
		{
			text = SetBreakPoint(GetTypeName(), p_DisplayOptions, p_DebugSource, SN::LeftId) + " " + text + SetBreakPoint(";", p_DisplayOptions, p_DebugSource, SN::ParameterOneId+depth);
		}

		return Bracket(priority, text, p_DisplayOptions, p_DebugSource);
	}

	void SNI_FunctionDef::AddVariablesForCall(long p_MetaLevel, SNI_VariablePointerMap& p_Map, size_t p_NumParams, SN::SN_Expression* p_ParamList) const
	{
		long depth = GetNumParameters();
		for (long j = 0; j < depth; j++)
		{
			SNI_Expression *p = p_ParamList[j].GetSNI_Expression();
			if (p)
			{
				p->AddVariables(p_MetaLevel, p_Map);
			}
		}
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
			if (IsKnownValue(p_ParamList[j], j))
			{
				text += p_ParamList[j].DisplayValueSN();
			}
			else if (p_ParamList[j].IsVariable())
			{
				SN::SN_Variable v = p_ParamList[j];
				text += v.FrameName();
			}
			else
			{
				text += p_ParamList[j].DisplayValueSN();;
			}
		}
		if (GetOperator().empty())
		{
			text += ")";
		}
		return text;
	}

	SN::SN_Expression SNI_FunctionDef::PrimaryFunctionExpressionOp(const SN::SN_Expression& p_Result) const
	{
		return SN::SN_Error(false, false, GetTypeName() + ": PrimaryFunctionExpressionOp 1 -Function does not have two parameters.");
	}

	SN::SN_Expression SNI_FunctionDef::PrimaryFunctionExpression(const SN::SN_Expression& p_Result) const
	{
		return SN::SN_Error(false, false, GetTypeName() + ": PrimaryFunctionExpression 1 -Function does not have two parameters.");
	}

	SN::SN_Expression SNI_FunctionDef::PrimaryFunctionExpressionOp(const SN::SN_Expression& p_Left, const SN::SN_Expression& p_Right) const
	{
		return SN::SN_Error(false, false, GetTypeName() + ": PrimaryFunctionExpressionOp 2 -Function does not have two parameters.");
	}

	SN::SN_Expression SNI_FunctionDef::PrimaryFunctionExpression(const SN::SN_Expression& p_Left, const SN::SN_Expression& p_Right) const
	{
		return SN::SN_Error(false, false, GetTypeName() + ": PrimaryFunctionExpression 2 -Function does not have two parameters.");
	}

	SN::SN_Expression SNI_FunctionDef::PrimaryFunctionExpressionOp(const SN::SN_Expression& p_Cond, const SN::SN_Expression& p_PositiveCase, const SN::SN_Expression& p_NegativeCase) const
	{
		return SN::SN_Error(false, false, GetTypeName() + ": PrimaryFunctionExpressionOp 3 -Function does not have three parameters.");
	}

	SN::SN_Expression SNI_FunctionDef::PrimaryFunctionExpression(const SN::SN_Expression& p_Cond, const SN::SN_Expression& p_PositiveCase, const SN::SN_Expression& p_NegativeCase) const
	{
		return SN::SN_Error(false, false, GetTypeName() + ": PrimaryFunctionExpression 3 -Function does not have three parameters.");
	}

	SN::SN_Expression * SNI_FunctionDef::LoadParametersCall(SN::SN_ExpressionList * p_ParameterList) const
	{
		size_t numParams = GetNumParameters()-1;
		if (numParams > p_ParameterList->size())
		{
			numParams = p_ParameterList->size();
		}
		SN::SN_Expression *paramList = new SN::SN_Expression[numParams];
		for (size_t j = 0; j < numParams; j++)
		{
			paramList[j] = p_ParameterList->back();
			p_ParameterList->pop_back();
		}
		return paramList;
	}

	SN::SN_Expression* SNI_FunctionDef::LoadParametersUnify(SN::SN_ExpressionList* p_ParameterList) const
	{
		size_t numParams = GetNumParameters();
		size_t numStackParams = p_ParameterList->size();
		if (numStackParams >= numParams)
		{
			SN::SN_Expression* paramList = new SN::SN_Expression[numParams];
			paramList[PU2_Result] = (*p_ParameterList)[numStackParams - numParams];
			for (size_t j = 1; j < numParams; j++)
			{
				paramList[j] = (*p_ParameterList)[numStackParams - j];
			}
			p_ParameterList->resize(numStackParams - numParams);
			return paramList;
		}
		return NULL;
	}

	SN::SN_Expression SNI_FunctionDef::DoEvaluate(long  /* = 0 */) const
	{
		return this;
	}

	SN::SN_Expression SNI_FunctionDef::DoPartialEvaluate(long  /* = 0 */) const
	{
		return this;
	}

	SN::SN_Expression SNI_FunctionDef::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		SN::SN_Expression *paramList = LoadParametersCall(p_ParameterList);
		SN::SN_Expression result = CallArray(paramList, p_MetaLevel, this);
		delete[] paramList;
		return result;
	}

	SN::SN_Expression SNI_FunctionDef::CallArray(SN::SN_Expression* p_ParamList, long p_MetaLevel, const SNI_Expression* p_Source) const
	{
		long depth = GetNumParameters() - 1;
		SN::SN_Expression *inputList = new SN::SN_Expression[depth];
		SNI_Frame::Push(this, NULL);
		SNI_Frame* topFrame = SNI_Frame::Top();

		SNI_DisplayOptions displayOptions(doTextOnly);
		LOG(WriteHeading(SN::DebugLevel, GetTypeName() + ": Start " + DisplayUnify(depth, p_ParamList, p_Source)));

		if (0 < p_MetaLevel)
		{
			SN::SN_Expression function(this);
			for (long j = 0; j < depth; j++)
			{
				function = SN::SN_Function(function, p_ParamList[j].DoEvaluate(p_MetaLevel));
			}
			return function;
		}

		for (long j = 0; j < depth; j++)
		{
			topFrame->CreateParameter(j+1, p_ParamList[j]);
			inputList[j] = p_ParamList[j].GetVariableValue();
		}
		Breakpoint(SN::DebugStop, SN::LeftId, GetTypeName(), "Call", p_Source, SN::CallPoint);
		size_t card = 0;
		size_t maxCard = SNI_Thread::TopManager()->MaxCardinalityCall();
		for (long j = 0; j < depth; j++)
		{
			if (!IsKnownValue(p_ParamList[j], j) && !p_ParamList[j].IsReferableValue())
			{
				card = CardinalityOfCall(depth, inputList);
				if (maxCard < card)
				{
					inputList[j] = p_ParamList[j].DoEvaluate(p_MetaLevel);
					SNI_Variable* v = topFrame->GetVariable(j+1);
					v->SetValue(inputList[j]);
				}
			}
			Breakpoint(SN::DetailStop, (SN::BreakId)(SN::ParameterOneId + j), GetTypeName(), "Call parameter:" + to_string(j), p_Source, SN::ParameterPoint);
		}

		card = CardinalityOfCall(depth, inputList);
		SN::SN_Value result;
		if (maxCard < card)
		{
			result = SN::SN_Error(false, true, "Cardinality " + to_string(card) + " exceeds max cardinality " + to_string(maxCard));
		}
		else
		{
			result = ForEachCall(card, depth, inputList);
		}

		SNI_Variable* resultVar = topFrame->GetResult();
		resultVar->SetValue(result);
		Breakpoint(SN::DebugStop, SN::RightId, GetTypeName(), "Result calculated", p_Source, SN::CallPoint);

		delete[] inputList;
		SNI_Expression* r = result.GetSNI_Expression();
		if (r)
		{
			r->Validate();
		}
		SNI_Frame::Pop();
		return result;
	}

	SN::SN_Error SNI_FunctionDef::DoAssert()
	{
		return Unify(NULL).GetError();
	}

	SN::SN_Error SNI_FunctionDef::DoPartialAssert()
	{
		return PartialUnify(NULL, SN::SN_Bool(true));
	}

	SN::SN_Expression SNI_FunctionDef::Unify(SN::SN_ExpressionList* p_ParameterList)
	{
		if (SNI_Thread::TopManager()->TailCallOptimization())
		{
			return this; // Let the AssertValue lower down the stack unify it.
		}
		SN::SN_Expression* param_List = LoadParametersUnify(p_ParameterList);
		SN::SN_Expression function = UnifyArray(param_List, this).GetSNI_Expression();
		delete[] param_List;
		return function;
	}


	SN::SN_Expression SNI_FunctionDef::UnifyArray(SN::SN_Expression * p_ParamList, const SNI_Expression *p_Source)
	{
		SN::SN_Error e = skynet::OK;
		long depth = GetNumParameters();
		SN::SN_Expression *inputList = new SN::SN_Expression[depth];
		bool *output = new bool[depth];
		bool allFound = false;
		long totalCalc = depth;
		SNI_Frame::Push(this, NULL);
		SNI_Frame *topFrame = SNI_Frame::Top();

		SNI_DisplayOptions displayOptions(doTextOnly);
		LOG(WriteHeading(SN::DebugLevel, GetTypeName() + ": Start " + DisplayUnify(depth, p_ParamList, p_Source)));
		for (long j = 0; j < depth; j++)
		{
			if (j == 0)
			{
				SNI_Variable *v =topFrame->GetResult();
				v->SetValue(p_ParamList[j]);
			}
			else
			{
				topFrame->CreateParameter(j, p_ParamList[j]);
			}
			p_ParamList[j].Simplify();
			if (IsKnownValue(p_ParamList[j], j))
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
		size_t maxCard = SNI_Thread::TopManager()->MaxCardinalityCall();
		size_t card = -1;
		Breakpoint(SN::DebugStop, SN::LeftId, GetTypeName(), "Unify before checking parameter cardinality", p_Source, SN::CallPoint);
		for (long j = 0; j < depth; j++)
		{
			if (!IsKnownValue(p_ParamList[j], j))
			{
				card = CardinalityOfUnify(depth, inputList, (long)j, totalCalc);
				LOG(WriteLine(SN::DebugLevel, "Parameter " + to_string(j) + " " + "Card: " + to_string(card) + ": " + inputList[j].DisplaySN() + " / " + inputList[j].GetSafeValue().DisplaySN()));
				topFrame->RegisterCardinality(card);
				if (p_ParamList[j].IsVariable())
				{
					if (allFound || maxCard < card)
					{
						LOG(WriteLine(SN::DebugLevel, "Self Assert variable start " + to_string(j) ));
						e = inputList[j].GetSNI_Expression()->SelfAssert();
						if (e.IsError())
						{
							break;
						}
						LOG(WriteLine(SN::DebugLevel, "Self Assert variable end " + to_string(j) + ": " + inputList[j].DisplaySN() + " / " + inputList[j].GetSafeValue().DisplaySN()));
					}
				}
				else if (inputList[j].IsComplete())
				{
					if (allFound || maxCard < card)
					{
						LOG(WriteLine(SN::DebugLevel, "Assert start " + to_string(j) + ": " + inputList[j].DisplayValueSN()));
						SNI_Variable *v = topFrame->GetVariable(j);
						v->SetValue(SN::SN_Expression());
						e = inputList[j].AssertValue(v);
						if (e.IsError())
						{
							break;
						}
						else if (v->GetSafeValue()!=NULL)
						{
							inputList[j] = v;
							p_ParamList[j] = v;
						}
						LOG(WriteLine(SN::DebugLevel, "Assert value " + to_string(j) + ": " + inputList[j].DisplaySN() + "/" + inputList[j].DisplayValueSN() + (IsKnownValue(inputList[j], j)?" known":" unknown")));
					}
				}
			}
			if (IsKnownValue(inputList[j], j))
			{
				inputList[j] = inputList[j].GetVariableValue();
				if (output[j])
				{
					output[j] = false;
					totalCalc--;
					LOG(WriteLine(SN::DebugLevel, "Value " + to_string(j) + " found. Total calc " + to_string(totalCalc)));
				}
			}
			else
			{
				calcPos = (long)j;
				LOG(WriteLine(SN::DebugLevel, "Value " + to_string(j) + " is to be calculated."));
			}
			if (j != 0 && j != depth-1)
			{
				Breakpoint(SN::DetailStop, (SN::BreakId)(SN::ParameterOneId + j), GetTypeName(), "Unify parameter " + to_string(j) + " card " + to_string(card), p_Source, SN::CallPoint);
			}
		}
		LOG(WriteLine(SN::DebugLevel, GetLogDescription(p_ParamList)));
		if (e.GetBool())
		{
			card = CardinalityOfUnify(depth, inputList, calcPos, totalCalc);
			topFrame->RegisterCardinality(card);
			Breakpoint(SN::DetailStop, (SN::BreakId)(SN::ParameterOneId + depth - 1), GetTypeName(), "Unify parameter " + to_string(depth-1) + " card " + to_string(card), p_Source, SN::CallPoint);
			if (0 < card)
			{
				if (GetBoolResult() && maxCard < card  && !IsKnownValue(inputList[PU2_Result], PU2_Result))
				{
					SN::SN_ValueSet condition;
					SNI_WorldSet* condition_worldSet = new SNI_WorldSet();
					condition.SetWorldSet(condition_worldSet);
					condition.AddTaggedValue(skynet::True, condition_worldSet->CreateWorld());
					condition.AddTaggedValue(skynet::False, condition_worldSet->CreateWorld());
					condition_worldSet->Complete();
					inputList[PU2_Result] = condition;
					p_ParamList[PU2_Result].GetSNI_Variable()->SetValue(condition);
					condition.GetSNI_ValueSet()->AssignToVariable(p_ParamList[PU2_Result].GetSNI_Variable());

					output[PU2_Result] = false;
					totalCalc--;
					card = CardinalityOfUnify(depth, inputList, calcPos, totalCalc);
				}
				LOG(WriteLine(SN::DebugLevel, "Cardinality " + to_string(card) + " with total fields " + to_string(totalCalc)));
				if (maxCard < card)
				{
					if (AllowDelay())
					{
						LOG(WriteLine(SN::DebugLevel, "Delayed Call " + GetLogDescription(inputList)));
						SNI_Thread::GetThread()->GetProcessor()->Delay(SN::SN_FunctionDef(dynamic_cast<SNI_FunctionDef*>(this)), GetNumParameters(), inputList, p_Source);
					}
					else
					{
						e = SN::SN_Error(true, true, GetTypeName()+": Max cardinality exceeded, and delayed call not allowed.");
					}
				}
				else
				{
					e = ForEachUnify(card, depth, inputList, p_ParamList, output, calcPos, totalCalc, p_Source);
					LOG(WriteLine(SN::DebugLevel, GetLogDescription(inputList)));

					for (long j = 0; j < depth; j++)
					{
						if (output[j])
						{
							LOG(WriteLine(SN::DebugLevel, "Parameter " + to_string(j) + " calculated: " + p_ParamList[j].DisplaySN()));
						}
					}
				}
				Breakpoint(SN::DebugStop, SN::RightId, GetTypeName(), "Unify after calculation", p_Source, SN::CallPoint);
			}
			else if (!IgnoreNoConstraint())
			{
				Breakpoint(SN::WarningStop, SN::RightId, GetTypeName(), "Unify after calculation", p_Source, SN::WarningPoint);
			}
		}
		LOG(WriteHeading(SN::DebugLevel, GetTypeName() + ": End " + DisplayUnify(depth, p_ParamList, p_Source)));
		SNI_Frame::Pop();
		delete [] output;
		delete[] inputList;
		return e;
	}

	size_t SNI_FunctionDef::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		return 1;
	}

	SN::SN_Error SNI_FunctionDef::UnifyElement(long p_Depth, SN::SN_Expression * p_InputList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet * worldSet, const SNI_Expression *p_Source) const
	{
		return SN::SN_Error(false, false, GetTypeName() + ": Unify element not implemented.");
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

	SN::SN_Error SNI_FunctionDef::ForEachUnify(size_t p_Card, long p_Depth, SN::SN_Expression * p_InputList, SN::SN_Expression * p_ParamList, bool * p_Output, long p_CalcPos, long p_TotalCalc, const SNI_Expression *p_Source) const
	{
		bool single = p_Card == 1;
		for (long j = 0; j < p_Depth; j++)
		{
			if (p_Output[j])
			{
				if (j != p_CalcPos)
				{
					if (!SupportsMultipleOutputs())
					{
						Breakpoint(SN::WarningStop, SN::MultipleOutputsId, GetTypeName(), "Multiple outputs" + to_string(j), p_Source, SN::CallPoint);
					}
				}
			}
			else if (!MergeValueSets(p_InputList) && p_InputList[j].IsValueSet())
			{
				single = false;
			}
		}
		if (single)
		{
			if (SNI_Thread::TopManager()->GetDirectPassType() == skynet::ReturnValueToVariable || !AllowDelay())
			{
				for (long j = 0; j < p_Depth; j++)
				{
					if (p_Output[j] && !p_ParamList[j].IsValueHolder() && !p_ParamList[j].IsReferableValue())
					{
						p_InputList[j] = new SNI_Variable;
						SNI_Frame *topFrame = SNI_Frame::Top();
						topFrame->GetVariable(j)->SetValue(p_InputList[j]);
					}
				}
			}
			SN::SN_Error e = UnifyElement(p_Depth, p_InputList, NULL, p_CalcPos, p_TotalCalc, NULL, p_Source);
			if (SNI_Thread::TopManager()->GetDirectPassType() == skynet::ReturnValueToVariable || !AllowDelay())
			{
				for (size_t j = 0; j < p_Depth; j++)
				{
					if (p_Output[j])
					{
						if (!p_ParamList[j].IsValueHolder() && !p_ParamList[j].IsValueHolder() && !p_ParamList[j].IsReferableValue())
						{
							SN::SN_Value simple = p_InputList[j].SimplifyValue();
							SNI_Frame *topFrame = SNI_Frame::Top();
							topFrame->GetVariable(j)->SetValue(simple);
							SN::SN_Error  e = p_ParamList[j].GetSNI_Expression()->AssertValue(simple);
							if (e.IsError())
							{
								return e;
							}
							p_InputList[j].GetSNI_Expression()->Complete();
							p_ParamList[j] = p_InputList[j];
						}
						else
						{
							p_InputList[j].GetSNI_Expression()->Complete();
							p_ParamList[j] = p_InputList[j];
						}
					}
				}
			}
			else
			{
				for (long j = 0; j < p_Depth; j++)
				{
					if (p_Output[j])
					{
						p_InputList[j].GetSNI_Expression()->Complete();
						p_ParamList[j] = p_InputList[j];
					}
				}
			}
			return e;
		}
		else
		{
			for (long j = 0; j < p_Depth; j++)
			{
				if (p_Output[j])
				{
					p_InputList[j] = new SNI_ValueSet;
				}
			}
			
			SN::SN_Error e =skynet::OK;
			{
				SNI_CartUnify cart(this, p_Depth, p_InputList, p_ParamList, p_Output, p_CalcPos, p_TotalCalc, p_Source);
				e = cart.ForEachUnify();
			}

			for (size_t j = 0; j < p_Depth; j++)
			{
				if (p_Output[j])
				{
					SN::SN_Value simple = p_InputList[j].SimplifyValue();
					p_InputList[j].GetSNI_Expression()->Complete();
					SN::SN_Error  e = p_ParamList[j].AssertValue(simple);
					p_ParamList[j].GetSNI_Expression()->Complete();
					if (e.IsError())
					{
						return e;
					}
				}
				else
				{
					p_ParamList[j] = p_InputList[j].SimplifyValue();
					p_ParamList[j].GetSNI_Expression()->Complete();
				}
			}
			return e;
		}
	}

	bool SNI_FunctionDef::MergeValueSets(SN::SN_Expression* p_ParamList) const
	{
		return false;
	}
}
