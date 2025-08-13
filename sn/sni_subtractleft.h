#if !defined(SNI_SUBTRACTLEFT_H_INCLUDED)
#define SNI_SUBTRACTLEFT_H_INCLUDED

#pragma once

#include "sni_binary.h"

namespace SNI
{
	class SNI_SubtractLeft : public SNI_Binary
	{
		PGC_CLASS(SNI_SubtractLeft)
	public:
		SNI_SubtractLeft();
		virtual ~SNI_SubtractLeft();

		virtual std::string GetTypeName() const;
		virtual std::string DisplayCpp() const;
		virtual std::string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

	protected:
		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Left) const;
		virtual SN::SN_Value LeftInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Expression LeftInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const;
		virtual SN::SN_Value RightInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Expression RightInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Right) const;

		virtual bool IsKnownValue(const SN::SN_Expression & p_Param, long j) const;
		virtual size_t ParamCardinality(const SN::SN_Expression & p_Param, long j) const;

		virtual size_t CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const;
		virtual size_t CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const;
	};
}

#endif // !defined(SNI_SUBTRACTLEFT_H_INCLUDED)
