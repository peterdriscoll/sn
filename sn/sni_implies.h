#if !defined(SNI_IMPLIES_H_INCLUDED)
#define SNI_IMPLIES_H_INCLUDED

#pragma once

#include "sni_binary.h"

namespace SNI
{
	class SNI_Implies : public SNI_Binary
	{
		PGC_CLASS(SNI_Implies)
	public:
		SNI_Implies();
		virtual ~SNI_Implies();

		/// @cond
		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;
		/// @endcond

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value & p_Left, const SN::SN_Value & p_Right) const;
		virtual SN::SN_Value LeftInverseFunctionValue(const SN::SN_Value & p_Result, const SN::SN_Value & p_Left) const;
		virtual SN::SN_Expression LeftInverseFunctionExpression(const SN::SN_Expression & p_Result, const SN::SN_Expression & p_Right) const;
		virtual SN::SN_Value RightInverseFunctionValue(const SN::SN_Value & p_Result, const SN::SN_Value & p_Right) const;
		virtual SN::SN_Expression RightInverseFunctionExpression(const SN::SN_Expression & p_Result, const SN::SN_Expression & p_Right) const;

		virtual SN::SN_Error UnifyElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet * worldSet, const SNI_Expression *p_Source) const;
		virtual size_t CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const;

		virtual size_t CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParameterList) const;
	};
}

#endif // !defined(SNI_IMPLIES_H_INCLUDED)
