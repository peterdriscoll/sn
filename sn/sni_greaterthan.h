#if !defined(SNI_GREATERTHAN_H_INCLUDED)
#define SNI_GREATERTHAN_H_INCLUDED

#pragma once

#include "sni_comparison.h"

namespace SNI
{
	class SNI_GreaterThan : public SNI_Comparison
	{
		PGC_CLASS(SNI_GreaterThan)
	public:
		SNI_GreaterThan();
		virtual ~SNI_GreaterThan();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;
		virtual string GetOperator() const;

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const;
	};
}

#endif // !defined(SNI_GREATERTHAN_H_INCLUDED)
