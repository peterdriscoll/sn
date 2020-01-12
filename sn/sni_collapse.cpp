#include "sni_collapse.h"

#include "logcontext.h"
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
		SN::LogContext context("SNI_Unary::UnifyInternal ( " + p_value.DisplaySN() + " )");

		SNI_Frame::Push(this, NULL);
		SNI_Frame *topFrame = SNI_Frame::Top();
		SNI_Variable *resultParam = topFrame->CreateParameterByName("result"); // Result.
		SNI_Variable *valueParam = topFrame->CreateParameterByName("param"); // Param 1.

		SN::SN_Value result = p_Result.Evaluate();

		resultParam->SetValue(result);
		valueParam->SetValue(p_value);
		SNI_Thread::GetThread()->DebugCommand(SN::CallPoint, GetTypeName() + ".Unify before cardinality check", SN::LeftId);

		if (result.IsError())
		{
			SNI_Frame::Pop();
			return result;
		}
		if (!result.IsNull())
		{
			resultParam->SetValue(result);
			SN::SN_Value newValue = InverseFunctionValue(result);
			if (!newValue.IsNull())
			{
				valueParam->SetValue(newValue);
				bool ok = p_value.AssertValue(newValue).GetBool();
				if (ok)
				{
					p_Result.DoCollapse();
				}
				resultParam->SetValue(p_Result);
				SNI_Thread::GetThread()->DebugCommand(SN::CallPoint, GetTypeName() + ".Unify", SN::RightId);
				SNI_Frame::Pop();
				return ok;
			}
		}
		SN::SN_Value value = p_value.Evaluate();
		valueParam->SetValue(value);
		SN::SN_Error err(true, true);
		if (!value.IsNull())
		{
			SN::SN_Value newResult = PrimaryFunctionValue(value);
			if (!newResult.IsNull())
			{
				err = p_Result.AssertValue(newResult);
				resultParam->SetValue(p_Result);
			}
		}
		SNI_Thread::GetThread()->DebugCommand(SN::CallPoint, GetTypeName() + ".Unify", SN::RightId);
		SNI_Frame::Pop();
		return err;
	}
}
