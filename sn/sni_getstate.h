#if !defined(SNI_GETSTATE_H_INCLUDED)
#define SNI_GETSTATE_H_INCLUDED

#pragma once

#include "sni_attribute.h"

namespace SNI
{
	class SNI_GetState : public SNI_Attribute
	{
		PGC_CLASS(SNI_GetState)
	public:
		SNI_GetState();
		virtual ~SNI_GetState();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;
		virtual string DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, size_t p_NumParams, SN::SN_Expression * p_ParamList, const SNI_Expression * p_DebugSource) const;

		SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Param) const;
		SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const;
	};
}

#endif // !defined(SNI_GETSTATE_H_INCLUDED)
