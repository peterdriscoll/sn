#include "sni_define.h"

#include "logcontext.h"
#include "sn_error.h"

#include "sni_variable.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Define::SNI_Define()
	{
	}

	SNI_Define::SNI_Define(SNI_Variable *p_Variable)
		: m_Variable(p_Variable)
	{
	}

	SNI_Define::~SNI_Define()
	{
	}

	void SNI_Define::PromoteMembers()
	{
	}

	string SNI_Define::GetTypeName() const
	{
		return "Define";
	}

	string SNI_Define::DisplayCpp() const
	{
		return "sn_Define(" + m_Variable->DisplayCpp() + ")";
	}

	string SNI_Define::DisplaySN(long /*priority*/, SNI_VariableConstPointerList &p_DisplayVariableList) const
	{
		return "$" + m_Variable->DisplaySN(GetPriority(), p_DisplayVariableList);
	}

	long SNI_Define::GetPriority() const
	{
		return 10;
	}

	bool SNI_Define::Equivalent(SNI_Object * p_Other) const
	{
		if (dynamic_cast<SNI_Define *>(p_Other))
		{
			SNI_Define * other = dynamic_cast<SNI_Define *>(p_Other);

			return m_Variable->Equivalent(dynamic_cast<SNI_Object *>(other->m_Variable));
		}
		return false;
	}

	SN::SN_Expression SNI_Define::Evaluate(long p_MetaLevel /* = 0 */) const
	{
		return m_Variable->Evaluate(p_MetaLevel);
	}

	SN::SN_Expression SNI_Define::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Define::Call ( " + DisplayPmExpressionList(p_ParameterList) + " )");

		ASSERTM(m_Variable && dynamic_cast<SNI_Variable *>(m_Variable), "The defined expression must be a variable.");
		return m_Variable->Call(p_ParameterList, p_MetaLevel);
	}

	SN::SN_Expression SNI_Define::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Define::PartialCall ( " + DisplayPmExpressionList(p_ParameterList) + " )");

		ASSERTM(m_Variable && dynamic_cast<SNI_Variable *>(m_Variable), "The defined expression must be a variable.");
		return m_Variable->PartialCall(p_ParameterList, p_MetaLevel);
	}

	SN::SN_Error SNI_Define::PartialAssertValue(const SN::SN_Expression &p_Expression, bool /* p_Define = false */)
	{
		ASSERTM(m_Variable && dynamic_cast<SNI_Variable *>(m_Variable), "The defined expression must be a variable.");
		return m_Variable->PartialAssertValue(p_Expression, true);
	}

	SN::SN_Expression SNI_Define::Unify(SN::SN_ExpressionList * p_ParameterList)
	{
		return m_Variable->Unify(p_ParameterList);
	}

	SN::SN_Error SNI_Define::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result)
	{
		return m_Variable->PartialUnify(p_ParameterList, p_Result);
	}

	SNI_Variable* SNI_Define::GetVariable()
	{
		return m_Variable;
	}
}
