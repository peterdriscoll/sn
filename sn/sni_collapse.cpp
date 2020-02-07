#include "sni_collapse.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sn_expression.h"
#include "sn_parameter.h"
#include "sn_functiondef.h"
#include "sn_error.h"

#include "sni_delayedprocessor.h"
#include "sni_variable.h"
#include "sni_world.h"
#include "sni_helpers.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Collapse::SNI_Collapse()
	{

	}

	SNI_Collapse::~SNI_Collapse()
	{

	}

	string SNI_Collapse::GetTypeName() const
	{
		return "Collapse";
	}

	string SNI_Collapse::DisplayCpp() const
	{
		return "SN::Collapse";
	}

	string SNI_Collapse::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return GetTypeName();
	}

	string SNI_Collapse::DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, SN::SN_ExpressionList * p_ParameterList, const SNI_Expression * p_DebugSource) const
	{
		return Bracket(priority, SetBreakPoint(GetTypeName(), p_DisplayOptions, p_DebugSource, SN::ParameterOneId) + " " + (*p_ParameterList)[0].GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions), p_DisplayOptions, p_DebugSource);
	}

	long SNI_Collapse::GetPriority() const
	{
		return 4;
	}

	SN::SN_Value SNI_Collapse::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.DoCollapse();
	}

	SN::SN_Expression SNI_Collapse::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.Collapse();
	}

	SN::SN_Value SNI_Collapse::InverseFunctionValue(const SN::SN_Value & p_Param) const
	{
		return p_Param;
	}

	SN::SN_Expression SNI_Collapse::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return p_Param.Collapse();
	}

	SN::SN_Expression SNI_Collapse::UnifyArray(SN::SN_Expression * p_ParamList)
	{
		SN::SN_Error e = UnifyInternal(p_ParamList[1], p_ParamList[0]);
		if (e.GetDelay())
		{
			SNI_DelayedProcessor::GetProcessor()->Delay(SN::SN_FunctionDef(dynamic_cast<SNI_FunctionDef*>(this)), p_ParamList);
		}
		return e;
	}

	SN::SN_Error SNI_Collapse::UnifyInternal(SN::SN_Expression &p_value, SN::SN_Expression &p_Result)
	{
		LOGGING(SN::LogContext context("SNI_Unary::UnifyInternal ( " + p_value.DisplaySN() + " )"));

		SNI_Frame::Push(this, NULL);
		SNI_Frame *topFrame = SNI_Frame::Top();
		SNI_Variable *resultParam = topFrame->CreateParameterByName("result", p_Result); // Result.
		SNI_Variable *valueParam = topFrame->CreateParameterByName("param", p_value); // Param 1.

		SNI_Thread::GetThread()->DebugCommand(SN::CallPoint, GetTypeName() + ".Unify before assert", SN::LeftId);

		SNI_Thread::GetThread()->SetDebugId(GetDebugId()+"AssertValue");
		SN::SN_Error err = p_value.AssertValue(p_Result);

		valueParam->SetValue(p_Result);
		SNI_Thread::GetThread()->DebugCommand(SN::CallPoint, GetTypeName() + ".Unify before collapse", SN::ParameterOneId);

		resultParam->SetValue(p_Result.GetVariableValue().GetSNI_Expression()->DoCollapse());
		SNI_Thread::GetThread()->DebugCommand(SN::CallPoint, GetTypeName() + ".Unify after collapse", SN::RightId);
		SNI_Frame::Pop();
		return LOG_RETURN(context, err);
	}
}
