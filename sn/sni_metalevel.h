#if !defined(SNI_METALEVEL_H_INCLUDED)
#define SNI_METALEVEL_H_INCLUDED

#pragma once

#include "sni_binary.h"

namespace SNI
{
	class SNI_MetaLevel : public SNI_Binary
	{
		PGC_CLASS(SNI_MetaLevel)
	public:
		SNI_MetaLevel();
		virtual ~SNI_MetaLevel();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;
		virtual string DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, size_t p_NumParams, SN::SN_Expression * p_ParamList, const SNI_Expression * p_DebugSource) const;

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Value LeftInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Expression LeftInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const;

		virtual SN::SN_Error PartialUnifyInternal(SN::SN_Expression &p_left, SN::SN_Expression &p_right, SN::SN_Expression &p_Result);
		virtual SN::SN_Expression CallArray(SN::SN_Expression* p_ParamList, long p_MetaLevel, const SNI_Expression* p_Source) const;
		virtual SN::SN_Expression UnifyArray(SN::SN_Expression * p_ParamList, const SNI_Expression *p_Source);
	};
}

#endif // !defined(SNI_METALEVEL_H_INCLUDED)
