#if !defined(SNI_DOUBLETOSTRING_H_INCLUDED)
#define SNI_DOUBLETOSTRING_H_INCLUDED

#pragma once

#include "sni_unary.h"

namespace SNI
{
	class SNI_DoubleToString : public SNI_Unary
	{
		PGC_CLASS(SNI_DoubleToString)
	public:
		SNI_DoubleToString();
		virtual ~SNI_DoubleToString();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left) const;
		virtual SN::SN_Value InverseFunctionValue(const SN::SN_Value &p_Param) const;
		virtual SN::SN_Expression InverseFunctionExpression(const SN::SN_Expression &p_Param) const;
	};
}

#endif // !defined(SNI_DOUBLETOSTRING_H_INCLUDED)
