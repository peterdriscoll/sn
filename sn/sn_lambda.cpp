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
		: SN_Base()
	{
	}

	SN_Lambda::SN_Lambda(SNI::SNI_Lambda *p_Lambda)
		: SN_Base(p_Lambda)
	{
	}

	SN_Lambda::SN_Lambda(const SN_Expression &p_FormalParameter, const SN_Expression &p_Expression)
		: SN_Base(new SNI::SNI_Lambda(p_FormalParameter.GetSNI_Expression(), p_Expression.GetSNI_Expression(), skynet::Null.GetSNI_Expression()))
	{
	}

	SN_Lambda::SN_Lambda(const SN_Expression &p_FormalParameter, const SN_Expression &p_Expression, const SN_Expression &p_ConstraintValue)
		: SN_Base(new SNI::SNI_Lambda(p_FormalParameter.GetSNI_Expression(), p_Expression.GetSNI_Expression(), p_ConstraintValue.GetSNI_Expression()))
	{
	}

	SN_Lambda::~SN_Lambda()
	{
	}

	// Comparison
	SN_Expression SN_Lambda::operator==(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator==(*this, p_Other);
	}

	// Functions
	SN_Expression SN_Lambda::operator()(const SN_Expression &p_Parameter) const
	{
		return SN_Operators::FunctionCall(*this, p_Parameter);
	}

	SN_Expression SN_Lambda::operator()(const SN_Expression &p_Parameter, const SN_Expression &p_Condition) const
	{
		return SN_Operators::FunctionCall(*this, p_Parameter, p_Condition);
	}

	SN::SN_Expression SN_Lambda::operator||(const SN::SN_Expression & p_Other) const
	{
		return SN_Operators::operator ||(*this, p_Other);
	}

	SNI::SNI_Lambda * SN_Lambda::GetSNI_Lambda() const
	{
		return const_cast<SNI::SNI_Lambda*>(m_Expression);
	}
}
