#if !defined(SNI_STATEFUL_H_INCLUDED)
#define SNI_STATEFUL_H_INCLUDED

#pragma once

#include "sni_expression.h"

using namespace std;

namespace SNI
{
	class SNI_Stateful : public SNI_Expression
	{
		PGC_CLASS(SNI_Stateful);
	public:
		static SNI_Class *Class();
		virtual SNI_Class *VClass();

		SNI_Stateful(const SNI_Stateful &p_Other);
		SNI_Stateful(enum skynet::Mutability p_Mutability, SNI_Expression *p_Expression);
		virtual ~SNI_Stateful();

		SNI_Expression * GetExpression();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual SNI_Expression *Clone(long p_MetaLevel, SNI_Frame * p_Frame, bool &);
		virtual SN::SN_Expression DoEvaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Expression DoPartialEvaluate(long p_MetaLevel = 0) const;

		virtual SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Error PartialAssertValue(const SN::SN_Expression &p_Expression, bool p_Define = false);
		virtual SN::SN_Expression Unify(SN::SN_ExpressionList * p_ParameterList);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Expression, bool p_Define = false);

		SNI_StateValue * DoImperativeTransform(bool &p_Changed, bool &p_Imperative, SNI_Expression *p_StateInput,  SNI_Expression *&p_StateOutput, long p_MetaLevel);

	private:
		static SNI_Class *m_Class;

		enum skynet::Mutability m_Mutability;
		SNI_Expression *m_Expression;
	};
}

#endif // !defined(SNI_STATEFUL_H_INCLUDED)
