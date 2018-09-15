#if !defined(SNI_ISA_H_INCLUDED)
#define SNI_ISA_H_INCLUDED

#pragma once

#include "sni_comparison.h"

namespace SNI
{
	class SNI_IsA : public SNI_Comparison
	{
		PGC_CLASS(SNI_IsA)
	public:
		SNI_IsA();
		virtual ~SNI_IsA();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;


		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const;
		virtual SN::SN_Value LeftInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Expression LeftInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const;
	};
}

#endif // !defined(SNI_ISA_H_INCLUDED)
