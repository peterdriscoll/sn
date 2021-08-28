#include "sni_debug.h"

#include "sni_cart.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sn_expression.h"
#include "sn_value.h"
#include "sn_error.h"

#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Debug::SNI_Debug(const SN::SN_Expression &p_Expression)
		: m_Expression(p_Expression.GetSNI_Expression())
	{
		REQUESTPROMOTION(m_Expression);
	}

	SNI_Debug::~SNI_Debug()
	{
	}

	void SNI_Debug::PromoteMembers()
	{
		REQUESTPROMOTION(m_Expression);
	}

	string SNI_Debug::GetTypeName() const
	{
		return m_Expression->GetTypeName();
	}

	string SNI_Debug::DisplayCpp() const
	{
		return m_Expression->DisplayCpp();
	}

	string SNI_Debug::DisplaySN(long p_priority, SNI_DisplayOptions & p_DisplayOptions) const
	{
		return m_Expression->DisplaySN(p_priority, p_DisplayOptions) + ".Debug()";
	}

	void SNI_Debug::AddVariables(long p_MetaLevel, SNI_VariablePointerMap& p_Map)
	{
		SNI_Expression* expression = m_Expression;
		if (expression)
		{
			expression->AddVariables(p_MetaLevel, p_Map);
		}
	}

	long SNI_Debug::GetPriority() const
	{
		return m_Expression->GetPriority();
	}

	string SNI_Debug::GetOperator() const
	{
		return m_Expression->GetOperator();
	}

	SNI_Expression * SNI_Debug::Clone(long p_MetaLevel, SNI_Frame *p_Frame, bool &p_Changed)
	{
		SNI_Expression *l_clone = m_Expression->Clone(p_MetaLevel, p_Frame, p_Changed);
		if (p_Changed)
		{
			return new SNI_Debug(l_clone);
		}
		return this;
	}

	SN::SN_Error SNI_Debug::DoAssert()
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".DoAssert ()", LOG_DEBUG_INDEX));

		return LOG_RETURN(context, m_Expression->DoAssert());
	}

	SN::SN_Error SNI_Debug::AssertValue(const SN::SN_Expression &p_Value)
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".AssertValue (" + p_Value.DisplaySN() + ")", LOG_DEBUG_INDEX));

		return LOG_RETURN(context, m_Expression->AssertValue(p_Value));
	}

	SN::SN_Error SNI_Debug::AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World ** p_World, SNI_WorldSet * p_WorldSet)
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".AssertValue (" + p_Value.DisplaySN() + ")", LOG_DEBUG_INDEX));

		return LOG_RETURN(context, m_Expression->AddValue(p_Value, p_NumWorlds, p_World, p_WorldSet));
	}

	SN::SN_Expression SNI_Debug::DoEvaluate(long p_MetaLevel) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".Evaluate ()", LOG_DEBUG_INDEX));
		
		return LOG_RETURN(context, m_Expression->DoEvaluate(p_MetaLevel));
	}

	SN::SN_Expression SNI_Debug::DoPartialEvaluate(long p_MetaLevel) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".DoPartialEvaluate ()", LOG_DEBUG_INDEX));

		return LOG_RETURN(context, m_Expression->DoPartialEvaluate(p_MetaLevel).Debug());
	}

	SN::SN_Expression SNI_Debug::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".Call ( " + DisplaySnExpressionList(p_ParameterList) + " )", LOG_DEBUG_INDEX));

		return LOG_RETURN(context, m_Expression->Call(p_ParameterList, p_MetaLevel));
	}

	SN::SN_Expression SNI_Debug::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".PartialCall ( " + DisplaySnExpressionList(p_ParameterList) + " )", LOG_DEBUG_INDEX));

		return LOG_RETURN(context, m_Expression->PartialCall(p_ParameterList, p_MetaLevel).Debug());
	}

	SN::SN_Expression SNI_Debug::Unify(SN::SN_ExpressionList * p_ParameterList)
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".Unify ( " + DisplaySnExpressionList(p_ParameterList) + " )", LOG_DEBUG_INDEX));

		return LOG_RETURN(context, m_Expression->Unify(p_ParameterList));
	}

	SN::SN_Error SNI_Debug::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".PartialUnify ( " + DisplaySnParameterList(p_ParameterList) + " = " + p_Result.DisplaySN() + " )", LOG_DEBUG_INDEX));

		return LOG_RETURN(context, m_Expression->PartialUnify(p_ParameterList, p_Result.Debug()));
	}

	SN::SN_Error SNI_Debug::ForEachCart(long p_Depth, SNI_Cart * p_Cart)
	{
		return m_Expression->ForEachCart(p_Depth, p_Cart);
	}

	void SNI_Debug::ForEachSplit(SNI_Splitter * p_Splitter)
	{
		m_Expression->ForEachSplit(p_Splitter);
	}
}
