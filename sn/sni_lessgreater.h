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

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_VariableConstPointerList &p_DisplayVariableList) const;
		virtual long GetPriority() const;
		virtual string GetOperator() const;

		virtual SN::SN_Value CompareValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Expression CompareExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const;
	};
}

#endif // !defined(SNI_LESSTHAN_H_INCLUDED)
