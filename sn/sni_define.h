#if !defined(SNI_DEFINE_H_INCLUDED)
#define SNI_DEFINE_H_INCLUDED

#pragma once

#include <string>

#include "sni_vardef.h"

namespace SNI
{
	class SNI_Define : public SNI_VarDef
	{
		PGC_CLASS(SNI_Define);
	public:
		SNI_Define(SNI_Variable *p_Variable);
		SNI_Define();
		virtual ~SNI_Define();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_VariablePointerList &p_DisplayVariableList) const;
		virtual long GetPriority() const;

		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual SN::SN_Expression Evaluate(long p_MetaLevel = 0) const;

		SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Error PartialAssertValue(const SN::SN_Expression &p_Expression, bool p_Define = false);
		virtual SN::SN_Expression Unify(SN::SN_ExpressionList * p_ParameterList);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result);

		SNI_Variable* GetVariable();

	private:
		virtual void PromoteMembers();

		SNI_Variable* m_Variable;
	};
}

#endif // !defined(SNI_DEFINE_H_INCLUDED)
