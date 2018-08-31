#if !defined(SNI_NOT_H_INCLUDED)
#define SNI_NOT_H_INCLUDED

#pragma once

#include "sni_unary.h"

namespace SNI
{
	class SNI_Not : public SNI_Unary
	{
		PGC_CLASS(SNI_Not)
	public:
		SNI_Not();
		virtual ~SNI_Not();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;
		virtual string GetOperator() const;

		SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Param) const;
		SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const;

	protected:
		virtual void PromoteMembers();
	};
}

#endif // !defined(SNI_NOT_H_INCLUDED)
