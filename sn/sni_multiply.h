#if !defined(SNI_MULTIPLY_H_INCLUDED)
#define SNI_MULTIPLY_H_INCLUDED

#pragma once

#include "sni_binary.h"

namespace SNI
{
	class SNI_Multiply : public SNI_Binary
	{
		PGC_CLASS(SNI_Multiply)
	public:
		SNI_Multiply();
		virtual ~SNI_Multiply();

		/// @cond
		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_VariableConstPointerList &p_DisplayVariableList) const;
		virtual long GetPriority() const;
		virtual string GetOperator() const;
		/// @endcond

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const;
		virtual SN::SN_Value LeftInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Expression LeftInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const;

	protected:
		virtual void PromoteMembers();
	};
}

#endif // !defined(SNI_MULTIPLY_H_INCLUDED)
