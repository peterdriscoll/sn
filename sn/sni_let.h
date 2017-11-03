#if !defined(SNI_LET_H_INCLUDED)
#define SNI_LET_H_INCLUDED

#pragma once

#include <string>
using namespace std;

#include "sni_expression.h"

namespace SNI
{
	class SNI_Let : public SNI_Expression
	{
		PGC_CLASS(SNI_Let);
	public:
		SNI_Let();
		SNI_Let(SNI_Expression *p_Condition, SNI_Expression *p_Expresion);
		virtual ~SNI_Let();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_VariablePointerList &p_DisplayVariableList) const;
		virtual long GetPriority() const;
		virtual string GetOperator() const;

		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual SNI_Expression * Clone(SNI_ReplacementList * p_ReplacementList, bool &p_Changed);

		virtual SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Error Unify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result);

		SNI_Expression * GetCondition();
		SNI_Expression * GetExpression();

	protected:
		virtual void PromoteMembers();

	private:
		SNI_Expression * m_Condition;
		SNI_Expression * m_Expression;

	private:
		static long m_Id;
	};
}

#endif // !defined(SNI_LET_H_INCLUDED)
