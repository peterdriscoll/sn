#include "sni_not.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Not::SNI_Not()
	{
	}

	SNI_Not::~SNI_Not()
	{
	}

	string SNI_Not::GetTypeName() const
	{
		return "Not";
	}

	string SNI_Not::DisplayCpp() const
	{
		return "SN::Not";
	}

	string SNI_Not::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "!";
	}

	long SNI_Not::GetPriority() const
	{
		return 4;
	}

	string SNI_Not::GetOperator() const
	{
		return "!";
	}

	SN::SN_Value SNI_Not::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.DoNot();
	}

	SN::SN_Expression SNI_Not::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return !p_Param;
	}

	SN::SN_Expression SNI_Not::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		SN::LogContext context("SNI_Unary::PartialCall ( " + DisplayPmExpressionList(p_ParameterList) + " )");

		SN::SN_Expression value = (*p_ParameterList)[0].PartialEvaluate(p_MetaLevel);

		if (0 == p_MetaLevel)
		{
			if (SN::Is<SNI_Value *>(value))
			{
				return LOG_RETURN(context, PrimaryFunctionValue(value));
			}
			if (SN::Is<SNI_Function *>(value))
			{
				SN::SN_Function function = value;
				SN::SN_Expression op = function.GetFunction();
				if (SN::Is<SNI_Not *>(op))
				{
					return LOG_RETURN(context, function.GetParameter());
				}
			}
		}
		return LOG_RETURN(context, PrimaryFunctionExpression(value));
	}
}
