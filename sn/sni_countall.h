#if !defined(SNI_COUNTAll_H_INCLUDED)
#define SNI_COUNTALL_H_INCLUDED

#pragma once

#include "sni_unary.h"

namespace SNI
{
	class SNI_CountAll : public SNI_Unary
	{
		PGC_CLASS(SNI_CountAll)
	public:
		SNI_CountAll();
		virtual ~SNI_CountAll();

		virtual std::string GetTypeName() const;
		virtual std::string DisplayCpp() const;
		virtual std::string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left) const;
		virtual SN::SN_Value InverseFunctionValue(const SN::SN_Value &p_Param) const;
		virtual SN::SN_Expression InverseFunctionExpression(const SN::SN_Expression &p_Param) const;

		virtual size_t CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const;
	};
}

#endif // !defined(SNI_COUNTALL_H_INCLUDED)
