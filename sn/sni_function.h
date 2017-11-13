#if !defined(SNI_FUNCTION_H_INCLUDED)
#define SNI_FUNCTION_H_INCLUDED

#pragma once

#include <string>

#include "sni_vardef.h"
#include "sni_value.h"

namespace SNI
{
	class SNI_Function : public SNI_VarDef
	{
		PGC_CLASS(SNI_Function);
	public:
		SNI_Function();
		SNI_Function(SNI_Expression *p_Function);
		SNI_Function(SNI_Expression *p_Function, SNI_Expression *p_Parameter);
		SNI_Function(SNI_Expression *p_Function, SNI_Expression *p_Parameter, SNI_Expression *p_Condition);
		virtual ~SNI_Function();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_VariablePointerList &p_DisplayVariableList) const;
		virtual long GetPriority() const;
		virtual string GetOperator() const;

		virtual SNI_Expression * Clone(SNI_ReplacementList * p_ReplacementList, bool &p_Changed);
		virtual bool Equivalent(SNI_Object * p_Other) const;

		virtual SN::SN_Expression Evaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialEvaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Error Assert();
		virtual SN::SN_Error AssertValue(const SN::SN_Expression &p_Value);
		virtual SN::SN_Error AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World ** p_WorldList, SNI_WorldSet * p_WorldSet);
		virtual SN::SN_Error PartialAssert();
		virtual SN::SN_Error PartialAssertValue(const SN::SN_Expression &p_Expression, bool p_Define = false);
		virtual SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression Unify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result);

		SNI_Expression * GetFunction();
		SNI_Expression * GetParameter();

	protected:
		virtual void PromoteMembers();

	private:
		SNI_Expression * m_Function;
		SNI_Expression * m_Parameter;
		SNI_Expression * m_Condition;
	};

	typedef vector<SNI_Function> SNI_FunctionList;
}

#endif // !defined(SNI_FUNCTION_H_INCLUDED)
