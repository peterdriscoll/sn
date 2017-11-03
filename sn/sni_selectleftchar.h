#if !defined(SNI_SELECTLEFTCHAR_H_INCLUDED)
#define SNI_SELECTLEFTCHAR_H_INCLUDED

#pragma once

#include "sni_unary.h"

namespace SNI
{
	class SNI_SelectLeftChar : public SNI_Unary
	{
		PGC_CLASS(SNI_SelectLeftChar)
	public:
		SNI_SelectLeftChar();
		virtual ~SNI_SelectLeftChar();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_VariablePointerList &p_DisplayVariableList) const;
		virtual long GetPriority() const;
		virtual string GetOperator() const;

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left) const;
		virtual SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression &p_Left) const;
		virtual SN::SN_Value InverseFunctionValue(const SN::SN_Value &p_Param) const;
		virtual SN::SN_Expression InverseFunctionExpression(const SN::SN_Expression &p_Param) const;

	protected:
		virtual void PromoteMembers();
	};
}

#endif // !defined(SNI_SELECTLEFTCHAR_H_INCLUDED)
