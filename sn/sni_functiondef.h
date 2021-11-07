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

	enum ParameterCall3
	{
		PC3_Condition = 0,
		PC3_Positive = 1,
		PC3_Negative = 2
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
	enum ParameterUnify3
	{
		PU3_Result = 0,
		PU3_Condition = 1,
		PU3_Positive = 2,
		PU3_Negative = 3
	};
	class SNI_FunctionDef : public SNI_Value
	{
		PGC_CLASS(SNI_FunctionDef);
	public:
		SNI_FunctionDef();
		virtual ~SNI_FunctionDef();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;
		virtual string GetOperator() const;
		virtual string GetDebugId() const;

		virtual bool AllowDelay() const;
		virtual bool SupportsMultipleOutputs() const;
		virtual bool IgnoreNoConstraint() const;
		virtual long GetNumParameters() const;

		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual size_t Hash() const;

		virtual bool IsKnownValue(const SN::SN_Expression & p_Param, long j) const;

		virtual size_t ParamCardinality(const SN::SN_Expression & p_Param, long j) const;

		virtual bool GetBoolResult() const;

		virtual SN::SN_Expression MakeCallExpression(SN::SN_ExpressionList* p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression MakePartialCallExpression(SN::SN_ExpressionList* p_ParameterList, long p_MetaLevel = 0) const;

		virtual size_t Cardinality(SN::SN_Expression * p_ParameterList) const;

		virtual void ExpandedBooleanResult(SN::SN_Expression* p_ParamList) const;

		virtual string DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, size_t p_NumParams, SN::SN_Expression * p_ParamList, const SNI_Expression *p_DebugSource) const;
		virtual void AddVariablesForCall(long p_MetaLevel, SNI_VariablePointerMap& p_Map, size_t p_NumParams, SN::SN_Expression* p_ParamList) const;

		string GetLogDescription(SN::SN_Expression * p_ParamList) const;

		virtual SN::SN_Expression PrimaryFunctionExpressionOp(const SN::SN_Expression & p_Result) const;
		virtual SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression & p_Result) const;
		virtual SN::SN_Expression PrimaryFunctionExpressionOp(const SN::SN_Expression & p_Left, const SN::SN_Expression & p_Right) const;
		virtual SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression & p_Left, const SN::SN_Expression & p_Right) const;
		virtual SN::SN_Expression PrimaryFunctionExpressionOp(const SN::SN_Expression & p_Cond, const SN::SN_Expression & p_PositiveCase, const SN::SN_Expression & p_NegativeCase) const;
		virtual SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression & p_Cond, const SN::SN_Expression & p_PositiveCase, const SN::SN_Expression & p_NegativeCase) const;

		virtual SN::SN_Expression* LoadParametersCall(SN::SN_ExpressionList * p_ParameterList) const;
		virtual SN::SN_Expression* LoadParametersUnify(SN::SN_ExpressionList* p_ParameterList) const;

		virtual SN::SN_Expression DoEvaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Expression DoPartialEvaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression CallArray(SN::SN_Expression * p_ParamList, long p_MetaLevel, const SNI_Expression* p_Source) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const = 0;

		virtual SN::SN_Error DoAssert();
		virtual SN::SN_Error DoPartialAssert();
		virtual SN::SN_Expression Unify(SN::SN_ExpressionList* p_ParameterList);
		virtual SN::SN_Expression UnifyArray(SN::SN_Expression* p_ParamList, const SNI_Expression *p_Source);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Expression, bool p_Define = false) = 0;

		virtual size_t CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const;
		virtual SN::SN_Error UnifyElement(long p_Depth, SN::SN_Expression * p_InputList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, SNI_WorldSet * worldSet, const SNI_Expression *p_Source) const;
		virtual size_t CardinalityOfCall(long p_Depth, SN::SN_Expression * p_ParameterList) const;
		virtual SN::SN_Value CallElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, SN::SN_ValueSet p_Result) const;

		static size_t MultiplyCardinality(size_t p_Left, size_t p_Right);

	protected:
		virtual SN::SN_Value ForEachCall(size_t p_Card, long p_Depth, SN::SN_Expression * p_InputList) const;
		virtual SN::SN_Error ForEachUnify(size_t p_Card, long p_Depth, SN::SN_Expression * p_ParamList, SN::SN_Expression * p_InputList, bool *p_Output, long p_CalcPos, long p_TotalCalc, const SNI_Expression *p_Source) const;
		virtual bool MergeValueSets(SN::SN_Expression* p_ParamList) const;
	};

	typedef vector<SNI_FunctionDef *> SNI_FunctionDefList;
}

#endif // !defined(SNI_FUNCTIONDEF_H_INCLUDED)
