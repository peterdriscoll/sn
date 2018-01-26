#if !defined(SNI_Binary_H_INCLUDED)
#define SNI_Binary_H_INCLUDED

#pragma once

#include "sni_functiondef.h"

namespace SNI
{
	class SNI_Binary : public SNI_FunctionDef
	{
		PGC_CLASS(SNI_Binary)
	public:
		SNI_Binary();
		virtual ~SNI_Binary();

		virtual long GetNumParameters() const;

		/// @cond
		virtual string GetTypeName() const = 0;
		virtual string DisplayCpp() const = 0;
		virtual string DisplaySN(long priority, SNI_VariableConstPointerList &p_DisplayVariableList) const = 0;
		/// @endcond

		virtual SN::SN_Expression CallArray(SN::SN_Expression * p_ParamList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const = 0;
		virtual SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const = 0;
		virtual SN::SN_Value LeftInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const = 0;
		virtual SN::SN_Expression LeftInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const = 0;
		virtual SN::SN_Value RightInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Expression RightInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const;

	protected:
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result);
		virtual SN::SN_Error PartialUnifyInternal(SN::SN_Expression &p_left, SN::SN_Expression &p_right, SN::SN_Expression &p_Result);

		virtual size_t CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParameterList) const;
		virtual SN::SN_Value CallElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, SN::SN_ValueSet p_Result) const;

		size_t CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const;
		virtual SN::SN_Error UnifyElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet * worldSet) const;
	};
}

#endif // !defined(SNI_Binary_H_INCLUDED)
