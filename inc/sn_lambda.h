#if !defined(SN_LAMBDA_H_INCLUDED)
#define SN_LAMBDA_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_base.h"
#include "../sn/sni_lambda.h"

namespace SNI
{
	class SNI_Lambda;
}

namespace SN
{
	class SN_EXPORT SN_Lambda : public SN_Base<SNI::SNI_Lambda, SN_Expression, SN_Error>
	{
	public:
		SN_Lambda();
		SN_Lambda(SNI::SNI_Lambda *p_Lambda);
		SN_Lambda(const SN_Expression &p_FormalParameter, const SN_Expression &p_Expression);
		virtual ~SN_Lambda();

		// Comparison
		SN_Expression operator ==(const SN_Expression &p_Other) const;

		// Function
		SN_Expression operator()(const SN_Expression &p_Parameter) const;
		SN_Expression operator()(const SN_Expression &p_Parameter, const SN_Expression &p_Condition) const;

		// Valuesets of functions
		SN::SN_Expression operator ||(const SN::SN_Expression &p_Other) const;

		SNI::SNI_Lambda * GetSNI_Lambda() const;
	};
}

#endif // !defined(SN_LAMBDA_H_INCLUDED)
