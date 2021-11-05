#if !defined(SNI_INCOMPLETEFUNCTION_H_INCLUDED)
#define SNI_INCOMPLETEFUNCTION_H_INCLUDED

#pragma once

#include <string>

#include "sni_value.h"

namespace SNI
{
	class SNI_IncompleteFunction : public SNI_Value
	{
		PGC_CLASS(SNI_IncompleteFunction);
	public:
		static SNI_Class* Class();
		SNI_Class* VClass();

		SNI_IncompleteFunction();
		SNI_IncompleteFunction(SNI_Expression *p_Function);
		SNI_IncompleteFunction(const SN::SN_Expression& p_Expression);
		SNI_IncompleteFunction(const SNI_IncompleteFunction &p_Function);
		virtual ~SNI_IncompleteFunction();

		virtual string GetTypeName() const;
		virtual string GetReferredName() const;
		virtual string DisplayCpp() const;
		virtual long GetPriority() const;

		virtual SNI_Expression *LoadParameters(SN::SN_ExpressionList * p_ParameterList) const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
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
	};

	typedef vector<SNI_IncompleteFunction> SNI_IncompleteFunctionList;
}

#endif // !defined(SNI_INCOMPLETEFUNCTION_H_INCLUDED)
