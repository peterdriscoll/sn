#include "sni_variable.h"

#include "logcontext.h"
#include "sn_expression.h"
#include "sn_value.h"
#include "sn_valueset.h"
#include "sn_lambda.h"
#include "sn_error.h"

#include "sni_delayedprocessor.h"
#include "sni_delayedcall.h"
#include "sni_replacement.h"
#include "sni_valueset.h"
#include "sni_world.h"
#include "sni_worldset.h"
#include "sni_error.h"
#include "sni_null.h"
#include "sni_helpers.h"
#include "sni_cartesian.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	long m_MaxTempNum = 0;
	SNI_Variable::SNI_Variable()
		: m_Value(NULL)
		, m_Frame(NULL)
		, m_Requested(false)
	{
		SetName("_t" + to_string(++m_MaxTempNum));
	}

	SNI_Variable::SNI_Variable(const string &p_Name)
		: m_Frame(NULL)
		, m_Value(NULL)
		, m_Requested(false)
	{
		SetName(p_Name);
	}

	SNI_Variable::SNI_Variable(const SN::SN_Expression &p_Other)
		: m_Value(p_Other.GetSNI_Expression())
		, m_Frame(NULL)
		, m_Requested(false)
	{
	}

	SNI_Variable::~SNI_Variable()
	{

	}

	void SNI_Variable::PromoteMembers()
	{
	}

	bool SNI_Variable::IsComplete() const
	{
		if (m_Value)
		{
			return m_Value->IsComplete();
		}
		return true;
	}

	void SNI_Variable::Complete()
	{
		if (m_Value)
		{
			m_Value->Complete();
		}
	}

	bool SNI_Variable::IsRequested() const
	{
		return m_Requested;
	}

	bool SNI_Variable::IsNullValue() const
	{
		return !m_Value || m_Value->IsNull();
	}

	bool SNI_Variable::IsKnownValue() const
	{
		return m_Value && m_Value->IsKnownValue();
	}

	bool SNI_Variable::IsVariable() const
	{
		return true;
	}

	bool SNI_Variable::IsReferableValue() const
	{
		return m_Value && m_Value->IsReferableValue();
	}

	void SNI_Variable::Request()
	{
		m_Requested = true;
		SNI_DelayedCall *call = dynamic_cast<SNI_DelayedCall *>(m_Value);
		if (call)
		{
			SNI_DelayedProcessor::GetProcessor()->Request(call);
		}
	}

	string SNI_Variable::FrameName() const
	{
		if (m_Frame)
		{	// Main thread
			return GetName() + m_Frame->NameSuffix();
		}
		return GetName();
	}

	SNI_Expression * SNI_Variable::GetValue(bool p_Request) const
	{
		if (p_Request)
		{
			const_cast<SNI_Variable*>(this)->Request();
		}
		if (m_Value)
		{
			const_cast<SNI_Variable*>(this)->Simplify();
			return m_Value;
		}
		return skynet::Null.GetSNI_Expression();
	}

	void SNI_Variable::Simplify()
	{
		if (IsKnownValue() && !SNI_World::ContextWorld())
		{
			m_Value = m_Value->SimplifyValue().GetSNI_Expression();
		}
	}

	SN::SN_Expression SNI_Variable::SimplifyValue()
	{
		Simplify();
		return m_Value;
	}


	SN::SN_Expression SNI_Variable::GetVariableValue(bool p_IfComplete)
	{
		return m_Value;
	}

	SN::SN_Error SNI_Variable::AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World ** p_WorldList, SNI_WorldSet *p_WorldSet)
	{
		if (p_Value.IsError())
		{
			return p_Value;
		}
		if (p_Value.IsVariable())
		{
			FORCE_ASSERTM("Variable not expected.");
		}
		if (SNI_World::ContextWorld())
		{
			if (!m_Value || m_Value->IsNull())
			{
				m_Value = new SNI_ValueSet();
			}
			return m_Value->AddValue(p_Value, p_NumWorlds, p_WorldList, p_WorldSet);
		}
		else
		{
			if (p_WorldList)
			{
				string worldString = DisplayWorlds(p_NumWorlds, p_WorldList);
				SN::LogContext context("SNI_Variable::AddValue (" + SN::SN_Expression(this).DisplayValueSN() + " := " + p_Value.DisplayValueSN() + " worlds " + worldString + " set " + DisplayWorldSet(p_WorldSet) + " )");
				bool exists = false;
				SNI_WorldSet *l_WorldSet = p_WorldSet;
				if (!l_WorldSet)
				{
					l_WorldSet = GetWorldSet();
				}
				SNI_World *world = l_WorldSet->JoinWorldsArray(ManualAddWorld, AlwaysCreateWorld, exists, p_NumWorlds, p_WorldList);
				if (exists)
				{
					context.LogExpression(world->DisplaySN(), p_Value);
					SN::SN_Error e = AssertValue(p_Value);
					if (e.GetBool())
					{
						l_WorldSet->AddToSetList(world);
					}
					return e;
				}
				else
				{
					context.LogText("Fail", "JoinWorlds failed on " + worldString);
					return SN::SN_Error("SNI_Variable::AddValue: JoinWorlds failed on " + worldString);
				}

			}
			else
			{
				return AssertValue(p_Value);
			}
		}
	}

	string SNI_Variable::GetTypeName() const
	{
		return "Variable";
	}

	string SNI_Variable::DisplayCpp() const
	{
		string value;
		if (m_Value)
		{
			//		value = m_Value->DisplayCpp();
		}
		return "sn_Variable(" + FrameName() + ")";
	}

	string SNI_Variable::DisplaySN(long p_Priority, SNI_VariableConstPointerList &p_DisplayVariableList) const
	{
		string value;
		if (!m_Value)
		{
			value = "";
		}
		else if (FindVariable(p_DisplayVariableList))
		{
			value = "";
		}
		else
		{
			p_DisplayVariableList.insert(p_DisplayVariableList.begin(), this);
			value = "[" + m_Value->DisplayValueSN(GetPriority(), p_DisplayVariableList) + "]";
			p_DisplayVariableList.erase(p_DisplayVariableList.begin());
		}
		return Bracket(p_Priority, FrameName() /* + value */);
	}

	string SNI_Variable::DisplayCall(long p_Priority, SNI_VariableConstPointerList & p_DisplayVariableList, SN::SN_ExpressionList * p_ParameterList) const
	{
		string text;
		string delimeter;
		for (SN::SN_Expression &p : *p_ParameterList)
		{
			string del = delimeter;
			delimeter = " ";
			text = p.GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayVariableList) + del + text;
		}
		return DisplaySN(p_Priority, p_DisplayVariableList) + " " + text;
	}

	long SNI_Variable::GetPriority() const
	{
		return 100;
	}

	bool SNI_Variable::GetBool() const
	{
		if (m_Value)
		{
			return m_Value->GetBool();
		}
		return false;
	}

	string SNI_Variable::GetString() const
	{
		if (m_Value)
		{
			return GetValue()->GetString();
		}
		return "";
	}

	size_t SNI_Variable::Count() const
	{
		if (m_Value)
		{
			return m_Value->Count();
		}
		return 0;
	}

	size_t SNI_Variable::Length() const
	{
		if (m_Value)
		{
			return m_Value->Length();
		}
		return 0;
	}

	SNI_Frame * SNI_Variable::GetFrame() const
	{
		return m_Frame;
	}

	void SNI_Variable::SetFrame(SNI_Frame *p_Frame)
	{
		m_Frame = p_Frame;
	}

	bool SNI_Variable::FindVariable(SNI_VariableConstPointerList &p_DisplayVariableList) const
	{
		for (unsigned long j = 0; j<p_DisplayVariableList.size(); j++)
		{
			if (p_DisplayVariableList[j] == this)
			{
				return true;
			}
		}
		return false;
	}

	SNI_Expression * SNI_Variable::Clone(SNI_Frame *p_Frame, bool &p_Changed)
	{
		return p_Frame->ReplaceVariable(this, p_Changed);
	}

	bool SNI_Variable::Equivalent(SNI_Object * p_Other) const
	{
		if (dynamic_cast<SNI_Variable *>(p_Other))
		{
			SNI_Variable* l_variable = dynamic_cast<SNI_Variable *>(p_Other);
			return this == l_variable;
		}

		if (m_Value && dynamic_cast<SNI_Expression *>(p_Other))
		{
			return m_Value->Equivalent(p_Other);
		}
		return false;
	}

	SNI_WorldSet * SNI_Variable::GetWorldSet()
	{
		if (m_Value)
		{
			return m_Value->GetWorldSet();
		}
		return NULL;
	}

	size_t SNI_Variable::Cardinality(size_t p_MaxCardinality) const
	{
		if (!m_Value)
		{
			return p_MaxCardinality;
		}
		if (SN::Is<SNI_DelayedCall *>(m_Value))
		{
			return p_MaxCardinality;
		}
		return m_Value->Cardinality(p_MaxCardinality);
	}

	SN::SN_Error SNI_Variable::ForEach(std::function<SN::SN_Error(const SN::SN_Expression &p_Param, SNI_World*p_World)> p_Action)
	{
		return p_Action(this, NULL);
	}

	SN::SN_Error SNI_Variable::ForEachCart(long p_Depth, SNI_Cart * p_Cart)
	{
		return p_Cart->ProcessValue(p_Depth, this, NULL);
	}

	void SNI_Variable::ForEachSplit(SNI_Splitter * p_Splitter)
	{
		if (m_Value)
		{
			m_Value->ForEachSplit(p_Splitter);
		}
		else
		{
			p_Splitter->ProcessValueSplit(SN::SN_Expression(this), NULL);
		}
	}

	SN::SN_Expression SNI_Variable::Evaluate(long p_MetaLevel /* = 0 */) const
	{
		if (p_MetaLevel > 0)
		{
			return const_cast<SNI_Variable *>(this);
		}
		SNI_DelayedCall *call = dynamic_cast<SNI_DelayedCall *>(m_Value);
		if (call)
		{
			SNI_DelayedProcessor::GetProcessor()->Request(call);
		}
		return GetValue()->Evaluate(p_MetaLevel);
	}

	SN::SN_Expression SNI_Variable::PartialEvaluate(long p_MetaLevel /* = 0 */) const
	{
		if (p_MetaLevel > 0)
		{
			return this;
		}
		if (IsNullValue())
		{
			return this;
		}
		SN::SN_Expression result = m_Value->PartialEvaluate(p_MetaLevel);
		if (result.IsNull())
		{ // Expression could not be reduced further, because a variable is unknown, or calling a lambda expression might lead to recursion.
			return this;
		}
		return result;
	}

	SN::SN_Error SNI_Variable::SelfAssert()
	{
		if (m_Value && !m_Value->IsNull() && !m_Value->IsKnownValue() && !m_Value->IsReferableValue())
		{
			SNI_Expression *value = m_Value;
			m_Value = NULL;
			return value->AssertValue(this);
		}
		return true;
	}

	SN::SN_Error SNI_Variable::AssertValue(const SN::SN_Expression &p_Value)
	{
		if (SNI_World::ContextWorld())
		{
			if (!m_Value || m_Value->IsNull())
			{
				m_Value = new SNI_ValueSet();
			}
			return m_Value->AddValue(p_Value, 0, NULL, NULL);
		}
		else
		{
			if (m_Value == NULL || dynamic_cast<SNI_Null *>(m_Value) || dynamic_cast<SNI_DelayedCall *>(m_Value))
			{
				if (SN::Is<SNI_ValueSet *>(p_Value))
				{
					SN::SN_ValueSet set(p_Value);

					set.GetSNI_ValueSet()->AssignToVariable(this);
				}

				SNI_DelayedCall *call = dynamic_cast<SNI_DelayedCall *>(m_Value);
				m_Value = dynamic_cast<SNI_Expression *>(p_Value.GetSNI_Value());
				if (call)
				{
					SNI_DelayedProcessor::GetProcessor()->Request(call);
				}
				return true;
			}

			return m_Value->AssertValue(p_Value);
		}
	}

	SN::SN_Error SNI_Variable::PartialAssertValue(const SN::SN_Expression &p_Expression, bool p_Define)
	{
		if (m_Value == NULL || dynamic_cast<SNI_Null *>(m_Value) || dynamic_cast<SNI_DelayedCall *>(m_Value))
		{
			if (SN::Is<SNI_Variable*>(p_Expression.GetSNI_Expression()))
			{
				SNI_Variable *var = dynamic_cast<SNI_Variable*>(p_Expression.GetSNI_Expression());
				if (var->m_Value != this)
				{
					if (var->m_Value == NULL || var->m_Value->IsNull())
					{
						m_Value = p_Expression.GetSNI_Expression();
					}
					else
					{
						m_Value = var->m_Value;
					}
					return true;
				}
			}
			if (p_Define || SN::Is<SNI_Value*>(p_Expression))
			{
				m_Value = p_Expression.GetSNI_Expression();
				return true;
			}
			return false;
		}
		if (dynamic_cast<SNI_Value *>(m_Value))
		{
			return SN::SN_Expression(p_Expression.GetSNI_Expression()).PartialAssertValue(SN::SN_Value(m_Value));//Equivalent(dynamic_cast<SNI_Object>(p_Expression.GetSNI_Expression()));
		}
		if (dynamic_cast<SNI_Value *>(p_Expression.GetSNI_Expression()))
		{
			return SN::SN_Expression(m_Value).PartialAssertValue(p_Expression);//Equivalent(dynamic_cast<SNI_Object>(p_Expression.GetSNI_Expression()));
		}
		return false; // to be implemented, or too hard
	}

	SN::SN_Expression SNI_Variable::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Variable::Call ( " + DisplayPmExpressionList(p_ParameterList) + " )");

		if (m_Value)
		{
			SNI_Expression * l_clone = m_Value->Clone(this, NULL);
			// SNI_Expression * l_clone = CloneValue();
			SNI_Manager::GetTopManager()->DebugCommand(SN::CallPoint, "Variable.Call");
			SN::SN_Expression e = l_clone->Call(p_ParameterList, p_MetaLevel);
			SNI_Frame::Pop();
			return e;
		}
		return dynamic_cast<SNI_Expression *>(SN::SN_Error(GetTypeName() + " function to call is unknown.").GetSNI_Error());
	}

	SN::SN_Expression SNI_Variable::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Variable::PartialCall ( " + DisplayPmExpressionList(p_ParameterList) + " )");

		if (m_Value)
		{
			if (dynamic_cast<SNI_Lambda*>(m_Value))
			{
				return skynet::Null; // Don't want to do a partial call because it might lead to recursion.
			}
			return m_Value->PartialCall(p_ParameterList, p_MetaLevel);
		}
		return SN::SN_Error(GetTypeName() + " partial function to call is unknown.");
	}

	SN::SN_Expression SNI_Variable::Unify(SN::SN_ExpressionList * p_ParameterList)
	{
		if (m_Value)
		{
			SNI_Expression * l_clone = m_Value->Clone(this, (*p_ParameterList)[0].GetSNI_Expression());

			SNI_Manager::GetTopManager()->DebugCommand(SN::CallPoint, GetTypeName() + ".Unify after clone");
			SN::SN_Expression e = l_clone->Unify(p_ParameterList);
			SNI_Manager::GetTopManager()->DebugCommand(SN::CallPoint, GetTypeName() + ".Unify after unify");
			SNI_Variable *result = SNI_Frame::Top()->GetResult();
			result->SetValue((*p_ParameterList)[0].GetVariableValue());
			SNI_Frame::Pop();
			return e;
		}
		else
		{
			m_Value = AddLambdas(p_ParameterList).GetSNI_Expression();
			return SN::SN_Error(true);
		}
	}

	SN::SN_Error SNI_Variable::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result)
	{
		if (m_Value)
		{
			return m_Value->PartialUnify(p_ParameterList, p_Result);
		}
		else
		{
			m_Value = AddLambdasPartial(p_ParameterList, p_Result).GetSNI_Expression();
			return true;
		}
	}

	/* static */ SN::SN_Expression SNI_Variable::AddLambdas(SN::SN_ExpressionList * p_ParameterList)
	{
		if (1 < p_ParameterList->size())
		{
			SN::SN_Expression param = p_ParameterList->back();
			p_ParameterList->pop_back();
			return SN::SN_Lambda(param, AddLambdas(p_ParameterList));
		}
		return (*p_ParameterList)[0];
	}

	/* static */ SN::SN_Expression SNI_Variable::AddLambdasPartial(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result)
	{
		if (p_ParameterList->size())
		{
			SN::SN_Expression param = p_ParameterList->back().GetValue();
			p_ParameterList->pop_back();
			return SN::SN_Lambda(param, AddLambdasPartial(p_ParameterList, p_Result));
		}
		return p_Result;
	}

	void SNI_Variable::AttachDelayedCall(SNI_DelayedCall *p_Call)
	{
		if (!m_Value)
		{
			m_Value = p_Call;
		}
	}

	void SNI_Variable::SetValue(const SN::SN_Expression &p_Value)
	{
		m_Value = dynamic_cast<SNI_Expression *>(p_Value.GetSNI_Expression());
	}
}
