#include "sni_define.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sn_error.h"

#include "sni_variable.h"
#include "sni_User.h"
#include "sni_Thread.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ SNI_Class* SNI_Define::Class()
	{
		return SNI_User::GetCurrentUser()->GetOrCreatePointer<SNI_Define, SNI_Class>("Define");
	}

	SNI_Class* SNI_Define::VClass()
	{
		return Class();
	}

	SNI_Define::SNI_Define()
		: m_Variable(NULL)
		, m_DefineType(skynet::Inname)
	{
	}

	SNI_Define::SNI_Define(SNI_Variable *p_Variable, enum skynet::DefineType p_DefineType)
		: m_Variable(p_Variable)
		, m_DefineType(p_DefineType)
	{
		REQUESTPROMOTION(m_Variable);
	}

	SNI_Define::~SNI_Define()
	{
	}

	void SNI_Define::PromoteMembers()
	{
		REQUESTPROMOTION(m_Variable);
	}

	std::string SNI_Define::GetTypeName() const
	{
		return "Define";
	}

	std::string SNI_Define::DisplayCpp() const
	{
		return "sn_Define(" + m_Variable->DisplayCpp() + ")";
	}

	std::string SNI_Define::DisplaySN(long /*priority*/, SNI_DisplayOptions &p_DisplayOptions) const
	{
		return "$" + m_Variable->DisplaySN(GetPriority(), p_DisplayOptions);
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

	SN::SN_Expression SNI_Define::DoEvaluate(long p_MetaLevel /* = 0 */) const
	{
		return m_Variable->DoEvaluate(p_MetaLevel);
	}

	SN::SN_Expression SNI_Define::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Define::Call ( " + DisplaySnExpressionList(p_ParameterList) + " )"));

		ASSERTM(m_Variable && dynamic_cast<SNI_Variable *>(m_Variable), "The defined expression must be a variable.");

		return LOG_RETURN(context, m_Variable->Call(p_ParameterList, p_MetaLevel));
	}

	SN::SN_Expression SNI_Define::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Define::PartialCall ( " + DisplaySnExpressionList(p_ParameterList) + " )"));

		ASSERTM(m_Variable && dynamic_cast<SNI_Variable *>(m_Variable), "The defined expression must be a variable.");
		SN::SN_Expression result(this);
		while (!p_ParameterList->empty())
		{
			result = SN::SN_Function(result, p_ParameterList->back().DoPartialEvaluate(p_MetaLevel));
			p_ParameterList->pop_back();
		}
		return LOG_RETURN(context, result);
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

	SN::SN_Error SNI_Define::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		return m_Variable->PartialUnify(p_ParameterList, p_Result, true);
	}

	SNI_Variable* SNI_Define::GetVariable()
	{
		return m_Variable;
	}
}
