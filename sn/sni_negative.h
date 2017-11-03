#if !defined(SNI_NEGATIVE_H_INCLUDED)
#define SNI_NEGATIVE_H_INCLUDED

#pragma once

#include "sni_unary.h"

namespace SNI
{
	class SNI_Negative : public SNI_Unary
	{
		PGC_CLASS(SNI_Negative)
	public:
		SNI_Negative();
		virtual ~SNI_Negative();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_VariablePointerList &p_DisplayVariableList) const;
		virtual long GetPriority() const;
		virtual string GetOperator() const;

		SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Param) const;
		SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const;

	protected:
		virtual void PromoteMembers();
	};
}

#endif // !defined(SNI_NEGATIVE_H_INCLUDED)
