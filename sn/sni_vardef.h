#if !defined(SNI_VARDEF_H_INCLUDED)
#define SNI_VARDEF_H_INCLUDED

#pragma once

#include <string>

#include "sni_expression.h"

namespace SNI
{
	class SNI_VarDef : public SNI_Expression
	{
		PGC_CLASS(SNI_VarDef);
	public:
		SNI_VarDef();
		virtual ~SNI_VarDef();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_VariablePointerList &p_DisplayVariableList) const;
		virtual long GetPriority() const;

	protected:
		virtual void PromoteMembers();
	};
}

#endif // !defined(SNI_VARDEF_H_INCLUDED)
