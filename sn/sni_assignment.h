#if !defined(SNI_ASSIGNMENT_H_INCLUDED)
#define SNI_ASSIGNMENT_H_INCLUDED

#pragma once

#include <string>

#include "sni_value.h"

namespace SNI
{
	class SNI_Assignment : public SNI_Value
	{
		PGC_CLASS(SNI_Assignment);
	public:
		SNI_Assignment(const SNI_Assignment & p_Other);
		SNI_Assignment(SNI_Expression * p_LeftHandSide, SNI_Expression * p_RightHandSide);
		virtual ~SNI_Assignment();

		virtual string GetTypeName() const;
		virtual string GetReferredName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;
		virtual string GetOperator() const;

		virtual bool IsValue() const;

		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual size_t Cardinality(size_t p_MaxCardinality = CARDINALITY_MAX) const;
		virtual SNI_Expression * Clone(long p_MetaLevel, SNI_Frame *p_Frame, bool &p_Changed);

		virtual SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression DoEvaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Expression DoPartialEvaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Expression Unify(SN::SN_ExpressionList * p_ParameterList);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Expression, bool p_Define = false);

		virtual SNI_Expression *DoImperativeTransform(bool &p_Changed, bool &p_Imperative, SNI_Expression *p_StateInput,  SNI_Expression *&p_StateOutput, long p_MetaLevel);

		SN::SN_Expression DoImperativeTransform2(bool & p_InputChanged, bool & p_OutputChanged, long p_MetaLevel);

		SNI_Expression * LeftHandSide();
		SNI_Expression * RightHandSide();

	protected:
		virtual void PromoteMembers();

	private:
		SNI_Expression * m_LeftHandSide;
		SNI_Expression * m_RightHandSide;
	};
}

#endif // !defined(SNI_ASSIGNMENT_H_INCLUDED)
