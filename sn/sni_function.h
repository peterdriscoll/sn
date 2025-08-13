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
		static SNI_Class* Class();
		SNI_Class* VClass();

		SNI_Function();
		SNI_Function(SNI_Expression *p_Function);
		SNI_Function(SNI_Expression *p_Function, SNI_Expression *p_Parameter);
		SNI_Function(SNI_Expression *p_Function, SNI_Expression *p_Parameter, SNI_Expression *p_Condition, unsigned long p_Id);
		SNI_Function(const SNI_Function &p_Function);
		virtual ~SNI_Function();

		SN::SN_Function Condition(SNI_Expression * p_Condition);

		virtual std::string GetTypeName() const;
		virtual std::string GetReferredName() const;
		virtual std::string DisplayCpp() const;
		virtual long GetPriority() const;

		virtual SNI_Expression *LoadParameters(SN::SN_ExpressionList * p_ParameterList) const;
		virtual std::string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual void AddVariables(long p_MetaLevel, SNI_VariablePointerMap& p_Map);
		virtual SNI_Expression *Clone(long p_MetaLevel, SNI_Frame *p_Frame, bool &p_Changed);
		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual size_t Hash() const;

		virtual SN::SN_Expression DoEvaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Expression DoPartialEvaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Error DoAssert();
		virtual SN::SN_Error AssertValue(const SN::SN_Expression &p_Value);
		virtual SN::SN_Error AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World ** p_WorldList, SNI_WorldSet * p_WorldSet);
		virtual SN::SN_Error DoPartialAssert();
		virtual SN::SN_Error PartialAssertValue(const SN::SN_Expression &p_Expression, bool p_Define = false);
		virtual SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression Unify(SN::SN_ExpressionList * p_ParameterList);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Expression, bool p_Define = false);

		SNI_Expression * GetFunction();
		SNI_Expression * GetParameter();

	protected:
		virtual void PromoteMembers();

	private:
		static SNI_Class* m_Class;

		SNI_Expression * m_Function;
		SNI_Expression * m_Parameter;
		SNI_Expression * m_Condition;
	};

	typedef std::vector<SNI_Function> SNI_FunctionList;
}

#endif // !defined(SNI_FUNCTION_H_INCLUDED)
