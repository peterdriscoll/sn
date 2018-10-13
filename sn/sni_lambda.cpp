#include "sni_lambda.h"

#include "logcontext.h"
#include "sn_parameter.h"
#include "sn_value.h"
#include "sn_error.h"

#include "sni_variable.h"
#include "sni_helpers.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ long SNI_Lambda::m_Id = 0;

	SNI_Lambda::SNI_Lambda()
	{

	}

	SNI_Lambda::SNI_Lambda(SNI_Expression *p_FormalParameter, SNI_Expression *p_Expression, SNI_Expression *p_ConstraintValue)
		: m_FormalParameter(p_FormalParameter)
		, m_Expression(p_Expression)
		, m_ConstraintValue(p_ConstraintValue)
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
	}

	string SNI_Lambda::GetTypeName() const
	{
		return "Lambda";
	}

	string SNI_Lambda::DisplayCpp() const
	{
		return "sn_Lambda(" + m_FormalParameter->DisplayCpp() + ", " + m_Expression->DisplayCpp() + ")";
	}

	string SNI_Lambda::DisplaySN(long /*priority*/, SNI_DisplayOptions &p_DisplayOptions) const
	{
		return "@" + m_FormalParameter->DisplaySN(GetPriority(), p_DisplayOptions) + "." + m_Expression->DisplaySN(GetPriority(), p_DisplayOptions);
	}

	long SNI_Lambda::GetPriority() const
	{
		return 0;
	}

	string SNI_Lambda::GetOperator() const
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

	SNI_Expression * SNI_Lambda::Clone(SNI_Frame *p_Frame, bool &p_Changed)
	{
		bool changed = false;
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
				return dynamic_cast<SNI_Expression *>(new SNI_Lambda(dynamic_cast<SNI_Expression *>(l_NewVariable), l_expression, m_ConstraintValue));
			}
		}
		else
		{
			SNI_Expression *l_expression = m_Expression->Clone(p_Frame, changed);
			if (changed)
			{
				p_Changed = true;
				m_FormalParameter->AssertIsA(m_ConstraintValue);
				return dynamic_cast<SNI_Expression *>(new SNI_Lambda(m_FormalParameter, l_expression, m_ConstraintValue));
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

	SN::SN_Expression SNI_Lambda::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		SN::LogContext context(DisplaySN0() + "SNI_Lambda::Call ( " + DisplayPmExpressionList(p_ParameterList) + " )");

		ASSERTM(p_ParameterList->size() >0, "Cannot call a lambda without a parameter");
		SN::SN_Expression param = p_ParameterList->back().GetSNI_Expression();
		p_ParameterList->pop_back();
		SN::SN_Error e;
		if (!m_FormalParameter || m_FormalParameter->IsNullValue())
		{	// Lazy evaluation. Delay until the value is needed.
			e = m_FormalParameter->PartialAssertValue(param, true);
		}
		else
		{	// The value is needed now.
			e = m_FormalParameter->AssertValue(param.Evaluate());
		}
		LOG(WriteLine(SN::DebugLevel, DisplaySN0()));
		if (e.IsError())
		{	// If can't assert actual to formal, then don't make the call. But no error return.
			return skynet::Null;
		}
		if (p_ParameterList->size() > 0)
		{
			SNI_Thread::GetThread()->DebugCommand(SN::CallPoint, "Lambda.Call");
			return m_Expression->Call(p_ParameterList, p_MetaLevel);
		}
		SNI_Thread::GetThread()->DebugCommand(SN::CallPoint, "Lambda.Evaluate");
		return m_Expression->Evaluate(p_MetaLevel);
	}

	SN::SN_Expression SNI_Lambda::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Lambda::PartialCall ( " + DisplayPmExpressionList(p_ParameterList) + " )");

		ASSERTM(p_ParameterList->size() >0, "Cannot call a lambda without a parameter");

		SNI_Expression * param = p_ParameterList->front().GetSNI_Expression();
		p_ParameterList->erase(p_ParameterList->begin());
		param->PartialAssertValue(SN::SN_Expression(m_FormalParameter));
		if (p_ParameterList->size() > 0)
		{
			return m_Expression->PartialCall(p_ParameterList, p_MetaLevel);
		}
		return m_Expression->PartialEvaluate(p_MetaLevel);
	}

	SN::SN_Expression SNI_Lambda::Evaluate(long p_MetaLevel /* = 0 */) const
	{
		return this;
	}

	SN::SN_Expression SNI_Lambda::PartialEvaluate(long p_MetaLevel /* = 0 */) const
	{
		return this;
	}

	SN::SN_Expression SNI_Lambda::Unify(SN::SN_ExpressionList * p_ParameterList)
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Lambda::Unify ( " + DisplayPmExpressionList(p_ParameterList) + " )");

		ASSERTM(p_ParameterList->size() > 1, "Cannot unify to a lambda without a parameter");
		SN::SN_Expression param = p_ParameterList->back();
		p_ParameterList->pop_back();
		SN::SN_Error e;
		if (!m_FormalParameter || m_FormalParameter->IsNullValue())
		{	// Lazy evaluation. Delay until the value is needed.
			e = SN::SN_Expression(m_FormalParameter).PartialAssertValue(param, true);
		}
		else
		{	// The value is needed now.
			e = m_FormalParameter->AssertValue(param.Evaluate());
		}
		if (e.IsError())
		{
			return skynet::Null; // The parameter didn't match, but there may be other polymorphic calls.
		}
		LOG(WriteFrame(SNI_Thread::GetThread(), SN::DebugLevel));
		SNI_Thread::GetThread()->DebugCommand(SN::ParameterPoint, "Lambda.Unify");
		if (p_ParameterList->size() > 1)
		{
			return m_Expression->Unify(p_ParameterList);
		}
		return m_Expression->AssertValue(p_ParameterList->back());
	}

	SN::SN_Error SNI_Lambda::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Lambda::PartialUnify ( " + DisplayPmParameterList(p_ParameterList) + " = " + p_Result.DisplaySN() + " )");

		ASSERTM(p_ParameterList->size() >0, "Cannot call a lambda without a parameter");
		SNI_Expression * param = p_ParameterList->front().GetValue().GetSNI_Expression();
		p_ParameterList->erase(p_ParameterList->begin());
		SN::SN_Error e = SN::SN_Expression(m_FormalParameter).PartialAssertValue(param, true);
		if (e.IsError())
		{
			return skynet::Null; // The parameter didn't match, but there may be other polymorphic calls.
		}
		if (e.IsError())
		{
			e.AddNote(context, this, "Assigning parameter value failed");
			return false;
		}
		if (p_ParameterList->size() > 0)
		{
			return m_Expression->PartialUnify(p_ParameterList, p_Result);
		}
		return m_Expression->PartialAssertValue(p_Result);
	}
}
