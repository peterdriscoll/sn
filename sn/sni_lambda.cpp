#include "sni_lambda.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sn_parameter.h"
#include "sn_value.h"
#include "sn_error.h"

#include "sni_variable.h"
#include "sni_helpers.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ SNI_Class* SNI_Lambda::Class()
	{
		return SNI_User::GetCurrentUser()->GetOrCreatePointer<SNI_Lambda, SNI_Class>("Lambda");
	}

	SNI_Class* SNI_Lambda::VClass()
	{
		return Class();
	}

	SNI_Lambda::SNI_Lambda()
	{
	}

	SNI_Lambda::SNI_Lambda(SNI_Expression *p_FormalParameter, SNI_Expression *p_Expression, SNI_Expression *p_ConstraintValue, unsigned long p_Id)
		: m_FormalParameter(p_FormalParameter)
		, m_Expression(p_Expression)
		, m_ConstraintValue(p_ConstraintValue)
		, SNI_Value(p_Id)
	{
		REQUESTPROMOTION(m_FormalParameter);
		REQUESTPROMOTION(m_Expression);
		REQUESTPROMOTION(m_ConstraintValue);
	}

	SNI_Lambda::~SNI_Lambda()
	{

	}

	void SNI_Lambda::PromoteMembers()
	{
		REQUESTPROMOTION(m_FormalParameter);
		REQUESTPROMOTION(m_Expression);
		REQUESTPROMOTION(m_ConstraintValue);
	}

	std::string SNI_Lambda::GetTypeName() const
	{
		return "Lambda";
	}

	std::string SNI_Lambda::GetReferredName() const
	{
		if (m_FormalParameter)
		{
			return GetTypeName() + "_" + m_FormalParameter->GetReferredName();
		}
		return GetTypeName();
	}

	bool SNI_Lambda::IsValue() const
	{
		return false;
	}

	bool SNI_Lambda::IsLambdaValue() const
	{
		return true;
	}

	bool SNI_Lambda::IsInline() const
	{
		return false;
	}

	std::string SNI_Lambda::DisplayCpp() const
	{
		return "Lambda(" + m_FormalParameter->DisplayCpp() + ", " + m_Expression->DisplayCpp() + ")";
	}

	std::string SNI_Lambda::DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const
	{
		p_DisplayOptions.IncrementLevel();
		std::string sParam = m_FormalParameter->DisplaySN(GetPriority(), p_DisplayOptions);
		p_DisplayOptions.DecrementLevel();

		std::string text = SetBreakPoint("@", p_DisplayOptions, this, SN::LeftId) + sParam + SetBreakPoint(".", p_DisplayOptions, this, SN::ParameterOneId) + m_Expression->DisplaySN(GetPriority(), p_DisplayOptions);
		return Bracket(priority, text, p_DisplayOptions, this);
	}

	void SNI_Lambda::AddVariables(long p_MetaLevel, SNI_VariablePointerMap& p_Map)
	{
		SNI_Expression* formalParameter = m_FormalParameter;
		if (formalParameter)
		{
			formalParameter->AddVariables(p_MetaLevel, p_Map);
		}
		SNI_Expression* expression = m_Expression;
		if (expression)
		{
			expression->AddVariables(p_MetaLevel, p_Map);
		}
		SNI_Expression* constraintValue = m_ConstraintValue;
		if (constraintValue)
		{
			constraintValue->AddVariables(p_MetaLevel, p_Map);
		}
	}

	long SNI_Lambda::GetPriority() const
	{
		return 1000;
	}

	std::string SNI_Lambda::GetOperator() const
	{
		return "@";
	}

	SNI_Expression * SNI_Lambda::GetExpression()
	{
		return m_Expression;
	}

	SNI_Expression * SNI_Lambda::GetFormalParameter()
	{
		return m_FormalParameter;
	}

	SNI_Expression * SNI_Lambda::Clone(long p_MetaLevel, SNI_Frame *p_Frame, bool &p_Changed)
	{
		bool changed = true;
		SNI_Variable *l_FormalParameter = dynamic_cast<SNI_Variable *>(m_FormalParameter);
		if (l_FormalParameter)
		{
			SNI_Variable *l_NewVariable = new SNI_Variable(l_FormalParameter->GetName());
			l_NewVariable->SetFrame(p_Frame);
			SNI_Expression *l_expression = p_Frame->CloneReplace(changed, l_FormalParameter, l_NewVariable, m_Expression);
			if (changed)
			{
				p_Changed = true;
				l_NewVariable->AssertIsA(m_ConstraintValue);
				SNI_Expression *lambda = new SNI_Lambda(dynamic_cast<SNI_Expression *>(l_NewVariable), l_expression, m_ConstraintValue, GetId());
				LOG(WriteClonedExpression(SN::DebugLevel, "", lambda));
				return lambda;
			}
		}
		else
		{
			SNI_Expression *l_expression = m_Expression->Clone(p_MetaLevel, p_Frame, changed);
			if (changed)
			{
				p_Changed = true;
				m_FormalParameter->AssertIsA(m_ConstraintValue);
				SNI_Expression *lambda = new SNI_Lambda(m_FormalParameter, l_expression, m_ConstraintValue, GetId());
				return lambda;
			}
		}
		return this;
	}

	bool SNI_Lambda::Equivalent(SNI_Object * p_Other) const
	{
		if (dynamic_cast<SNI_Lambda *>(p_Other))
		{
			SNI_Lambda * l_lambda = dynamic_cast<SNI_Lambda *>(p_Other);
			return m_FormalParameter->Equivalent(dynamic_cast<SNI_Object *>(l_lambda->m_FormalParameter))
				&& m_Expression->Equivalent(dynamic_cast<SNI_Object *>(l_lambda->m_Expression));
		}
		return false;
	}

	size_t SNI_Lambda::Cardinality(size_t) const
	{
		return 1;
	}

	SNI_Expression * SNI_Lambda::LoadFormalParameters(SN::SN_ExpressionList &p_FormalParameterList)
	{
		p_FormalParameterList.push_back(m_FormalParameter);
		return m_Expression->LoadFormalParameters(p_FormalParameterList);
	}

	SN::SN_Expression SNI_Lambda::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + "SNI_Lambda::Call ( " + DisplaySnExpressionList(p_ParameterList) + " )"));

		Breakpoint(SN::DebugStop, SN::LeftId, GetTypeName(), "Call", this, SN::CallPoint);
		if (0 < p_MetaLevel)
		{
			SN::SN_Expression formalParameter;
			if (m_FormalParameter)
			{
				formalParameter = m_FormalParameter->DoEvaluate(p_MetaLevel);
			}
			SN::SN_Expression expression;
			if (m_Expression)
			{
				expression = m_Expression->DoEvaluate(p_MetaLevel);
			}
			SN::SN_Expression constraintValue;
			if (m_ConstraintValue)
			{
				constraintValue = m_ConstraintValue->DoEvaluate(p_MetaLevel);
			}
			return SN::SN_Lambda(formalParameter, expression, constraintValue);
		}

		if (p_ParameterList->empty())
		{
			return SN::SN_Error(false, false, "Cannot call a lambda expression without a parameter");
		}
		SN::SN_Expression param = p_ParameterList->back().GetSNI_Expression();
		p_ParameterList->pop_back();
		SN::SN_Error e = skynet::OK;
		if ((SNI_Thread::TopManager()->GetEvaluationType() == skynet::Lazy) && (!m_FormalParameter || m_FormalParameter->IsNullValue()))
		{	// Lazy evaluation. Delay until the value is needed.
			e = m_FormalParameter->PartialAssertValue(param, true);
		}
		else
		{	// The value is needed now.
			e = m_FormalParameter->AssertValue(param.DoEvaluate());
		}
		LOG(WriteLine(SN::DebugLevel, DisplaySN0()));
		if (e.IsError())
		{	// If can't assert actual to formal, then don't make the call. But no error return.
			return skynet::Null;
		}
		SN::SN_Expression result;
		if (p_ParameterList->size() > 0)
		{
			Breakpoint(SN::DebugStop, SN::ParameterOneId, GetTypeName(), "Call before calculation", this, SN::CallPoint);
			result = m_Expression->Call(p_ParameterList, p_MetaLevel);
		}
		else
		{
			Breakpoint(SN::DebugStop, SN::ParameterOneId, GetTypeName(), "Call before evaluate", this, SN::CallPoint);
			if (SNI_Thread::TopManager()->TailCallOptimization())
			{
				result = m_Expression; // Let the DoEvaluate lower down the stack evaluate it.
			}
			else
			{
				result = m_Expression->DoEvaluate(p_MetaLevel);
			}
		}

		Breakpoint(SN::DebugStop, SN::RightId, GetTypeName(), "Call return", this, SN::CallPoint);

		return LOG_RETURN(context, result);
	}

	SN::SN_Expression SNI_Lambda::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Lambda::PartialCall ( " + DisplaySnExpressionList(p_ParameterList) + " )"));

		if (0 < p_MetaLevel)
		{
			SN::SN_Expression formalParameter;
			if (m_FormalParameter)
			{
				formalParameter = m_FormalParameter->DoPartialEvaluate(p_MetaLevel);
			}
			SN::SN_Expression expression;
			if (m_Expression)
			{
				expression = m_Expression->DoPartialEvaluate(p_MetaLevel);
			}
			SN::SN_Expression constraintValue;
			if (m_ConstraintValue)
			{
				constraintValue = m_ConstraintValue->DoPartialEvaluate(p_MetaLevel);
			}
			return SN::SN_Lambda(formalParameter, expression, constraintValue);
		}

		if (p_ParameterList->empty())
		{
			return SN::SN_Error(false, false, "Cannot partial call a lambda expression without a parameter");
		}

		SN::SN_Expression param = p_ParameterList->back().DoPartialEvaluate(p_MetaLevel);
		p_ParameterList->pop_back();
		if (param.IsKnownValue() && !param.IsLambdaValue())
		{
			m_FormalParameter->PartialAssertValue(param);
			if (p_ParameterList->size() > 0)
			{
				return m_Expression->PartialCall(p_ParameterList, p_MetaLevel);
			}
			return LOG_RETURN(context, m_Expression->DoPartialEvaluate(p_MetaLevel));
		}
		return LOG_RETURN(context, SN::SN_Function(DoPartialEvaluate(p_MetaLevel), param));
	}

	SN::SN_Expression SNI_Lambda::DoEvaluate(long p_MetaLevel /* = 0 */) const
	{
		return this;
	}

	SN::SN_Expression SNI_Lambda::DoPartialEvaluate(long p_MetaLevel /* = 0 */) const
	{
		return SN::SN_Lambda(m_FormalParameter, m_Expression->DoPartialEvaluate(p_MetaLevel), m_ConstraintValue->DoPartialEvaluate(p_MetaLevel));
	}

	SN::SN_Expression SNI_Lambda::Unify(SN::SN_ExpressionList * p_ParameterList)
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Lambda::Unify ( " + DisplaySnExpressionList(p_ParameterList) + " )"));

		Breakpoint(SN::DebugStop, SN::LeftId, GetTypeName(), "Unify", this, SN::CallPoint);
		if (p_ParameterList->empty())
		{
			return SN::SN_Error(false, false, "Cannot unify to a lambda expression without a parameter");
		}
		SN::SN_Expression param = p_ParameterList->back();
		p_ParameterList->pop_back();
		SN::SN_Expression result = skynet::OK;
		if ((SNI_Thread::TopManager()->GetEvaluationType() == skynet::Lazy) && (!m_FormalParameter || m_FormalParameter->IsNullValue()))
		{	// Lazy evaluation. Delay until the value is needed.
			result = SN::SN_Expression(m_FormalParameter).PartialAssertValue(param, true);
		}
		else
		{	// The value is needed now.
			result = param.AssertValue(m_FormalParameter);
		}
		if (result.IsError())
		{
			return LOG_RETURN(context, skynet::Null); // The parameter didn't match, but there may be other polymorphic calls.
		}
		LOG(WriteFrame(SNI_Thread::GetThread(), SN::DebugLevel));
		if (p_ParameterList->size() > 1)
		{
			Breakpoint(SN::DebugStop, SN::ParameterOneId, GetTypeName(), "Unify parameter", this, SN::CallPoint);
			result = m_Expression->Unify(p_ParameterList);
		}
		else
		{
			Breakpoint(SN::DebugStop, SN::ParameterOneId, GetTypeName(), "Assert value parameter", this, SN::CallPoint);
			if (SNI_Thread::TopManager()->TailCallOptimization())
			{
				result = m_Expression; // Let the AssertValue lower down the stack asert it.
			}
			else
			{
				result = m_Expression->AssertValue(p_ParameterList->back());
			}
		}

		Breakpoint(SN::DebugStop, SN::RightId, GetTypeName(), "Assert value return", this, SN::CallPoint);
		return LOG_RETURN(context, result);
	}

	SN::SN_Error SNI_Lambda::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Lambda::PartialUnify ( " + DisplaySnParameterList(p_ParameterList) + " = " + p_Result.DisplaySN() + " )"));

		ASSERTM(p_ParameterList->size() >0, "Cannot call a lambda without a parameter");
		SN::SN_Expression param = p_ParameterList->back().GetValue();
		p_ParameterList->pop_back();
		SN::SN_Error e = SN::SN_Expression(m_FormalParameter).PartialAssertValue(param, true);
		if (e.IsError())
		{
			return LOG_RETURN(context, skynet::Fail); // The parameter didn't match, but there may be other polymorphic calls.
		}
		if (e.IsError())
		{
			if (e.IsSignificantError())
			{
				SNI_CallRecord *callRecord = new SNI_CallRecord("Assigning parameter value.", this);
				LOGGING(callRecord->SetLogContext(context));
				e.GetSNI_Error()->AddNote(callRecord);
			}
			return LOG_RETURN(context, e);
		}
		if (p_ParameterList->size() > 0)
		{
			return LOG_RETURN(context, m_Expression->PartialUnify(p_ParameterList, p_Result));
		}
		return LOG_RETURN(context, m_Expression->PartialAssertValue(p_Result));
	}
}
