#if !defined(SNI_LAMBDA_H_INCLUDED)
#define SNI_LAMBDA_H_INCLUDED

#pragma once

#include <string>

#include "sni_value.h"

namespace SNI
{
	class SNI_Lambda : public SNI_Value
	{
		PGC_CLASS(SNI_Lambda);
	public:
		SNI_Lambda();
		SNI_Lambda(SNI_Expression *p_FormalParameter, SNI_Expression *p_Expresion, SNI_Expression *p_ConstraintValue);
		virtual ~SNI_Lambda();

		virtual string GetTypeName() const;
		virtual bool IsLambdaValue() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;
		virtual string GetOperator() const;

		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual size_t Cardinality(size_t p_MaxCardinality = CARDINALITY_MAX) const;
		virtual SNI_Expression * LoadFormalParameters(SN::SN_ExpressionList & p_FormalParameterList);
		virtual SNI_Expression * Clone(SNI_Frame *p_Frame, bool &p_Changed);

		virtual SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression Evaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialEvaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Expression Unify(SN::SN_ExpressionList * p_ParameterList);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Expression, bool p_Define = false);

		SNI_Expression * GetFormalParameter();
		SNI_Expression * GetExpression();

	protected:
		virtual void PromoteMembers();

	private:
		SNI_Expression * m_FormalParameter;
		SNI_Expression * m_Expression;
		SNI_Expression * m_ConstraintValue;

	private:
		static long m_Id;
	};

	typedef vector<SNI_Lambda> SNI_LambdaList;
}

#endif // !defined(SNI_LAMBDA_H_INCLUDED)
