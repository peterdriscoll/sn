#if !defined(SNI_OR_H_INCLUDED)
#define SNI_OR_H_INCLUDED

#pragma once

#include "sni_binary.h"

namespace SN
{
	class SN_Error;
}

namespace SNI
{
	class SNI_Or : public SNI_Binary
	{
		PGC_CLASS(SNI_Or)
	public:
		SNI_Or();
		SNI_Or(bool p_Partial);
		virtual ~SNI_Or();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_VariablePointerList &p_DisplayVariableList) const;
		virtual long GetPriority() const;
		virtual string GetOperator() const;

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value & p_Left, const SN::SN_Value & p_Right) const;
		virtual SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression & p_Left, const SN::SN_Expression & p_Right) const;
		virtual SN::SN_Value LeftInverseFunctionValue(const SN::SN_Value & p_Result, const SN::SN_Value & p_Left) const;
		virtual SN::SN_Expression LeftInverseFunctionExpression(const SN::SN_Expression & p_Result, const SN::SN_Expression & p_Right) const;
		virtual SN::SN_Value RightInverseFunctionValue(const SN::SN_Value & p_Result, const SN::SN_Value & p_Right) const;
		virtual SN::SN_Expression RightInverseFunctionExpression(const SN::SN_Expression & p_Result, const SN::SN_Expression & p_Right) const;

		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression Unify(SN::SN_ParameterList * p_ParameterList);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result);
		virtual SN::SN_Error PartialUnifyInternal(SN::SN_Expression &p_left, SN::SN_Expression &p_right, SN::SN_Expression &p_Result);

		virtual size_t CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParamList) const;

		virtual size_t CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const;
		virtual SN::SN_Error UnifyElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet * worldSet) const;
	private:
		bool m_Partial;
	};
}

#endif // !defined(SNI_OR_H_INCLUDED)
