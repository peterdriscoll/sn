#if !defined(SNI_DEBUG_H_INCLUDED)
#define SNI_DEBUG_H_INCLUDED

#pragma once

#include "sni_expression.h"

namespace SNI
{
	class SNI_Debug : public SNI_Expression
	{
		PGC_CLASS(SNI_Debug)
	public:
		SNI_Debug(const SN::SN_Expression &p_other);
		virtual ~SNI_Debug();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long p_Priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual void AddVariables(long p_MetaLevel, SNI_VariablePointerMap& p_Map);
		virtual long GetPriority() const;
		virtual string GetOperator() const;

		virtual SNI_Expression * Clone(long p_MetaLevel, SNI_Frame *p_Frame, bool &p_Changed);
		virtual SN::SN_Error DoAssert();
		virtual SN::SN_Error AssertValue(const SN::SN_Expression &p_Value);
		virtual SN::SN_Error AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World ** p_World, SNI_WorldSet * p_WorldSet);
		virtual SN::SN_Expression DoEvaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Expression DoPartialEvaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression Unify(SN::SN_ExpressionList * p_ParameterList);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Expression, bool p_Define = false);

		virtual SN::SN_Error ForEachCart(long p_Depth, SNI_Cart * p_Cart);
		virtual void ForEachSplit(SNI_Splitter * p_Splitter);

	protected:
		virtual void PromoteMembers();

	private:
		SNI_Expression * m_Expression;
	};
}

#endif // !defined(SNI_DEBUG_H_INCLUDED)
