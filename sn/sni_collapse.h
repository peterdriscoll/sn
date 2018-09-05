#if !defined(SNI_COLLAPSE_H_INCLUDED)
#define SNI_COLLAPSE_H_INCLUDED

#pragma once

#include "sni_unary.h"

namespace SNI
{
	class SNI_Collapse : public SNI_Unary
	{
		PGC_CLASS(SNI_Collapse)
	public:
		SNI_Collapse();
		virtual ~SNI_Collapse();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

		SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Param) const;
		SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const;
		virtual SN::SN_Value InverseFunctionValue(const SN::SN_Value &p_Param) const;
		virtual SN::SN_Expression InverseFunctionExpression(const SN::SN_Expression &p_Param) const;

		virtual SN::SN_Expression UnifyArray(SN::SN_Expression * p_ParamList);
		virtual SN::SN_Error UnifyInternal(SN::SN_Expression & p_value, SN::SN_Expression & p_Result);
	};
}

#endif // !defined(SNI_COLLAPSE_H_INCLUDED)
