#if !defined(SNI_ISA_H_INCLUDED)
#define SNI_ISA_H_INCLUDED

#pragma once

#include "sni_comparison.h"

namespace SNI
{
	class SNI_IsA : public SNI_Comparison
	{
		PGC_CLASS(SNI_IsA)
	public:
		SNI_IsA();
		virtual ~SNI_IsA();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;
		virtual string GetOperator() const;

		virtual bool IsKnownValue(const SN::SN_Expression & p_Param, long j) const;

		virtual SN::SN_Error AssertValue(const SN::SN_Value & p_Left, const SN::SN_Value & p_Right, SN::SN_Expression & p_Result) const;

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const;

		virtual size_t CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const;
		virtual SN::SN_Error UnifyElement(long p_Depth, SN::SN_Expression * p_InputList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet * worldSet, const SNI_Expression *p_Source) const;

		virtual size_t CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const;
	};
}

#endif // !defined(SNI_ISA_H_INCLUDED)
