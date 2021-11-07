#if !defined(SNI_FUNCTIONCALL_H_INCLUDED)
#define SNI_FUNCTIONCALL_H_INCLUDED

#pragma once

#include "sni_value.h"

namespace SNI
{
	class SNI_FunctionCall : public SNI_Value
	{
		PGC_CLASS(SNI_FunctionCall)
	public:
		SNI_FunctionCall();
		virtual ~SNI_FunctionCall();

		static SNI_Class* Class();
		SNI_Class* VClass();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;
		virtual string GetOperator() const;

		virtual bool Equivalent(SNI_Object * p_Other) const;

		virtual SN::SN_Expression DoEvaluate(long p_MetaLevel) const;

		virtual SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;

		virtual SN::SN_Expression Unify(SN::SN_ExpressionList * p_ParameterList);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define = false);

	private:
		static SNI_Class* m_Class;
	};
}

#endif // !defined(SNI_FUNCTIONCALL_H_INCLUDED)
