#include "sn_lambda.h"
#include "sn_expression.h"
#include "sn_error.h"

#include "sni_lambda.h"
#include "sni_expression.h"
#include "sni_helpers.h"

#include "sn_pch.h"

namespace SN
{
	SN_Lambda::SN_Lambda()
		: SN_Expression()
	{
	}

	SN_Lambda::SN_Lambda(SNI::SNI_Lambda *p_Lambda)
		: SN_Expression(p_Lambda)
	{
	}

	SN_Lambda::SN_Lambda(const SN_Expression &p_FormalParameter, const SN_Expression &p_Expression)
		: SN_Expression(new SNI::SNI_Lambda(p_FormalParameter.GetSNI_Expression(), p_Expression.GetSNI_Expression(), skynet::Null.GetSNI_Expression()))
	{
	}

	SN_Lambda::SN_Lambda(const SN_Expression &p_FormalParameter, const SN_Expression &p_Expression, const SN_Expression &p_ConstraintValue)
		: SN_Expression(new SNI::SNI_Lambda(p_FormalParameter.GetSNI_Expression(), p_Expression.GetSNI_Expression(), p_ConstraintValue.GetSNI_Expression()))
	{
	}

	SN_Lambda::~SN_Lambda()
	{
	}
}
