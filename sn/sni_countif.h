#if !defined(SNI_COUNTIF_H_INCLUDED)
#define SNI_COUNTIF_H_INCLUDED

#pragma once

#include "sni_binary.h"

namespace SNI
{
	class SNI_CountIf : public SNI_Binary
	{
		PGC_CLASS(SNI_CountIf)
	public:
		SNI_CountIf();
		virtual ~SNI_CountIf();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Value LeftInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Expression LeftInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const;

		virtual size_t CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const;
	};
}

#endif // !defined(SNI_COUNTIF_H_INCLUDED)
