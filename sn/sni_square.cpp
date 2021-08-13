#include "sni_Square.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sn.h"

#include "sni_value.h"
#include "sni_null.h"
#include "sni_error.h"
#include "sni_helpers.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Square::SNI_Square()
	{
	}

	SNI_Square::~SNI_Square()
	{
	}

	string SNI_Square::GetTypeName() const
	{
		return "Square";
	}

	string SNI_Square::DisplayCpp() const
	{
		return "SN::Square";
	}

	string SNI_Square::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "^-^";
	}

	long SNI_Square::GetPriority() const
	{
		return 0;
	}

	SN::SN_Value SNI_Square::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoSquare();
	}

	SN::SN_Value SNI_Square::InverseFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoSquareRoot();
	}

	SN::SN_Expression SNI_Square::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return skynet::SquareRoot.PrimaryFunctionExpression(p_Param);
	}

	SN::SN_Expression SNI_Square::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context("SNI_Unary::PartialCall ( " + DisplaySnExpressionList(p_ParameterList) + " )"));

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
				if (SN::Is<SNI_SquareRoot *>(op))
				{
					return LOG_RETURN(context, function.GetParameter());
				}
			}
			return LOG_RETURN(context, PrimaryFunctionExpression(value));
		}
		return LOG_RETURN(context, PrimaryFunctionExpressionOp(value));
	}
}
