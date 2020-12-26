#include "sni_negative.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Negative::SNI_Negative()
	{
	}

	SNI_Negative::~SNI_Negative()
	{
	}

	string SNI_Negative::GetTypeName() const
	{
		return "Negative";
	}

	string SNI_Negative::DisplayCpp() const
	{
		return "SN::Negative";
	}

	string SNI_Negative::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "!";
	}

	long SNI_Negative::GetPriority() const
	{
		return 4;
	}

	string SNI_Negative::GetOperator() const
	{
		return "-";
	}

	SN::SN_Value SNI_Negative::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoNegative();
	}

	SN::SN_Expression SNI_Negative::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context("SNI_Unary::PartialCall ( " + DisplayPmExpressionList(p_ParameterList) + " )"));

		SN::SN_Expression value = (*p_ParameterList)[0].DoPartialEvaluate(p_MetaLevel);

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
				if (SN::Is<SNI_Negative *>(op))
				{
					return LOG_RETURN(context, function.GetParameter());
				}
			}
			return LOG_RETURN(context, PrimaryFunctionExpression(value));
		}
		return LOG_RETURN(context, PrimaryFunctionExpressionOp(value));
	}
}
