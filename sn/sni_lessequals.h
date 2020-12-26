#if !defined(SNI_LESSEQUALS_H_INCLUDED)
#define SNI_LESSEQUALS_H_INCLUDED

#pragma once

#include "sni_comparison.h"

namespace SNI
{
	class SNI_LessEquals : public SNI_Comparison
	{
		PGC_CLASS(SNI_LessEquals)
	public:
		SNI_LessEquals();
		virtual ~SNI_LessEquals();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;
		virtual string GetOperator() const;

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const;
	};
}

#endif // !defined(SNI_LESSTHAN_H_INCLUDED)
