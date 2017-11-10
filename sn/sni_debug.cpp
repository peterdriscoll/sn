#include "sni_debug.h"

#include "sni_cart.h"

#include "logcontext.h"
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
	}

	SNI_Debug::~SNI_Debug()
	{
	}

	void SNI_Debug::PromoteMembers()
	{
	}

	string SNI_Debug::GetTypeName() const
	{
		return m_Expression->GetTypeName();
	}

	string SNI_Debug::DisplayCpp() const
	{
		return m_Expression->DisplayCpp();
	}

	string SNI_Debug::DisplaySN(long p_priority, SNI_VariablePointerList & p_DisplayVariableList) const
	{
		return m_Expression->DisplaySN(p_priority, p_DisplayVariableList) + ".Debug()";
	}

	long SNI_Debug::GetPriority() const
	{
		return m_Expression->GetPriority();
	}

	string SNI_Debug::GetOperator() const
	{
		return m_Expression->GetOperator();
	}

	SNI_Expression * SNI_Debug::Clone(SNI_ReplacementList * p_ReplacementList, bool &p_Changed)
	{
		SNI_Expression *l_clone = m_Expression->Clone(p_ReplacementList, p_Changed);
		if (p_Changed)
		{
			return new SNI_Debug(l_clone);
		}
		return this;
	}

	SN::SN_Error SNI_Debug::Assert()
	{
		SN::LogContext context(DisplaySN0() + ".Assert ()", LOG_DEBUG_INDEX);
		return LOG_RETURN(context, m_Expression->Assert());
	}

	SN::SN_Error SNI_Debug::AssertValue(const SN::SN_Expression &p_Value)
	{
		SN::LogContext context(DisplaySN0() + ".AssertValue (" + p_Value.DisplaySN() + ")", LOG_DEBUG_INDEX);
		return LOG_RETURN(context, m_Expression->AssertValue(p_Value));
	}

	SN::SN_Error SNI_Debug::AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World ** p_World, SNI_WorldSet * p_WorldSet)
	{
		SN::LogContext context(DisplaySN0() + ".AssertValue (" + p_Value.DisplaySN() + ")", LOG_DEBUG_INDEX);
		return LOG_RETURN(context, m_Expression->AddValue(p_Value, p_NumWorlds, p_World, p_WorldSet));
	}

	SN::SN_Expression SNI_Debug::Evaluate(long p_MetaLevel) const
	{
		SN::LogContext context(DisplaySN0() + ".Evaluate ()", LOG_DEBUG_INDEX);
		return LOG_RETURN(context, m_Expression->Evaluate(p_MetaLevel));
	}

	SN::SN_Expression SNI_Debug::PartialEvaluate(long p_MetaLevel) const
	{
		SN::LogContext context(DisplaySN0() + ".PartialEvaluate ()", LOG_DEBUG_INDEX);
		return LOG_RETURN(context, m_Expression->PartialEvaluate(p_MetaLevel).Debug());
	}

	SN::SN_Expression SNI_Debug::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		SN::LogContext context(DisplaySN0() + ".Call ( " + DisplayPmExpressionList(p_ParameterList) + " )", LOG_DEBUG_INDEX);

		return LOG_RETURN(context, m_Expression->Call(p_ParameterList, p_MetaLevel));
	}

	SN::SN_Expression SNI_Debug::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		SN::LogContext context(DisplaySN0() + ".PartialCall ( " + DisplayPmExpressionList(p_ParameterList) + " )", LOG_DEBUG_INDEX);

		return LOG_RETURN(context, m_Expression->PartialCall(p_ParameterList, p_MetaLevel).Debug());
	}

	SN::SN_Error SNI_Debug::Unify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result)
	{
		SN::LogContext context(DisplaySN0() + ".Unify ( " + DisplayPmParameterList(p_ParameterList) + " = " + p_Result.DisplaySN() + " )", LOG_DEBUG_INDEX);

		return LOG_RETURN(context, m_Expression->Unify(p_ParameterList, p_Result));
	}

	SN::SN_Error SNI_Debug::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result)
	{
		SN::LogContext context(DisplaySN0() + ".PartialUnify ( " + DisplayPmParameterList(p_ParameterList) + " = " + p_Result.DisplaySN() + " )", LOG_DEBUG_INDEX);

		return LOG_RETURN(context, m_Expression->PartialUnify(p_ParameterList, p_Result.Debug()));
	}

	SN::SN_Error SNI_Debug::ForEachCartUnify(long p_Depth, SNI_Cart * p_Cart)
	{
		return m_Expression->ForEachCartUnify(p_Depth, p_Cart);
	}

	void SNI_Debug::ForEachCall(SNI_Cartesian * p_Cart, long p_Depth)
	{
		m_Expression->ForEachCall(p_Cart, p_Depth);
	}

	void SNI_Debug::ForEachSplit(SNI_Splitter * p_Splitter)
	{
		m_Expression->ForEachSplit(p_Splitter);
	}
}
