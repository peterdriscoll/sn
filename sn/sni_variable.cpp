#include "sni_variable.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

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
		: SNI_Namable("_t" + to_string(++m_MaxTempNum))
		, m_Value(NULL)
		, m_Frame(NULL)
		, m_Requested(false)
		, m_Inline(false)
	{
	}

	SNI_Variable::SNI_Variable(const string &p_Name)
		: m_Frame(NULL)
		, m_Value(NULL)
		, m_Requested(false)
		, m_Inline(false)
	{
		SetName(p_Name);
	}

	SNI_Variable::SNI_Variable(const string &p_Name, const string &p_DomainName, enum skynet::DefineType p_DefineType)
		: m_Frame(NULL)
		, m_Value(NULL)
		, m_Requested(false)
		, m_Inline(p_DefineType = skynet::Inline)
	{
		SetName(p_Name);
		SetDomainName(p_DomainName);
	}

	SNI_Variable::SNI_Variable(const SN::SN_Expression &p_Other)
		: m_Value(p_Other.GetSNI_Expression())
		, m_Frame(NULL)
		, m_Requested(false)
		, m_Inline(false)
	{
		REQUESTPROMOTION(m_Value);
	}

	SNI_Variable::~SNI_Variable()
	{

	}

	void SNI_Variable::PromoteMembers()
	{
		REQUESTPROMOTION(m_Value);
		REQUESTPROMOTION(m_Frame);
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
			SNI_Thread::GetThread()->RegisterChange(dynamic_cast<SNI_Variable *>(this));
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

	bool SNI_Variable::IsValue() const
	{
		return m_Value && m_Value->IsValue();
	}

	bool SNI_Variable::IsKnownValue() const
	{
		return m_Value && m_Value->IsKnownValue();
	}

	bool SNI_Variable::IsLeftKnownValue() const
	{
		return m_Value && m_Value->IsLeftKnownValue();
	}

	bool SNI_Variable::IsRightKnownValue() const
	{
		return m_Value && m_Value->IsRightKnownValue();
	}

	bool SNI_Variable::IsKnownTypeValue() const
	{
		return m_Value && m_Value->IsKnownTypeValue();
	}

	bool SNI_Variable::IsLambdaValue() const
	{
		return m_Value && m_Value->IsLambdaValue();
	}

	bool SNI_Variable::IsStringValue() const
	{
		return m_Value && m_Value->IsLambdaValue();
	}

	bool SNI_Variable::IsVariable() const
	{
		return true;
	}

	bool SNI_Variable::IsValueHolder() const
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
			SNI_Thread::GetThread()->GetProcessor()->Request(call);
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

	string SNI_Variable::FramePathName() const
	{
		if (m_Value && m_Value->IsVariable())
		{
			SN::SN_Variable value = m_Value;
			return FrameName() + "/" + value.GetSNI_Variable()->FramePathName();
		}
		return FrameName();
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

	const SNI_Expression * SNI_Variable::GetSafeValue() const
	{
		if (m_Value)
		{
			return m_Value->GetSafeValue();
		}
		return m_Value;
	}

	void SNI_Variable::Simplify()
	{
		if (m_Value && !SNI_Thread::GetThread()->ContextWorld())
		{
			if (m_Value->IsVariable())
			{
				m_Value->Simplify();
			}
			else
			{
				SNI_Expression *value = m_Value->SimplifyValue().GetSNI_Expression();
				if (value != m_Value)
				{
					m_Value = value;
					SNI_Thread::GetThread()->RegisterChange(dynamic_cast<SNI_Variable *>(this));
				}
			}
		}
	}

	SN::SN_Expression SNI_Variable::SimplifyValue()
	{
		Simplify();
		return m_Value;
	}

	SN::SN_Expression SNI_Variable::GetVariableValue(bool p_IfComplete)
	{
		if (m_Value)
		{
			if (m_Value->IsVariable())
			{
				SN::SN_Variable value = m_Value;
				return value.GetVariableValue(p_IfComplete);
			}
			Simplify();
			return m_Value;
		}
		return this;
	}

	void SNI_Variable::Fix(SN::SN_Expression p_Value)
	{
		if (m_Value)
		{
			m_Value->Fix(p_Value);
		}
	}

	void SNI_Variable::Fix()
	{
		if (m_Value)
		{
			m_Value->Fix();
		}
	}

	SN::SN_Error SNI_Variable::AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World ** p_WorldList, SNI_WorldSet *p_WorldSet)
	{
		if (p_Value.IsError())
		{
			return p_Value.GetError();
		}
		if (p_Value.IsVariable())
		{
			return p_Value.AddValue(p_Value, p_NumWorlds, p_WorldList, p_WorldSet);
		}
		if (SNI_Thread::GetThread()->ContextWorld())
		{
			if (!m_Value || m_Value->IsNull())
			{
				m_Value = new SNI_ValueSet();
				REQUESTPROMOTION(m_Value);
			}
			return m_Value->AddValue(p_Value, p_NumWorlds, p_WorldList, p_WorldSet);
			SNI_Thread::GetThread()->RegisterChange(dynamic_cast<SNI_Variable *>(this));
		}
		else
		{
			if (p_WorldList)
			{
				LOGGING(SN::LogContext context("SNI_Variable::AddValue (" + SN::SN_Expression(this).DisplayValueSN() + " := " + p_Value.DisplayValueSN() + " worlds " + worldString + " set " + DisplayWorldSet(p_WorldSet) + " )"));
				bool exists = false;
				SNI_WorldSet *l_WorldSet = p_WorldSet;
				if (!l_WorldSet)
				{
					l_WorldSet = GetWorldSet();
				}
				SNI_World *world = l_WorldSet->JoinWorldsArray(ManualAddWorld, AlwaysCreateWorld, exists, p_NumWorlds, p_WorldList);
				if (exists)
				{
					LOGGING(context.LogExpression(world->DisplaySN(), p_Value));
					SN::SN_Error e = AssertValue(p_Value);
					if (e.GetBool())
					{
						l_WorldSet->AddToSetList(world);
					}
					return LOG_RETURN(context, e);
				}
				else
				{
					LOGGING(context.LogText("Fail", "JoinWorlds failed on " + worldString));
					return LOG_RETURN(context, SN::SN_Error(false, false, "SNI_Variable::AddValue: JoinWorlds failed on " + DisplayWorlds(p_NumWorlds, p_WorldList)));
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

	string SNI_Variable::GetValueTypeName() const
	{
		if (m_Value)
		{
			return m_Value->GetValueTypeName();
		}
		return GetTypeName();
	}

	string SNI_Variable::GetReferredName() const
	{
		return GetName();
	}
	
	string SNI_Variable::DisplayCpp() const
	{
		return "SN_Variable(" + GetName() + ")";
	}

	string SNI_Variable::DisplaySN(long p_Priority, SNI_DisplayOptions &p_DisplayOptions) const
	{
		if (m_Value && m_Value->IsValue() && !m_Value->IsValueSet() && p_DisplayOptions.GetDebugHTML() == doDebugPointsJS)
		{
			if (p_DisplayOptions.GetLevel() == 0)
			{
				p_DisplayOptions.IncrementLevel();
				string sValue = "<button class='name' ng-click='selectvar(\"" + FrameName() + "\")'>{{ selecttext('" + FrameName() + "','" + FramePathName()+":"+m_Value->DisplayValueSN(p_Priority, p_DisplayOptions) + "')}}</button>";
				p_DisplayOptions.DecrementLevel();
				return sValue;
			}
		}
		return FrameName();
	}

	string SNI_Variable::DisplayValueSN(long priority, SNI_DisplayOptions & p_DisplayOptions) const
	{
		if (m_Value)
		{
			if (dynamic_cast<SNI_DelayedCall *>(m_Value))
			{
				return FrameName();
			}
			return m_Value->DisplayValueSN(priority, p_DisplayOptions);
		}
		return FrameName();
	}

	string SNI_Variable::DisplayCall(long p_Priority, SNI_DisplayOptions & p_DisplayOptions, size_t p_NumParams, SN::SN_Expression *p_ParamList, const SNI_Expression *p_DebugSource) const
	{
		string text;
		string delimeter;
		for (size_t j = 0; j < p_NumParams; j++)
		{
			SN::SN_Expression &p = p_ParamList[j];
			string del = delimeter;
			delimeter = " ";
			text += del + p.GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions);
		}
		return Bracket(p_Priority, SetBreakPoint(FrameName(), p_DisplayOptions, this, SN::LeftId) + " " + text, p_DisplayOptions, this);
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
		REQUESTPROMOTION(m_Frame);
	}

	SNI_Expression * SNI_Variable::Clone(long p_MetaLevel, SNI_Frame *p_Frame, bool &p_Changed)
	{
		if (p_MetaLevel <= 0)
		{
			return p_Frame->ReplaceVariable(this, p_Changed);
		}
		return this;
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

	size_t SNI_Variable::Hash() const
	{
		return m_Value->Hash();
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

	size_t SNI_Variable::LeftCardinality(size_t p_MaxCardinality) const
	{
		if (!m_Value)
		{
			return p_MaxCardinality;
		}
		if (SN::Is<SNI_DelayedCall *>(m_Value))
		{
			return p_MaxCardinality;
		}
		return m_Value->LeftCardinality(p_MaxCardinality);
	}

	size_t SNI_Variable::RightCardinality(size_t p_MaxCardinality) const
	{
		if (!m_Value)
		{
			return p_MaxCardinality;
		}
		if (SN::Is<SNI_DelayedCall *>(m_Value))
		{
			return p_MaxCardinality;
		}
		return m_Value->RightCardinality(p_MaxCardinality);
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

	SN::SN_Expression SNI_Variable::DoEvaluate(long p_MetaLevel /* = 0 */) const
	{
		if (p_MetaLevel > 0)
		{
			return const_cast<SNI_Variable *>(this);
		}
		SNI_DelayedCall *call = dynamic_cast<SNI_DelayedCall *>(m_Value);
		if (call)
		{
			SNI_Thread::GetThread()->GetProcessor()->Request(call);
		}
		return GetValue()->DoEvaluate(p_MetaLevel);
	}

	SN::SN_Expression SNI_Variable::DoPartialEvaluate(long p_MetaLevel /* = 0 */) const
	{
		if (p_MetaLevel > 0)
		{
			return this;
		}
		if (IsNullValue())
		{
			return this;
		}
		if (!dynamic_cast<SNI_Lambda*>(m_Value)
			&& !dynamic_cast<SNI_Virtual*>(m_Value)
			&& !dynamic_cast<SNI_Derived*>(m_Value))
		{
			SN::SN_Expression result = m_Value->DoPartialEvaluate(p_MetaLevel);
			if (result.IsNull())
			{ // Expression could not be reduced further, because a variable is unknown, or calling a lambda expression might lead to recursion.
				return this;
			}
			return result;
		}
		return this;
	}

	SN::SN_Error SNI_Variable::SelfAssert()
	{
		SN::SN_Error e(skynet::OK);
		if (m_Value && !m_Value->IsNull() && !m_Value->IsKnownValue() && !m_Value->IsReferableValue() && !m_Value->IsVariable() && !m_Value->IsValueSet())
		{
			SNI_Expression *saveValue = m_Value;
			m_Value = NULL;
			e = saveValue->AssertValue(this);
			if (e.IsError() || !m_Value)
			{
				m_Value = saveValue;
			}
		}
		return e;
	}

	SN::SN_Error SNI_Variable::AssertValue(const SN::SN_Expression &p_Value)
	{
		if (SNI_Thread::GetThread()->ContextWorld())
		{
			if (!m_Value || m_Value->IsNull())
			{
				m_Value = new SNI_ValueSet();
				REQUESTPROMOTION(m_Value);
			}
			return m_Value->AddValue(p_Value, 0, NULL, NULL);
			SNI_Thread::GetThread()->RegisterChange(dynamic_cast<SNI_Variable *>(this));
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
				m_Value = p_Value.GetSNI_Expression();
				SNI_Thread::GetThread()->RegisterChange(dynamic_cast<SNI_Variable *>(this));
				REQUESTPROMOTION(m_Value);
				if (call)
				{
					SNI_Thread::GetThread()->GetProcessor()->Request(call);
				}
				return skynet::OK;
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
						REQUESTPROMOTION(m_Value);
					}
					else
					{
						m_Value = var->m_Value;
						REQUESTPROMOTION(m_Value);
					}
					SNI_Thread::GetThread()->RegisterChange(dynamic_cast<SNI_Variable *>(this));
					LOG(WriteVariable(SN::DebugLevel, this));
					return skynet::OK;
				}
			}

			if (p_Define || SN::Is<SNI_Value*>(p_Expression))
			{
				if (SN::Is<SNI_ValueSet *>(p_Expression))
				{
					SN::SN_ValueSet set(p_Expression);

					set.GetSNI_ValueSet()->AssignToVariable(this);
				}

				m_Value = p_Expression.GetSNI_Expression();
				SNI_Thread::GetThread()->RegisterChange(dynamic_cast<SNI_Variable *>(this));
				REQUESTPROMOTION(m_Value);
				LOG(WriteVariable(SN::DebugLevel, this));
				return skynet::OK;
			}
			return skynet::Fail;
		}
		if (dynamic_cast<SNI_Value *>(m_Value))
		{
			return SN::SN_Expression(p_Expression.GetSNI_Expression()).PartialAssertValue(SN::SN_Value(m_Value));//Equivalent(dynamic_cast<SNI_Object>(p_Expression.GetSNI_Expression()));
		}
		if (dynamic_cast<SNI_Value *>(p_Expression.GetSNI_Expression()))
		{
			return SN::SN_Expression(m_Value).PartialAssertValue(p_Expression, p_Define);
		}
		return skynet::Fail; // to be implemented, or too hard
	}

	SN::SN_Expression SNI_Variable::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Variable::Call ( " + DisplayPmExpressionList(p_ParameterList) + " )"));

		if (m_Value)
		{
			SNI_Expression * l_clone = m_Value->Clone(this, NULL);
			LOG(WriteClonedExpression(SN::DebugLevel, "Call var: ", l_clone));
			Breakpoint(SN::DebugStop, SN::LeftId, GetTypeName(), "Call " + GetName(), this, SN::CallPoint);

			SN::SN_Expression e = l_clone->Call(p_ParameterList, p_MetaLevel);
			SNI_Frame::Pop();
			if (e.IsNull())
			{
				return LOG_RETURN(context, skynet::Fail);
			}
			else
			{
				Breakpoint(SN::DebugStop, SN::RightId, GetTypeName(), "Return from call " + GetName(), this, SN::CallPoint);
			}
			return LOG_RETURN(context, e);
		}
		return LOG_RETURN(context, dynamic_cast<SNI_Expression *>(SN::SN_Error(false, false, GetTypeName() + " function to call is unknown.").GetSNI_Error()));
	}

	SN::SN_Expression SNI_Variable::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Variable::PartialCall ( " + DisplayPmExpressionList(p_ParameterList) + " )"));

		SN::SN_Expression result(this);
		if (m_Value)
		{
			if (!dynamic_cast<SNI_Lambda*>(m_Value)
				&& !dynamic_cast<SNI_Virtual*>(m_Value)
				&& !dynamic_cast<SNI_Derived*>(m_Value))
			{
				SNI_Expression * l_clone = m_Value->Clone(this, (*p_ParameterList)[0].GetSNI_Expression());

				return LOG_RETURN(context, l_clone->PartialCall(p_ParameterList, p_MetaLevel));
			}
		}
		while (!p_ParameterList->empty())
		{
			result = SN::SN_Function(result, p_ParameterList->back().DoPartialEvaluate(p_MetaLevel));
			p_ParameterList->pop_back();
		}
		return LOG_RETURN(context, result);
	}

	SN::SN_Expression SNI_Variable::Unify(SN::SN_ExpressionList * p_ParameterList)
	{
		if (m_Value)
		{
			SNI_Expression * l_clone = m_Value->Clone(this, (*p_ParameterList)[0].GetSNI_Expression());
			
			if (SNI_Thread::TopManager()->TailCallOptimization())
			{
				LOG(WriteExp(l_clone));
			}

			LOG(WriteHeading(SN::DebugLevel, GetTypeName() + ": Start " + DisplayUnifyExp(p_ParameterList)));
			LOG(WriteClonedExpression(SN::DebugLevel, "Unify var: ", l_clone));

			SNI_Frame *topFrame = SNI_Frame::Top();
			for (size_t j = 1; j < p_ParameterList->size(); j++)
			{
				topFrame->CreateParameter(j, (*p_ParameterList)[j]);
			}

			SN::SN_Expression e = l_clone->Unify(p_ParameterList);

			SNI_Variable *result = topFrame->GetResult();
			result->SetValue((*p_ParameterList)[0].GetVariableValue());
			for (size_t j = 1; j < p_ParameterList->size(); j++)
			{
				SNI_Variable *param = topFrame->GetVariable(j);
				topFrame->CreateParameter(j, (*p_ParameterList)[j]);
			}
			LOG(WriteHeading(SN::DebugLevel, GetTypeName() + ": End " + DisplayUnifyExp(p_ParameterList)));

			SNI_Frame::Pop();
			if (e.GetSNI_Error())
			{
				return e;
			} else if (e.IsNull())
			{
				return skynet::OK;
			}
			return e;
		}
		else
		{
			AssertValue(AddLambdas(p_ParameterList).GetSNI_Expression());
			SNI_Thread::GetThread()->RegisterChange(dynamic_cast<SNI_Variable *>(this));
			REQUESTPROMOTION(m_Value);
			return skynet::OK;
		}
	}

	SN::SN_Error SNI_Variable::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		if (m_Value)
		{
			SN::SN_Error e = m_Value->PartialUnify(p_ParameterList, p_Result, p_Define);
			if (e.IsNull())
			{
				return skynet::Fail;
			}
			return e;
		}
		else if (p_Define || SNI_Thread::TopManager()->AutoDefine())
		{
			m_Value = AddLambdasPartial(p_ParameterList, p_Result).GetSNI_Expression();
			SNI_Thread::GetThread()->RegisterChange(dynamic_cast<SNI_Variable *>(this));
			REQUESTPROMOTION(m_Value);
			return skynet::OK;
		}
		return skynet::Fail;
	}

	void SNI_Variable::AttachDelayedCall(SNI_DelayedCall *p_Call)
	{
		if (!m_Value)
		{
			m_Value = p_Call;
			SNI_Thread::GetThread()->RegisterChange(dynamic_cast<SNI_Variable *>(this));
			REQUESTPROMOTION(m_Value);
		}
		else if (m_Value->IsVariable())
		{
			m_Value->AttachDelayedCall(p_Call);
		}
	}

	void SNI_Variable::SetValue(const SN::SN_Expression &p_Value)
	{
		SNI_Expression *value = dynamic_cast<SNI_Expression *>(p_Value.GetSNI_Expression());
		if (value != this)
		{
			m_Value = value;
			SNI_Thread::GetThread()->RegisterChange(dynamic_cast<SNI_Variable *>(this));
			REQUESTPROMOTION(m_Value);
		}
	}

	void SNI_Variable::SetInline(bool p_Inline)
	{
		m_Inline = p_Inline;
	}

	SN::SN_Expression SNI_Variable::DoBuildMeta(long p_MetaLevel)
	{
		SN::SN_Expression expression(this);
		return SN::SN_Meta(p_MetaLevel, expression);
	}
}
