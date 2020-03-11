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

// Collapse returns true if the expression could be true. It returns false otherwise.

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

	string SNI_Collapse::DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, size_t p_NumParams, SN::SN_Expression * p_ParamList, const SNI_Expression * p_DebugSource) const
	{
		return Bracket(priority, SetBreakPoint(GetTypeName(), p_DisplayOptions, p_DebugSource, SN::ParameterOneId) + " " + p_ParamList[PC1_First].GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions), p_DisplayOptions, p_DebugSource);
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

	SN::SN_Expression SNI_Collapse::UnifyArray(SN::SN_Expression * p_ParamList, const SNI_Expression *p_Source)
	{
		SN::SN_Error e = UnifyInternal(p_ParamList[1], p_ParamList[0], p_Source);
		if (e.GetDelay())
		{
			SNI_Thread::GetThread()->GetProcessor()->Delay(SN::SN_FunctionDef(dynamic_cast<SNI_FunctionDef*>(this)), GetNumParameters(), p_ParamList, p_Source);
		}
		return e;
	}

	SN::SN_Error SNI_Collapse::UnifyInternal(SN::SN_Expression &p_value, SN::SN_Expression &p_Result, const SNI_Expression *p_Source)
	{
		LOGGING(SN::LogContext context("SNI_Unary::UnifyInternal ( " + p_value.DisplaySN() + " )"));

		SNI_Frame::Push(this, NULL);
		LOG(WriteHeading(SN::DebugLevel, GetTypeName() + ": Start " + p_Result.DisplayValueSN() + "=" + GetTypeName() + " " + p_value.DisplayValueSN()));
		SNI_Frame *topFrame = SNI_Frame::Top();
		SNI_Variable *resultParam = topFrame->CreateParameterByName("result", p_Result); // Result.
		SNI_Variable *valueParam = topFrame->CreateParameterByName("param", p_value); // Param 1.

		Breakpoint(SN::DebugStop, SN::LeftId, GetTypeName(), "Unify before collapse", p_Source, SN::CallPoint);

		SN::SN_Error err = skynet::OK;
		if (SNI_Thread::TopManager()->GetLogicType() != skynet::FailAsNeg)
		{
			err = p_value.AssertValue(p_Result);

			valueParam->SetValue(p_Result);
			Breakpoint(SN::DebugStop, SN::CallId, GetTypeName(), "Unify before collapse", p_Source, SN::CallPoint);

			resultParam->SetValue(p_Result.GetVariableValue().GetSNI_Expression()->DoCollapse());
		}
		else
		{
			SNI_WorldSet *worldSet = new SNI_WorldSet();
			SNI_World *successWorld = worldSet->CreateWorld();
			SNI_World *failWorld = worldSet->CreateWorld();
			worldSet->Complete();

			SNI_Thread::GetThread()->PushContextWorld(successWorld);
			SN::SN_Error e = p_value.DoAssert();
			SNI_Thread::GetThread()->PopContextWorld();
			if (e.IsError())
			{
				successWorld->Fail(FailedInCollapse);
				worldSet->CheckDependentWorlds();
				err = p_Result.AssertValue(skynet::False);
			}
			else
			{
				failWorld->Fail(FailedInCollapse);
				worldSet->CheckDependentWorlds();
				err = p_Result.AssertValue(skynet::True);
			}

			valueParam->SetValue(p_Result);
		}

		LOG(WriteHeading(SN::DebugLevel, GetTypeName() + ": End " + p_Result.DisplayValueSN() + "=" + GetTypeName() + " " + p_value.DisplayValueSN()));
		Breakpoint(SN::DebugStop, SN::RightId, GetTypeName(), "Unify after collapse", p_Source, SN::CallPoint);
		SNI_Frame::Pop();
		return LOG_RETURN(context, err);
	}
}
