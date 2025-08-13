#if !defined(SNI_LOCAL_H_INCLUDED)
#define SNI_LOCAL_H_INCLUDED

#pragma once

#include <string>

#include "sni_expression.h"

//	Skynet
//		local x in E
//		local x, y in E
//		local x, y, ..., z in E
//	C++
//		Local(x, E)
//		Local(x, Local(y, E))
//	Declares the variables x, y ... z local to the expression. This means
//	That the names refer to a separate variable within the scope than without.
//	Currently, this is achieved by cloning the expression. An expression is
//	copied, replacingthe variables with local ones.
//	Later the same effect may be achieved using frames, but the effect will
//	be the same.

namespace SNI
{
	class SNI_Variable;

	class SNI_Local : public SNI_Expression
	{
		PGC_CLASS(SNI_Local);
	public:
		static SNI_Class* Class();
		virtual SNI_Class* VClass();

		SNI_Local();
		SNI_Local(SNI_Variable *p_LocalVariable, SNI_Expression *p_Expression);
		virtual ~SNI_Local();

		virtual std::string GetTypeName() const;
		virtual std::string GetReferredName() const;
		virtual std::string DisplayCpp() const;
		virtual std::string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual void AddVariables(long p_MetaLevel, SNI_VariablePointerMap& p_Map);
		virtual long GetPriority() const;
		virtual std::string GetOperator() const;

		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual SNI_Expression * Clone(long p_MetaLevel, SNI_Frame *p_Frame, bool &p_Changed);

		virtual SN::SN_Expression DoEvaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Expression DoPartialEvaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Error DoAssert();
		virtual SN::SN_Error AssertValue(const SN::SN_Expression &p_Value);
		virtual SN::SN_Error DoPartialAssert();
		virtual SN::SN_Error PartialAssertValue(const SN::SN_Expression &p_Expression, bool p_Define = false);

		virtual SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression Unify(SN::SN_ExpressionList * p_ParameterList);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Expression, bool p_Define = false);

		virtual SN::SN_Error AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World ** p_WorldList, SNI_WorldSet * p_WorldSet);

		SNI_Variable* GetLocalVariable();
		SNI_Expression * GetExpression();

	protected:
		virtual void PromoteMembers();

	private:
		SNI_Variable* m_LocalVariable;
		SNI_Expression * m_Expression;

	private:
		static SNI_Class* m_Class;

		static long m_Id;
	};

	typedef std::vector<SNI_Local> SNI_LocalList;
}

#endif // !defined(SNI_LOCAL_H_INCLUDED)
