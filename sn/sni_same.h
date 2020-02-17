#if !defined(SNI_SAME_H_INCLUDED)
#define SNI_SAME_H_INCLUDED

#pragma once

#include "sni_unary.h"

namespace SNI
{
	class SNI_Same : public SNI_Unary
	{
		PGC_CLASS(SNI_Same)
	public:
		SNI_Same();
		virtual ~SNI_Same();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;
		virtual string GetOperator() const;
		virtual string DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, size_t p_NumParams, SN::SN_Expression * p_ParamList, const SNI_Expression * p_DebugSource) const;

		SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Param) const;
		SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const;

	protected:
		size_t CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const;
	};
}

#endif // !defined(SNI_SAME_H_INCLUDED)
