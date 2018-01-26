#if !defined(SNI_UNARYOR_H_INCLUDED)
#define SNI_UNARYOR_H_INCLUDED

#pragma once

#include "sni_unary.h"

namespace SNI
{
	class SNI_UnaryOr : public SNI_Unary
	{
		PGC_CLASS(SNI_UnaryOr)
	public:
		SNI_UnaryOr();
		virtual ~SNI_UnaryOr();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_VariableConstPointerList &p_DisplayVariableList) const;
		virtual long GetPriority() const;

		virtual bool AllowDelay() const;

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Param) const;
		virtual SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const;
		virtual SN::SN_Value InverseFunctionValue(const SN::SN_Value & p_Param) const;
		virtual SN::SN_Expression InverseFunctionExpression(const SN::SN_Expression & p_Param) const;

	protected:
		size_t CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const;
	};
}

#endif // !defined(SNI_UNARYOR_H_INCLUDED)
