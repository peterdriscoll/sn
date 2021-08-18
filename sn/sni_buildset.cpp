#include "sni_buildset.h"

#include "sn.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sn_parameter.h"

#include "sni_helpers.h"
#include "sni_error.h"
#include "sni_null.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_BuildSet::SNI_BuildSet()
	{
	}

	SNI_BuildSet::~SNI_BuildSet()
	{
	}

	string SNI_BuildSet::GetTypeName() const
	{
		return "BuildSet";
	}

	string SNI_BuildSet::DisplayCpp() const
	{
		return "SN::BuildSet";
	}

	string SNI_BuildSet::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "Set";
	}

	long SNI_BuildSet::GetPriority() const
	{
		return 0;
	}
	
	SN::SN_Value SNI_BuildSet::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoBuildSet();
	}

	SN::SN_Value SNI_BuildSet::InverseFunctionValue(const SN::SN_Value &p_Param) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_BuildSet::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_BuildSet::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel  /* = 0 */) const
	{
		LOGGING(SN::LogContext context("SNI_Unary::PartialCall ( " + DisplaySnExpressionList(p_ParameterList) + " )"));

		SN::SN_Value value = (*p_ParameterList)[0].DoEvaluate(p_MetaLevel);
		if (value.IsNull())
		{
			return LOG_RETURN(context, value);
		}

		if (0 < p_MetaLevel)
		{
			return LOG_RETURN(context, PrimaryFunctionExpressionOp(value));
		}

		return LOG_RETURN(context, PrimaryFunctionValue(value));
	}

	SN::SN_Expression SNI_BuildSet::UnifyArray(SN::SN_Expression * p_ParamList, const SNI_Expression *p_Source)
	{
		SNI_Frame::Push(this, NULL);
		SNI_Frame* topFrame = SNI_Frame::Top();
		topFrame->CreateParameter(PU1_Result, p_ParamList[PU1_Result]);
		topFrame->CreateParameter(PU1_First, p_ParamList[PU1_First]);
		
		Breakpoint(SN::DebugStop, SN::LeftId, GetTypeName(), "Unify before checking parameter cardinality", p_Source, SN::CallPoint);

		SN::SN_Error e = skynet::OK;
		if (!p_ParamList[PU1_First].IsKnownValue())
		{
			if (p_ParamList[PU1_First].IsVariable())
			{
				e = p_ParamList[PU1_First].GetSNI_Expression()->SelfAssert();
			}
			else if (p_ParamList[PU1_First].IsComplete())
			{
				SNI_Variable* v = topFrame->GetVariable(PU1_First);
				v->SetValue(SN::SN_Expression());
				e = p_ParamList[PU1_First].AssertValue(v);
				p_ParamList[PU1_First] = v;
			}
		}

		Breakpoint(SN::DetailStop, SN::ParameterOneId, GetTypeName(), "Unified parameter 1", p_Source, SN::CallPoint);

		if (!e.IsError())
		{
			if (!p_ParamList[PU1_First].IsKnownValue())
			{
				SNI_Thread::GetThread()->GetProcessor()->Delay(SN::SN_FunctionDef(dynamic_cast<SNI_FunctionDef*>(this)), GetNumParameters(), p_ParamList, p_Source);
				return skynet::OK;
			}

			e = p_ParamList[PU1_Result].AssertValue(PrimaryFunctionValue(p_ParamList[PU1_First].GetVariableValue()));
		}
		Breakpoint(SN::WarningStop, SN::RightId, GetTypeName(), "Unify after calculation", p_Source, SN::WarningPoint);
		SNI_Frame::Pop();

		return e;
	}
}
