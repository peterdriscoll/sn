#if !defined(SNI_CONCAT_H_INCLUDED)
#define SNI_CONCAT_H_INCLUDED

#pragma once

#include "sni_binary.h"

namespace SNI
{
	class SNI_Concat : public SNI_Binary
	{
		PGC_CLASS(SNI_Concat)
	public:
		SNI_Concat();
		virtual ~SNI_Concat();

		/// @cond
		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_VariableConstPointerList &p_DisplayVariableList) const;
		virtual long GetPriority() const;
		/// @endcond

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const;
		virtual SN::SN_Value LeftInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Left) const;
		virtual SN::SN_Expression LeftInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Left) const;
		virtual SN::SN_Value RightInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Expression RightInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Right) const;

	private:
		/// @cond
		virtual void PromoteMembers();
		/// @endcond
	};
}

#endif // !defined(SNI_CONCAT_H_INCLUDED)
