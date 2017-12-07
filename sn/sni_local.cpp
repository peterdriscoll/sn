#include "sni_local.h"

#include "logcontext.h"
#include "sn_value.h"
#include "sn_variable.h"
#include "sn_error.h"

#include "sni_variable.h"
#include "sni_replacement.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ long SNI_Local::m_Id = 0;

	SNI_Local::SNI_Local()
		: m_LocalVariable(NULL)
		, m_Expression(NULL)
	{

	}

	SNI_Local::SNI_Local(SNI_Variable *p_LocalVariable, SNI_Expression *p_Expression)
		: m_LocalVariable(p_LocalVariable)
		, m_Expression(p_Expression)
	{
	}

	SNI_Local::~SNI_Local()
	{

	}

	void SNI_Local::PromoteMembers()
	{
	}

	string SNI_Local::GetTypeName() const
	{
		return "Local";
	}

	string SNI_Local::DisplayCpp() const
	{
		return "sn_Local(" + m_LocalVariable->DisplayCpp() + ", " + m_Expression->DisplayCpp() + ")";
	}

	string SNI_Local::DisplaySN(long /*priority*/, SNI_VariablePointerList &p_DisplayVariableList) const
	{
		return "#" + m_LocalVariable->DisplaySN(GetPriority(), p_DisplayVariableList) + "." + m_Expression->DisplaySN(GetPriority(), p_DisplayVariableList);
	}

	long SNI_Local::GetPriority() const
	{
		return 0;
	}

	string SNI_Local::GetOperator() const
	{
		return "#";
	}

	SNI_Expression * SNI_Local::GetExpression()
	{
		return m_Expression;
	}

	SNI_Variable* SNI_Local::GetLocalVariable()
	{
		return m_LocalVariable;
	}

	SNI_Expression * SNI_Local::Clone(SNI_Frame *p_Frame, bool &p_Changed)
	{
		bool changed = false;
		SNI_Variable *l_from = new SNI_Variable;
		l_from->SetName(dynamic_cast<SNI_Variable *>(m_LocalVariable)->GetName() + "_" + to_string(++m_Id));
		SNI_Variable *l_LocalVariable = dynamic_cast<SNI_Variable *>(m_LocalVariable);
		l_from->SetName(l_LocalVariable->GetName() + "_" + to_string(++m_Id));
		SNI_Expression *result = p_Frame->CloneReplace(p_Changed, l_LocalVariable, l_from, m_Expression);
		if (changed)
		{
			p_Changed = true;
			return dynamic_cast<SNI_Expression *>(new SNI_Local(l_from, result));
		}
		return this;
	}

	SN::SN_Expression SNI_Local::Evaluate(long p_MetaLevel /* = 0 */) const
	{
		return m_Expression->Evaluate(p_MetaLevel);
	}

	SN::SN_Expression SNI_Local::PartialEvaluate(long p_MetaLevel /* = 0 */) const
	{
		return m_Expression->PartialEvaluate(p_MetaLevel);
	}

	SN::SN_Error SNI_Local::Assert()
	{
		return AssertValue(skynet::True);
	}

	SN::SN_Error SNI_Local::AssertValue(const SN::SN_Expression &p_Value)
	{
		return m_Expression->AssertValue(p_Value);
	}

	SN::SN_Error SNI_Local::PartialAssert()
	{
		return PartialAssertValue(skynet::True);
	}

	SN::SN_Error SNI_Local::PartialAssertValue(const SN::SN_Expression &p_Expression, bool p_Define /* = false */)
	{
		return m_Expression->PartialAssertValue(p_Expression, p_Define);
	}

	bool SNI_Local::Equivalent(SNI_Object * p_Other) const
	{
		if (dynamic_cast<SNI_Local *>(p_Other))
		{
			SNI_Local * l_local = dynamic_cast<SNI_Local *>(p_Other);
			return m_LocalVariable->Equivalent(dynamic_cast<SNI_Object *>(l_local->m_LocalVariable))
				&& m_Expression->Equivalent(dynamic_cast<SNI_Object *>(l_local->m_Expression));
		}
		return false;
	}

	SN::SN_Expression SNI_Local::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Local::Call ( " + DisplayPmExpressionList(p_ParameterList) + " )");

		return m_Expression->Call(p_ParameterList, p_MetaLevel);
	}

	SN::SN_Expression SNI_Local::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel  /* = 0 */) const
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Local::PartialCall ( " + DisplayPmExpressionList(p_ParameterList) + " )");

		return m_Expression->PartialCall(p_ParameterList, p_MetaLevel);
	}

	SN::SN_Expression SNI_Local::Unify(SN::SN_ExpressionList * p_ParameterList)
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Local::Unify ( " + DisplayPmExpressionList(p_ParameterList) + " )");

		return m_Expression->AssertValue((*p_ParameterList)[0]);
	}

	SN::SN_Error SNI_Local::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result)
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Local::PartialUnify ( " + DisplayPmParameterList(p_ParameterList) + " = " + p_Result.DisplaySN() + " )");

		return m_Expression->PartialUnify(p_ParameterList, p_Result);
	}
}
