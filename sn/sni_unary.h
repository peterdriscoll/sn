#if !defined(SNI_UNARY_H_INCLUDED)
#define SNI_UNARY_H_INCLUDED

#pragma once

#include "sni_functiondef.h"

namespace SNI
{
	class SNI_Unary : public SNI_FunctionDef
	{
		PGC_CLASS(SNI_Unary)
	public:
		SNI_Unary();
		virtual ~SNI_Unary();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual string DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, SN::SN_ExpressionList * p_ParameterList, const SNI_Expression *p_DebugSource) const;
		virtual long GetPriority() const;
		virtual long GetNumParameters() const;

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left) const = 0;
		virtual SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression &p_Left) const = 0;
		virtual SN::SN_Value InverseFunctionValue(const SN::SN_Value &p_Param) const;
		virtual SN::SN_Expression InverseFunctionExpression(const SN::SN_Expression &p_Param) const;

		virtual SN::SN_Expression CallArray(SN::SN_Expression * p_ParamList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;

		virtual size_t CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const;
		virtual SN::SN_Error UnifyElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet * worldSet) const;

		virtual size_t CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const;
		virtual SN::SN_Value CallElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, SN::SN_ValueSet p_Result) const;

		virtual bool EvaluateNow(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const;

	protected:
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Expression, bool p_Define = false);
		virtual SN::SN_Error PartialUnifyInternal(SN::SN_Expression &p_x, SN::SN_Expression &p_Result);
	};
}

#endif // !defined(SNI_UNARY_H_INCLUDED)
