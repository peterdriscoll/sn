#if !defined(SNI_LOCAL_H_INCLUDED)
#define SNI_LOCAL_H_INCLUDED

#pragma once

#include <string>

#include "sni_expression.h"

namespace SNI
{
	class SNI_Variable;

	class SNI_Local : public SNI_Expression
	{
		PGC_CLASS(SNI_Local);
	public:
		SNI_Local();
		SNI_Local(SNI_Variable *p_LocalVariable, SNI_Expression *p_Expression);
		virtual ~SNI_Local();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_VariablePointerList &p_DisplayVariableList) const;
		virtual long GetPriority() const;
		virtual string GetOperator() const;

		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual SNI_Expression * Clone(SNI_ReplacementList * p_ReplacementList, bool &p_Changed);

		virtual SN::SN_Expression Evaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialEvaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Error Assert();
		virtual SN::SN_Error AssertValue(const SN::SN_Expression &p_Value);
		virtual SN::SN_Error PartialAssert();
		virtual SN::SN_Error PartialAssertValue(const SN::SN_Expression &p_Expression, bool p_Define = false);

		virtual SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression Unify(SN::SN_ExpressionList * p_ParameterList);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result);

		SNI_Variable* GetLocalVariable();
		SNI_Expression * GetExpression();

	protected:
		virtual void PromoteMembers();

	private:
		SNI_Variable* m_LocalVariable;
		SNI_Expression * m_Expression;

	private:
		static long m_Id;
	};

	typedef vector<SNI_Local> SNI_LocalList;
}

#endif // !defined(SNI_LOCAL_H_INCLUDED)
