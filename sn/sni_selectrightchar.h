#if !defined(SNI_SELECTRIGHTCHAR_H_INCLUDED)
#define SNI_SELECTRIGHTCHAR_H_INCLUDED

#pragma once

#include "sni_unary.h"

namespace SNI
{
	class SNI_SelectRightChar : public SNI_Unary
	{
		PGC_CLASS(SNI_SelectRightChar)
	public:
		SNI_SelectRightChar();
		virtual ~SNI_SelectRightChar();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left) const;
		virtual SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression &p_Left) const;
		virtual SN::SN_Value InverseFunctionValue(const SN::SN_Value &p_Param) const;
		virtual SN::SN_Expression InverseFunctionExpression(const SN::SN_Expression &p_Param) const;

		virtual bool IsKnownValue(const SN::SN_Expression & p_Param, long) const;
		virtual size_t ParamCardinality(const SN::SN_Expression & p_Param, long j) const;

		virtual size_t CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const;
		virtual size_t CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const;
	};
}

#endif // !defined(SNI_SELECTRIGHTCHAR_H_INCLUDED)
