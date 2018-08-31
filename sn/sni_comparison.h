#if !defined(SNI_COMPARISON_H_INCLUDED)
#define SNI_COMPARISON_H_INCLUDED

#pragma once

#include "sni_binary.h"

namespace SNI
{
	class SNI_Comparison : public SNI_Binary
	{
		PGC_CLASS(SNI_Comparison)
	public:
		SNI_Comparison();
		virtual ~SNI_Comparison();

		virtual string GetTypeName() const = 0;
		virtual string DisplayCpp() const = 0;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const = 0;

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const = 0;
		virtual SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const = 0;
		virtual SN::SN_Value LeftInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Expression LeftInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const;

		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result);
		virtual SN::SN_Error PartialUnifyInternal(SN::SN_Expression &p_left, SN::SN_Expression &p_right, SN::SN_Expression &p_Result);

		virtual SN::SN_Error UnifyElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet * worldSet) const;
		virtual size_t CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const;
	};
}

#endif // !defined(SNI_COMPARISON_H_INCLUDED)
