#include "sni_let.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

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

	/*static*/ SNI_Class* SNI_Let::Class()
	{
		return SNI_User::GetCurrentUser()->GetOrCreatePointer<SNI_Let, SNI_Class>("Let");
	}

	SNI_Class* SNI_Let::VClass()
	{
		return Class();
	}

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

	std::string SNI_Let::GetTypeName() const
	{
		return "Let";
	}

	std::string SNI_Let::DisplayCpp() const
	{
		return "Let(" + m_Condition->DisplayCpp() + ", " + m_Expression->DisplayCpp() + ")";
	}

	std::string SNI_Let::DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const
	{
		std::string text = SetBreakPoint("let", p_DisplayOptions, this, SN::LeftId) + " " + m_Condition->DisplaySN(GetPriority(), p_DisplayOptions) + " " + SetBreakPoint("in", p_DisplayOptions, this, SN::CallId) + " " + m_Expression->DisplaySN(GetPriority(), p_DisplayOptions);
		return Bracket(priority, text, p_DisplayOptions, this);
	}

	void SNI_Let::AddVariables(long p_MetaLevel, SNI_VariablePointerMap& p_Map)
	{
		SNI_Expression* condition = m_Condition;
		if (condition)
		{
			condition->AddVariables(p_MetaLevel, p_Map);
		}
		SNI_Expression* expression = m_Expression;
		if (expression)
		{
			expression->AddVariables(p_MetaLevel, p_Map);
		}
	}

	long SNI_Let::GetPriority() const
	{
		return 0;
	}

	std::string SNI_Let::GetOperator() const
	{
		return "let";
	}

	SNI_Expression * SNI_Let::GetExpression()
	{
		return m_Expression;
	}

	SNI_Expression * SNI_Let::GetCondition()
	{
		return m_Condition;
	}

	SNI_Expression * SNI_Let::Clone(long p_MetaLevel, SNI_Frame *p_Frame, bool &p_Changed)
	{
		bool changed = false;
		SNI_Expression * condition = m_Condition->Clone(p_MetaLevel, p_Frame, changed);
		SNI_Expression * expression = m_Expression->Clone(p_MetaLevel, p_Frame, changed);
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

	SN::SN_Expression SNI_Let::DoEvaluate(long p_MetaLevel /* = 0 */) const
	{
		if (0 < p_MetaLevel)
		{
			SN::SN_Expression condition = m_Condition;
			SN::SN_Expression expression = m_Expression;
			return SN::SN_Let(condition.DoEvaluate(p_MetaLevel), expression.DoEvaluate(p_MetaLevel));
		}

		SNI_Frame* topFrame = SNI_Frame::Push(this, NULL);

		SNI_DisplayOptions displayOptions(doTextOnly);
		LOG(WriteHeading(SN::DebugLevel, GetTypeName() + ": Start " + DisplaySN(0, displayOptions)));

		SNI_Variable* condition_param = topFrame->CreateParameterByName("Condition", m_Condition);
		SNI_Variable* expression_param = topFrame->CreateParameterByName("Expression", m_Expression);
		Breakpoint(SN::DebugStop, SN::LeftId, GetTypeName(), "Assert value before condition check", this, SN::CallPoint);

		SN::SN_Error e = m_Condition->DoAssert();
		SN::SN_Expression result = e;

		condition_param->SetValue(e);
		Breakpoint(SN::DebugStop, SN::CallId, GetTypeName(), "Assert value before call", this, SN::CallPoint);

		if (e.IsError())
		{
			if (e.IsSignificantError())
			{
				SNI_CallRecord* callRecord = new SNI_CallRecord("Asserting let condition.", this);
				LOGGING(callRecord->SetLogContext(context));
				e.GetSNI_Error()->AddNote(callRecord);
			}
		}
		else
		{
			result = m_Expression->DoEvaluate(p_MetaLevel);
			expression_param->SetValue(result);
			topFrame->GetResult()->SetValue(result);
		}
		Breakpoint(SN::DebugStop, SN::RightId, GetTypeName(), "Assert value after call", this, SN::CallPoint);
		SNI_Frame::Pop();
		return result;
	}

	SN::SN_Expression SNI_Let::DoPartialEvaluate(long p_MetaLevel /* = 0 */) const
	{
		SN::SN_Expression condition = m_Condition->DoPartialEvaluate(p_MetaLevel);
		SN::SN_Expression expression = m_Expression->DoPartialEvaluate(p_MetaLevel);
		return SN::SN_Let(condition, expression);
	}

	SN::SN_Error SNI_Let::DoPartialAssert()
	{
		return PartialAssertValue(skynet::True);
	}

	SN::SN_Error SNI_Let::PartialAssertValue(const SN::SN_Expression& p_Expression, bool p_Define /* = false */)
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Let::PartialAssertValue ( " + p_Value.DisplaySN() + " )"));

		SN::SN_Error e = m_Condition->DoPartialAssert();

		if (e.IsError())
		{
			if (e.IsSignificantError())
			{
				SNI_CallRecord* callRecord = new SNI_CallRecord("Asserting let condition.", this);
				LOGGING(callRecord->SetLogContext(context));
				e.GetSNI_Error()->AddNote(callRecord);
			}
		}
		else
		{
			e = m_Expression->PartialAssertValue(p_Expression, p_Define);
		}
		return e;
	}

	SN::SN_Error SNI_Let::DoAssert()
	{
		return AssertValue(skynet::True);
	}

	SN::SN_Error SNI_Let::AssertValue(const SN::SN_Expression &p_Value)
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Let::AssertValue ( " + p_Value.DisplaySN() + " )"));

		SNI_Frame *topFrame = SNI_Frame::Push(this, NULL);

		SNI_DisplayOptions displayOptions(doTextOnly);
		LOG(WriteHeading(SN::DebugLevel, GetTypeName() + ": Start " + DisplaySN(0, displayOptions)));

		SNI_Variable* condition_param = topFrame->CreateParameterByName("Condition", m_Condition);
		SNI_Variable* expression_param = topFrame->CreateParameterByName("Expression", m_Expression);
		Breakpoint(SN::DebugStop, SN::LeftId, GetTypeName(), "Assert value before condition check", this, SN::CallPoint);

		SN::SN_Error e = m_Condition->DoAssert();

		condition_param->SetValue(e);
		Breakpoint(SN::DebugStop, SN::CallId, GetTypeName(), "Assert value before call", this, SN::CallPoint);

		if (e.IsError())
		{
			if (e.IsSignificantError())
			{
				SNI_CallRecord *callRecord = new SNI_CallRecord("Asserting let condition.", this);
				LOGGING(callRecord->SetLogContext(context));
				e.GetSNI_Error()->AddNote(callRecord);
			}
		}
		else
		{
			e = m_Expression->AssertValue(p_Value);
			expression_param->SetValue(p_Value);
		}
		LOG(WriteHeading(SN::DebugLevel, GetTypeName() + ": End " + DisplaySN(0, displayOptions)));
		Breakpoint(SN::DebugStop, SN::RightId, GetTypeName(), "Assert value after call", this, SN::CallPoint);
		SNI_Frame::Pop();
		return e;
	}

	SN::SN_Expression SNI_Let::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + "SNI_Let::Call ( " + DisplaySnExpressionList(p_ParameterList) + " )"));

		SN::SN_Error e = m_Condition->DoAssert();
		if (e.IsError())
		{
			if (e.IsSignificantError())
			{
				SNI_CallRecord *callRecord = new SNI_CallRecord("Calling let condition.", this);
				LOGGING(callRecord->SetLogContext(context));
				e.GetSNI_Error()->AddNote(callRecord);
			}
			return e;
		}
		if (p_ParameterList->size() > 0)
		{
			return m_Expression->Call(p_ParameterList, p_MetaLevel);
		}
		return m_Expression->DoEvaluate(p_MetaLevel);
	}

	SN::SN_Expression SNI_Let::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Let::PartialCall ( " + DisplaySnExpressionList(p_ParameterList) + " )"));

		SN::SN_Error e = m_Condition->DoPartialAssert();
		if (e.IsError())
		{
			if (e.IsSignificantError())
			{
				SNI_CallRecord *callRecord = new SNI_CallRecord("Partial calling let condition.", this);
				LOGGING(callRecord->SetLogContext(context));
				e.GetSNI_Error()->AddNote(callRecord);
			}
			return e;
		}
		if (p_ParameterList->size() > 0)
		{
			return m_Expression->PartialCall(p_ParameterList, p_MetaLevel);
		}
		return m_Expression->DoPartialEvaluate(p_MetaLevel);
	}

	SN::SN_Expression SNI_Let::Unify(SN::SN_ExpressionList * p_ParameterList)
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Let::Unify ( " + DisplaySnExpressionList(p_ParameterList) + " )"));

		SNI_Frame *topFrame = SNI_Frame::Push(this, NULL);

		LOG(WriteHeading(SN::DebugLevel, GetTypeName() + ": Start " + DisplayUnifyExp(p_ParameterList)));
		SNI_Variable* condition_param = topFrame->CreateParameterByName("Condition", m_Condition);
		Breakpoint(SN::DebugStop, SN::LeftId, GetTypeName(), "Unify before condition check", this, SN::CallPoint);

		SN::SN_Expression result = SN::SN_Expression(m_Condition).DoAssert();

		condition_param->SetValue(m_Condition);

		Breakpoint(SN::DebugStop, SN::CallId, GetTypeName(), "Unify before call", this, SN::CallPoint);

		if (result.IsError())
		{
			if (result.IsSignificantError())
			{
				SNI_CallRecord *callRecord = new SNI_CallRecord("Unifying let condition.", this);
				LOGGING(callRecord->SetLogContext(context));
				result.GetSNI_Error()->AddNote(callRecord);
			}
			return result;
		}
		if (p_ParameterList->size() > 0)
		{
			result = m_Expression->Unify(p_ParameterList);
		}
		else
		{
			result = m_Expression->AssertValue((*p_ParameterList)[0]);
		}

		Breakpoint(SN::DebugStop, SN::RightId, GetTypeName(), "Unify after call", this, SN::CallPoint);
		LOG(WriteHeading(SN::DebugLevel, GetTypeName() + ": End " + DisplayUnifyExp(p_ParameterList)));
		SNI_Frame::Pop();
		return result;
	}

	SN::SN_Error SNI_Let::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Let::PartialUnify ( " + DisplaySnParameterList(p_ParameterList) + " = " + p_Result.DisplaySN() + " )"));

		SN::SN_Error e = SN::SN_Expression(m_Condition).DoPartialAssert();
		if (e.IsError())
		{
			if (e.IsSignificantError())
			{
				SNI_CallRecord *callRecord = new SNI_CallRecord("Partial unify of let condition.", this);
				LOGGING(callRecord->SetLogContext(context));
				e.GetSNI_Error()->AddNote(callRecord);
			}
			return e;
		}
		if (p_ParameterList->size() > 0)
		{
			return m_Expression->PartialUnify(p_ParameterList, p_Result);
		}
		return m_Expression->PartialAssertValue(p_Result);
	}

	SN::SN_Error SNI_Let::AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World ** p_WorldList, SNI_WorldSet * p_WorldSet)
	{
		ASSERTM(p_WorldList == 0, "Called only with a cardinality of 1, so no worlds.");
		if (p_Value.IsKnownValue() || p_Value.IsReferableValue())
		{
			return AssertValue(p_Value);
		}
		return skynet::OK;
	}
}
