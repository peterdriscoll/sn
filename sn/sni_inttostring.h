#if !defined(SNI_INTTOSTRING_H_INCLUDED)
#define SNI_INTTOSTRING_H_INCLUDED

#pragma once

#include "sni_unary.h"

namespace SNI
{
	class SNI_IntToString : public SNI_Unary
	{
		PGC_CLASS(SNI_IntToString)
	public:
		SNI_IntToString();
		virtual ~SNI_IntToString();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left) const;
		virtual SN::SN_Value InverseFunctionValue(const SN::SN_Value &p_Param) const;
		virtual SN::SN_Expression InverseFunctionExpression(const SN::SN_Expression &p_Param) const;
	};
}

#endif // !defined(SNI_INTTOSTRING_H_INCLUDED)
