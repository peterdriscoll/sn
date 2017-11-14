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

	void SNI_Collapse::PromoteMembers()
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

	string SNI_Collapse::DisplaySN(long /*priority*/, SNI_VariablePointerList & /*p_DisplayVariableList*/) const
	{
		return "_";
	}

	long SNI_Collapse::GetPriority() const
	{
		return 4;
	}

	string SNI_Collapse::GetOperator() const
	{
		return "->|";
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

	SN::SN_Expression SNI_Collapse::Unify(SN::SN_ParameterList * p_ParameterList)
	{
		SN::LogContext context("SNI_Collapse::Unify ( " + DisplayPmParameterList(p_ParameterList) + " )");

		SN::SN_Error e = UnifyInternal((*p_ParameterList)[1].GetValue(), (*p_ParameterList)[0].GetValue());
		if (e.GetDelay())
		{
			SNI_DelayedProcessor::GetProcessor()->Delay(SN::SN_FunctionDef(dynamic_cast<SNI_FunctionDef*>(this)), p_ParameterList);
		}
		if (e.IsError())
		{
			e.AddNote(context, this, "Collapse failed");
		}
		return e;
	}

	SN::SN_Error SNI_Collapse::UnifyInternal(SN::SN_Expression &p_value, SN::SN_Expression &p_Result)
	{
		SN::LogContext context("SNI_Unary::UnifyInternal ( " + p_value.DisplaySN() + " )");

		SN::SN_Value result = p_Result.Evaluate();
		if (result.IsError())
		{
			return result;
		}
		if (!result.IsNull())
		{
			SN::SN_Value newValue = InverseFunctionValue(result);
			if (!newValue.IsNull())
			{
				bool ok = p_value.AssertValue(newValue).GetBool();
				if (ok)
				{
					p_Result.DoCollapse();
				}
				return ok;
			}
		}
		SN::SN_Value value = p_value.Evaluate();
		if (!value.IsNull())
		{
			SN::SN_Value newResult = PrimaryFunctionValue(value);
			if (!newResult.IsNull())
			{
				return p_Result.AssertValue(newResult);
			}
		}
		return SN::SN_Error(true, true);
	}
}
