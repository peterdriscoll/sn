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
		static SNI_Class* Class();
		virtual SNI_Class* VClass();

		SNI_Define(SNI_Variable *p_Variable, enum skynet::DefineType p_DefineType);
		SNI_Define();
		virtual ~SNI_Define();

		virtual std::string GetTypeName() const;
		virtual std::string DisplayCpp() const;
		virtual std::string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual SN::SN_Expression DoEvaluate(long p_MetaLevel = 0) const;

		SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Error PartialAssertValue(const SN::SN_Expression &p_Expression, bool p_Define = false);
		virtual SN::SN_Expression Unify(SN::SN_ExpressionList * p_ParameterList);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Expression, bool p_Define = false);

		SNI_Variable* GetVariable();

	private:
		virtual void PromoteMembers();

		static SNI_Class* m_Class;

		SNI_Variable* m_Variable;
		enum skynet::DefineType m_DefineType;
	};
}

#endif // !defined(SNI_DEFINE_H_INCLUDED)
