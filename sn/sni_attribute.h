#if !defined(SNI_ATTRIBUTE_H_INCLUDED)
#define SNI_ATTRIBUTE_H_INCLUDED

#pragma once

#include "sni_unary.h"

namespace SNI
{
	class SNI_Attribute : public SNI_Unary
	{
		PGC_CLASS(SNI_Attribute)
	public:
		SNI_Attribute();
		virtual ~SNI_Attribute();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;
		virtual string DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, size_t p_NumParams, SN::SN_Expression * p_ParamList, const SNI_Expression * p_DebugSource) const;

		virtual SN::SN_Expression UnifyArray(SN::SN_Expression* p_ParameterList, const SNI_Expression* p_Source);
	};
}

#endif // !defined(SNI_ATTRIBUTE_H_INCLUDED)
