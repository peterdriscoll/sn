#if !defined(SNI_PARTIALASSERT_H_INCLUDED)
#define SNI_PARTIALASSERT_H_INCLUDED

#pragma once

#include "sni_unary.h"

namespace SNI
{
	class SNI_PartialAssert : public SNI_Unary
	{
		PGC_CLASS(SNI_PartialAssert)
	public:
		SNI_PartialAssert();
		virtual ~SNI_PartialAssert();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;
		virtual string DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, size_t p_NumParams, SN::SN_Expression * p_ParamList, const SNI_Expression * p_DebugSource) const;

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left) const;
		virtual SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression &p_Left) const;

		virtual SN::SN_Error PartialUnifyInternal(SN::SN_Expression & p_value, SN::SN_Expression & p_Result);
		virtual SN::SN_Expression CallArray(SN::SN_Expression * p_ParamList, long p_MetaLevel) const;
		virtual SN::SN_Expression UnifyArray(SN::SN_Expression * p_ParamList, const SNI_Expression *p_Source);
	};
}

#endif // !defined(SNI_PARTIALASSERT_H_INCLUDED)
