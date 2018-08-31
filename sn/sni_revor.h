#if !defined(SNI_REVOR_H_INCLUDED)
#define SNI_REVOR_H_INCLUDED

#pragma once

#include "sni_binary.h"

namespace SNI
{
	class SNI_RevOr : public SNI_Binary
	{
		PGC_CLASS(SNI_RevOr)
	public:
		SNI_RevOr();
		virtual ~SNI_RevOr();

		/// @cond
		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;
		/// @endcond

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value & p_Left, const SN::SN_Value & p_Right) const;
		virtual SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression & p_Left, const SN::SN_Expression & p_Right) const;
		virtual SN::SN_Value LeftInverseFunctionValue(const SN::SN_Value & p_Result, const SN::SN_Value & p_Left) const;
		virtual SN::SN_Expression LeftInverseFunctionExpression(const SN::SN_Expression & p_Result, const SN::SN_Expression & p_Right) const;
		virtual SN::SN_Value RightInverseFunctionValue(const SN::SN_Value & p_Result, const SN::SN_Value & p_Right) const;
		virtual SN::SN_Expression RightInverseFunctionExpression(const SN::SN_Expression & p_Result, const SN::SN_Expression & p_Right) const;

		virtual SN::SN_Error UnifyElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet * worldSet) const;
		virtual size_t CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const;

		virtual SN::SN_Value CallElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, SN::SN_ValueSet p_Result) const;
		virtual size_t CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParameterList) const;
	};
}

#endif // !defined(SNI_REVOR_H_INCLUDED)
