#if !defined(SNI_LET_H_INCLUDED)
#define SNI_LET_H_INCLUDED

#pragma once

#include <string>
using namespace std;

#include "sni_expression.h"

//  Skynet,
//		let C in E
//  C++
//      Let(C, E)
//	C is asserted true independently. If it fails then an error exception is returned
//  through the error handling system (not the result of the expression).
//	The let condition is asserted as a separate statement at a global level,
//  but using local variables.
//	This may not be what you want. A local condition becomes a global failure. 
//	You can catch the error using Catch to localise the effect.
//	Alternatively, only assert that which cannot fail.

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
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual void AddVariables(long p_MetaLevel, SNI_VariablePointerMap& p_Map);
		virtual long GetPriority() const;
		virtual string GetOperator() const;

		virtual bool Equivalent(SNI_Object * p_Other) const;
		SN::SN_Error AssertValue(const SN::SN_Expression & p_Value);
		virtual SNI_Expression * Clone(long p_MetaLevel, SNI_Frame *p_Frame, bool &p_Changed);

		virtual SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression Unify(SN::SN_ExpressionList * p_ParameterList);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Expression, bool p_Define = false);

		virtual SN::SN_Error AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World ** p_WorldList, SNI_WorldSet * p_WorldSet);

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
