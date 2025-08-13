#if !defined(SNI_GREATEREQUALS_H_INCLUDED)
#define SNI_GREATEREQUALS_H_INCLUDED

#pragma once

#include "sni_comparison.h"

namespace SNI
{
	class SNI_GreaterEquals : public SNI_Comparison
	{
		PGC_CLASS(SNI_GreaterEquals)
	public:
		SNI_GreaterEquals();
		virtual ~SNI_GreaterEquals();

		virtual std::string GetTypeName() const;
		virtual std::string DisplayCpp() const;
		virtual std::string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;
		virtual std::string GetOperator() const;

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const;
	};
}

#endif // !defined(SNI_GREATEREQUALS_H_INCLUDED)
