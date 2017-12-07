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

	SNI_Lambda::SNI_Lambda(SNI_Expression *p_FormalParameter, SNI_Expression *p_Expression)
		: m_FormalParameter(p_FormalParameter)
		, m_Expression(p_Expression)
	{
	}

	SNI_Lambda::~SNI_Lambda()
	{

	}

	void SNI_Lambda::PromoteMembers()
	{
	}

	string SNI_Lambda::GetTypeName() const
	{
		return "Lambda";
	}

	string SNI_Lambda::DisplayCpp() const
	{
		return "sn_Lambda(" + m_FormalParameter->DisplayCpp() + ", " + m_Expression->DisplayCpp() + ")";
	}

	string SNI_Lambda::DisplaySN(long /*priority*/, SNI_VariablePointerList &p_DisplayVariableList) const
	{
		return "@" + m_FormalParameter->DisplaySN(GetPriority(), p_DisplayVariableList) + "." + m_Expression->DisplaySN(GetPriority(), p_DisplayVariableList);
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

	SNI_Expression * SNI_Lambda::Clone(SNI_ReplacementList * p_ReplacementList, bool &p_Changed)
	{
		bool changed = false;
		SN::SN_Variable l_to = new SNI_Variable;
		l_to.SetName(dynamic_cast<SNI_Variable *>(m_FormalParameter)->GetName() + "_" + to_string(++m_Id));
		p_ReplacementList->push_back(SNI_Replacement(SN::SN_Variable(m_FormalParameter), l_to));
		SN::SN_Expression e_before = m_Expression;
		string before = e_before.DisplaySN();
		SNI_Expression * result = m_Expression->Clone(p_ReplacementList, changed);
		SN::SN_Expression e_after = result;
		string after = e_after.DisplaySN();
		p_ReplacementList->pop_back();

		if (changed)
		{
			p_Changed = true;
			return dynamic_cast<SNI_Expression *>(new SNI_Lambda(dynamic_cast<SNI_Expression *>(l_to.GetSNI_Variable()), result));
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
		m_FormalParameter->PartialAssertValue(param, true);
		SNI_Manager::GetTopManager()->DebugCommand(SN::CallPoint, 0, 0);
		if (p_ParameterList->size() > 0)
		{
			return m_Expression->Call(p_ParameterList, p_MetaLevel);
		}
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

		ASSERTM(p_ParameterList->size() > 1, "Cannot call a lambda without a parameter");
		SN::SN_Expression x = p_ParameterList->back();
		p_ParameterList->pop_back();
		SN::SN_Error e = SN::SN_Expression(m_FormalParameter).PartialAssertValue(x, true); // Lazy evaluation until needed.
		//SN::SN_Error e = SN::SN_Expression(x).AssertValue(m_FormalParameter); // Strict evaluation until needed.

		if (e.IsError())
		{
			e.AddNote(context, this, "Assigning parameter value failed");
			return SN::SN_Error(false);
		}
		SNI_Manager::GetTopManager()->DebugCommand(SN::ParameterPoint, 0, 0);
		if (p_ParameterList->size() > 1)
		{
			return m_Expression->Unify(p_ParameterList);
		}
		return m_Expression->AssertValue(p_ParameterList->back());
	}

	SN::SN_Error SNI_Lambda::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result)
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Lambda::PartialUnify ( " + DisplayPmParameterList(p_ParameterList) + " = " + p_Result.DisplaySN() + " )");

		ASSERTM(p_ParameterList->size() >0, "Cannot call a lambda without a parameter");
		SNI_Expression * x = p_ParameterList->front().GetValue().GetSNI_Expression();
		p_ParameterList->erase(p_ParameterList->begin());
		SN::SN_Error e = SN::SN_Expression(m_FormalParameter).PartialAssertValue(x);
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
