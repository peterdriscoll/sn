#if !defined(SNI_IF_H_INCLUDED)
#define SNI_IF_H_INCLUDED

#pragma once

#include "sni_functiondef.h"

namespace SNI
{
	class SNI_If : public SNI_FunctionDef
	{
		PGC_CLASS(SNI_If)
	public:
		SNI_If();
		virtual ~SNI_If();

		/// @cond
		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_VariablePointerList &p_DisplayVariableList) const;
		virtual long GetPriority() const;
		virtual string GetOperator() const;
		/// @endcond

		virtual SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression Unify(SN::SN_ParameterList * p_ParameterList);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result);
		virtual SN::SN_Error PartialUnifyInternal(SN::SN_Expression &p_Condition, SN::SN_Expression &p_PositiveCase, SN::SN_Expression &p_NegativeCase, SN::SN_Expression &p_Result);

	protected:
		/// @cond
		virtual void PromoteMembers();
		/// @endcond
	};
}

#endif // !defined(SNI_IF_H_INCLUDED)
