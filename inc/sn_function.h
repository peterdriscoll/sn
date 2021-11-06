#if !defined(SN_FUNCTION_H_INCLUDED)
#define SN_FUNCTION_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_expression.h"

namespace SNI
{
	class SNI_Function;
}

namespace SN
{
	class SN_Expression;
	class SN_Value;

	class SN_EXPORT SN_Function : public SN_Expression
	{
	public:
		static SN_Class Class();
		
		SN_Function(const SN_Function &p_Function);
		SN_Function(const SN_Expression  &p_Function);
		SN_Function(const SN_Expression &p_Function, const SN_Expression &p_Parameter);
		virtual ~SN_Function();

		SN_Function &Condition(const SN_Expression &p_Condition);

		SN_Expression GetFunction() const;
		SN_Expression GetParameter() const;

		SNI::SNI_Function * GetSNI_Function() const;
	};
}

#endif // !defined(SN_FUNCTION_H_INCLUDED)
