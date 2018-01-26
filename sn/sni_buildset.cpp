#include "sni_buildset.h"

#include "sn.h"
#include "logcontext.h"
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

	string SNI_BuildSet::DisplaySN(long /*priority*/, SNI_VariableConstPointerList & /*p_DisplayVariableList*/) const
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

	SN::SN_Expression SNI_BuildSet::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.BuildSet();
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
		SN::LogContext context("SNI_Unary::PartialCall ( " + DisplayPmExpressionList(p_ParameterList) + " )");
		SN::SN_Value value = (*p_ParameterList)[0].Evaluate(p_MetaLevel);
		if (value.IsNull())
		{
			return LOG_RETURN(context, value);
		}

		if (0 < p_MetaLevel)
		{
			return LOG_RETURN(context, PrimaryFunctionExpression(value));
		}

		return LOG_RETURN(context, PrimaryFunctionValue(value));
	}
}
