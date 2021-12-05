#if !defined(SNI_ASSIGN_H_INCLUDED)
#define SNI_ASSIGN_H_INCLUDED

#pragma once

#include "sni_functiondef.h"

namespace SNI
{
	class SNI_Assign : public SNI_FunctionDef
	{
		PGC_CLASS(SNI_Assign)
	public:
		SNI_Assign();
		virtual ~SNI_Assign();

		virtual long GetNumParameters() const;

		string DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, size_t p_NumParams, SN::SN_Expression *p_ParamList, const SNI_Expression *p_DebugSource) const;

		/// @cond
		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;
		virtual string GetOperator() const;
		/// @endcond

		virtual SN::SN_Expression* LoadParametersUnify(SN::SN_ExpressionList* p_ParameterList) const;

		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;

		virtual SN::SN_Expression PrimaryFunctionExpressionOp(const SN::SN_Expression &p_Left, const SN::SN_Expression& p_Right, const SN::SN_Expression& p_State) const;
		virtual SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right, const SN::SN_Expression& p_State) const;

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right, const SN::SN_Expression& p_State) const;

		virtual SN::SN_Value RightInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Expression RightInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const;

	protected:
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Expression, bool p_Define = false);
		virtual SN::SN_Error PartialUnifyInternal(SN::SN_Expression &p_left, SN::SN_Expression& p_right, SN::SN_Expression& p_state, SN::SN_Expression &p_Result);

		virtual size_t CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParameterList) const;
		virtual SN::SN_Value CallElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, SN::SN_ValueSet p_Result) const;

		size_t CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const;
		virtual SN::SN_Error UnifyElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet * worldSet, const SNI_Expression *p_Source) const;
	};
}

#endif // !defined(SNI_ASSIGN_H_INCLUDED)
