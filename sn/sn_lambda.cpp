#include "sn_lambda.h"
#include "sn_expression.h"
#include "sn_error.h"

#include "sni_lambda.h"
#include "sni_expression.h"
#include "sni_helpers.h"

#include "sn_pch.h"

namespace SN
{
	/*static*/ SN_Class SN_Lambda::Class()
	{
		return SNI_Lambda::Class();
	}

	SN_Lambda::SN_Lambda()
		: SN_Expression()
	{
	}

	SN_Lambda::SN_Lambda(SNI::SNI_Lambda *p_Lambda)
		: SN_Expression(p_Lambda)
	{
	}

	SN_Lambda::SN_Lambda(const SN_Expression &p_FormalParameter, const SN_Expression &p_Expression)
		: SN_Expression(new SNI::SNI_Lambda(p_FormalParameter.GetSNI_Expression(), p_Expression.GetSNI_Expression(), skynet::Null.GetSNI_Expression(), 0))
	{
		GetSNI_Lambda()->CreateId();
	}

	SN_Lambda::SN_Lambda(const SN_Expression &p_FormalParameter, const SN_Expression &p_Expression, const SN_Expression &p_ConstraintValue)
		: SN_Expression(new SNI::SNI_Lambda(p_FormalParameter.GetSNI_Expression(), p_Expression.GetSNI_Expression(), p_ConstraintValue.GetSNI_Expression(), 0))
	{
		GetSNI_Lambda()->CreateId();
	}

	SN_Lambda::~SN_Lambda()
	{
	}

	SNI::SNI_Lambda* SN_Lambda::GetSNI_Lambda()
	{
		return dynamic_cast<SNI::SNI_Lambda*>(m_Expression);
	}

	SNI::SNI_Lambda* SN_Lambda::GetSNI_Lambda() const
	{
		return dynamic_cast<SNI::SNI_Lambda*>(m_Expression);
	}
}
