#if !defined(SNI_LESSTHAN_H_INCLUDED)
#define SNI_LESSTHAN_H_INCLUDED

#pragma once

#include "sni_comparison.h"

namespace SNI
{
	class SNI_LessThan : public SNI_Comparison
	{
		PGC_CLASS(SNI_LessThan)
	public:
		SNI_LessThan();
		virtual ~SNI_LessThan();

		virtual std::string GetTypeName() const;
		virtual std::string DisplayCpp() const;
		virtual std::string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;
		virtual std::string GetOperator() const;

		virtual SN::SN_Value CompareValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Expression CompareExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const;
	};
}

#endif // !defined(SNI_LESSTHAN_H_INCLUDED)
