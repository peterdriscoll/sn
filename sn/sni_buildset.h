#if !defined(SNI_BUILDSET_H_INCLUDED)
#define SNI_BUILDSET_H_INCLUDED

#pragma once

#include "sni_unary.h"

namespace SNI
{
	class SNI_BuildSet : public SNI_Unary
	{
		PGC_CLASS(SNI_BuildSet)
	public:
		SNI_BuildSet();
		virtual ~SNI_BuildSet();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left) const;
		virtual SN::SN_Value InverseFunctionValue(const SN::SN_Value &p_Param) const;
		virtual SN::SN_Expression InverseFunctionExpression(const SN::SN_Expression &p_Param) const;

		virtual SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel) const;
		virtual SN::SN_Expression UnifyArray(SN::SN_Expression * p_ParamList, const SNI_Expression *p_Source);
	};
}

#endif // !defined(SNI_BUILDSET_H_INCLUDED)
