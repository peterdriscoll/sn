#if !defined(SNI_FUNCTIONDEF_H_INCLUDED)
#define SNI_FUNCTIONDEF_H_INCLUDED

#pragma once

#include <string>

#include "sni_value.h"
#include "sni_result.h"
#include "sn_value.h"

namespace SNI
{
	class SNI_World;
	class SNI_WorldSet;

	enum ParameterCall1
	{
		PC1_First = 0,
	};
	enum ParameterCall2
	{
		PC2_First = 0,
		PC2_Second = 1
	};

	enum ParameterUnify1
	{
		PU1_Result = 0,
		PU1_First = 1
	};
	enum ParameterUnify2
	{
		PU2_Result = 0,
		PU2_First = 1,
		PU2_Second = 2
	};
	class SNI_FunctionDef : public SNI_Value
	{
		PGC_CLASS(SNI_FunctionDef);
	public:
		SNI_FunctionDef();
		virtual ~SNI_FunctionDef();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_VariablePointerList &p_DisplayVariableList) const;
		virtual long GetPriority() const;
		virtual string GetOperator() const;

		virtual bool AllowDelay() const;
		virtual long GetNumParameters() const;

		virtual bool Equivalent(SNI_Object * p_Other) const;

		virtual size_t Cardinality(SN::SN_Expression * p_ParameterList) const;

		virtual SN::SN_Expression Evaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialEvaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression CallArray(SN::SN_Expression * p_ParamList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;

		virtual SN::SN_Error Assert();
		virtual SN::SN_Error PartialAssert();
		virtual SN::SN_Expression UnifyArray(SN::SN_Expression* p_ParamList);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result);

		virtual size_t CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const;
		virtual SN::SN_Error UnifyElement(long p_Depth, SN::SN_Expression * p_InputList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet * worldSet) const;
		virtual size_t CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParameterList) const;
		virtual SN::SN_Value CallElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, SN::SN_ValueSet p_Result) const;

		static size_t MultiplyCardinality(size_t p_Left, size_t p_Right);

		virtual SN::SN_Expression * LoadParametersUnify(SN::SN_ExpressionList * p_ParameterList) const;
	protected:
		virtual SN::SN_Value ForEachCall(size_t p_Card, long p_Depth, SN::SN_Expression * p_InputList) const;
		virtual SN::SN_Error ForEachUnify(size_t p_Card, long p_Depth, SN::SN_Expression * p_ParamList, SN::SN_Expression * p_InputList, bool *p_Output, long p_CalcPos, long p_TotalCalc) const;

		virtual SN::SN_Expression * LoadParametersCall(SN::SN_ExpressionList * p_ParameterList) const;
	};

	typedef vector<SNI_FunctionDef *> SNI_FunctionDefList;
}

#endif // !defined(SNI_FUNCTIONDEF_H_INCLUDED)
