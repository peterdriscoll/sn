#if !defined(SNI_SUBTRACTRIGHT_H_INCLUDED)
#define SNI_SUBTRACTRIGHT_H_INCLUDED

#pragma once

#include "sni_concat.h"
#include "sni_binary.h"

namespace SNI
{
	class SNI_SubtractRight : public SNI_Binary
	{
		PGC_CLASS(SNI_SubtractRight)
	public:
		SNI_SubtractRight();
		virtual ~SNI_SubtractRight();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_VariablePointerList &p_DisplayVariableList) const;
		virtual long GetPriority() const;
		virtual string GetOperator() const;

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const;
		virtual SN::SN_Value LeftInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Left) const;
		virtual SN::SN_Expression LeftInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Left) const;
		virtual SN::SN_Value RightInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Expression RightInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Right) const;

	protected:
		virtual void PromoteMembers();
	};
}

#endif // !defined(SNI_SUBTRACTRIGHT_H_INCLUDED)
