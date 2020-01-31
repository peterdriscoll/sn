#include "sni_squareroot.h"

#include "logcontext.h"
#include "sn.h"

#include "sni_value.h"
#include "sni_null.h"
#include "sni_error.h"
#include "sni_helpers.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_SquareRoot::SNI_SquareRoot()
	{

	}

	SNI_SquareRoot::~SNI_SquareRoot()
	{

	}

	string SNI_SquareRoot::GetTypeName() const
	{
		return "SquareRoot";
	}

	string SNI_SquareRoot::DisplayCpp() const
	{
		return "SN::SquareRoot";
	}

	string SNI_SquareRoot::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "SquareRoot";
	}

	long SNI_SquareRoot::GetPriority() const
	{
		return 0;
	}

	SN::SN_Value SNI_SquareRoot::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoSquareRoot();
	}

	SN::SN_Expression SNI_SquareRoot::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.SquareRoot();
	}

	SN::SN_Value SNI_SquareRoot::InverseFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoSquare();
	}

	SN::SN_Expression SNI_SquareRoot::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return p_Param.Square();
	}

	SN::SN_Expression SNI_SquareRoot::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
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
				if (SN::Is<SNI_Square *>(op))
				{
					return LOG_RETURN(context, function.GetParameter());
				}
			}
		}
		return LOG_RETURN(context, PrimaryFunctionExpression(value));
	}
}
