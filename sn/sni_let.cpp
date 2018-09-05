#include "sni_let.h"

#include "logcontext.h"
#include "sn_expression.h"
#include "sn_parameter.h"
#include "sn_value.h"
#include "sn_variable.h"
#include "sn_error.h"

#include "sni_variable.h"
#include "sni_replacement.h"
#include "sni_error.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ long SNI_Let::m_Id = 0;

	SNI_Let::SNI_Let()
	{
	}

	SNI_Let::SNI_Let(SNI_Expression *p_Condition, SNI_Expression *p_Expression)
		: m_Condition(p_Condition)
		, m_Expression(p_Expression)
	{
		REQUESTPROMOTION(m_Condition);
		REQUESTPROMOTION(m_Expression);
	}

	SNI_Let::~SNI_Let()
	{
	}

	void SNI_Let::PromoteMembers()
	{
		REQUESTPROMOTION(m_Condition);
		REQUESTPROMOTION(m_Expression);
	}

	string SNI_Let::GetTypeName() const
	{
		return "Lambda";
	}

	string SNI_Let::DisplayCpp() const
	{
		return "sn_Lambda(" + m_Condition->DisplayCpp() + ", " + m_Expression->DisplayCpp() + ")";
	}

	string SNI_Let::DisplaySN(long /*priority*/, SNI_DisplayOptions &p_DisplayOptions) const
	{
		return "@" + m_Condition->DisplaySN(GetPriority(), p_DisplayOptions) + "." + m_Expression->DisplaySN(GetPriority(), p_DisplayOptions);
	}

	long SNI_Let::GetPriority() const
	{
		return 0;
	}

	string SNI_Let::GetOperator() const
	{
		return "@";
	}

	SNI_Expression * SNI_Let::GetExpression()
	{
		return m_Expression;
	}

	SNI_Expression * SNI_Let::GetCondition()
	{
		return m_Condition;
	}

	SNI_Expression * SNI_Let::Clone(SNI_Frame *p_Frame, bool &p_Changed)
	{
		bool changed = false;
		SNI_Expression * condition = m_Condition->Clone(p_Frame, changed);
		SNI_Expression * expression = m_Expression->Clone(p_Frame, changed);
		if (changed)
		{
			p_Changed = true;
			return dynamic_cast<SNI_Expression *>(new SNI_Let(condition, expression));
		}
		return this;
	}

	bool SNI_Let::Equivalent(SNI_Object * p_Other) const
	{
		if (dynamic_cast<SNI_Let *>(p_Other))
		{
			SNI_Let * l_lambda = dynamic_cast<SNI_Let *>(p_Other);
			return m_Condition->Equivalent(dynamic_cast<SNI_Object *>(l_lambda->m_Condition))
				&& m_Expression->Equivalent(dynamic_cast<SNI_Object *>(l_lambda->m_Expression));
		}
		return false;
	}

	SN::SN_Expression SNI_Let::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		SN::LogContext context(DisplaySN0() + "SNI_Let::Call ( " + DisplayPmExpressionList(p_ParameterList) + " )");
		SN::SN_Error e = m_Condition->Assert();
		if (!e.GetBool())
		{
			e.AddNote(context, this, "Let condition failed");
			return e;
		}
		if (p_ParameterList->size() > 0)
		{
			return m_Expression->Call(p_ParameterList, p_MetaLevel);
		}
		return m_Expression->Evaluate(p_MetaLevel);
	}

	SN::SN_Expression SNI_Let::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Let::PartialCall ( " + DisplayPmExpressionList(p_ParameterList) + " )");

		SN::SN_Error e = m_Condition->PartialAssert();
		if (e.IsError())
		{
			e.AddNote(context, this, "Let condition failed");
			return e;
		}
		if (p_ParameterList->size() > 0)
		{
			return m_Expression->PartialCall(p_ParameterList, p_MetaLevel);
		}
		return m_Expression->PartialEvaluate(p_MetaLevel);
	}

	SN::SN_Expression SNI_Let::Unify(SN::SN_ExpressionList * p_ParameterList)
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Let::Unify ( " + DisplayPmExpressionList(p_ParameterList) + " )");

		SN::SN_Error e = SN::SN_Expression(m_Condition).Assert();
		if (e.IsError())
		{
			e.AddNote(context, this, "Let condition failed");
			return e;
		}
		if (p_ParameterList->size() > 0)
		{
			return m_Expression->Unify(p_ParameterList);
		}
		return m_Expression->AssertValue((*p_ParameterList)[0]);
	}

	SN::SN_Error SNI_Let::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result)
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Let::PartialUnify ( " + DisplayPmParameterList(p_ParameterList) + " = " + p_Result.DisplaySN() + " )");

		SN::SN_Error e = SN::SN_Expression(m_Condition).PartialAssert();
		if (e.IsError())
		{
			e.AddNote(context, this, "Let condition failed");
			return e;
		}
		if (p_ParameterList->size() > 0)
		{
			return m_Expression->PartialUnify(p_ParameterList, p_Result);
		}
		return m_Expression->PartialAssertValue(p_Result);
	}
}
