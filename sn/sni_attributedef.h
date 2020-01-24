#if !defined(SNI_ATTRIBUTEDEF_H_INCLUDED)
#define SNI_ATTRIBUTEDEF_H_INCLUDED

#pragma once

#include "sni_functiondef.h"

namespace SNI
{
	class SNI_AttributeDef : public SNI_FunctionDef
	{
		PGC_CLASS(SNI_AttributeDef)
	public:
		SNI_AttributeDef();
		virtual ~SNI_AttributeDef();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

		virtual bool IsKnownValue(const SN::SN_Expression & p_Param, long j) const;

		virtual void AssertValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right, SN::SN_Value &p_Result) const = 0;
		virtual SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const = 0;
	};
}

#endif // !defined(SNI_ATTRIBUTEDEF_H_INCLUDED)
