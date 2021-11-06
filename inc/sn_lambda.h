#if !defined(SN_LAMBDA_H_INCLUDED)
#define SN_LAMBDA_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_expression.h"
#include "../sn/sni_lambda.h"

namespace SNI
{
	class SNI_Lambda;
}

namespace SN
{
	class SN_EXPORT SN_Lambda : public SN_Expression
	{
	public:
		static SN_Class Class();

		SN_Lambda();
		SN_Lambda(SNI::SNI_Lambda *p_Lambda);
		SN_Lambda(const SN_Expression &p_FormalParameter, const SN_Expression &p_Expression);
		SN_Lambda(const SN_Expression &p_FormalParameter, const SN_Expression &p_Expression, const SN_Expression &p_ConstraintValue);
		virtual ~SN_Lambda();
	};
}

#endif // !defined(SN_LAMBDA_H_INCLUDED)
