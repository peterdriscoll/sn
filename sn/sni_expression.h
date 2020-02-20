#if !defined(SNI_EXPRESSION_H_INCLUDED)
#define SNI_EXPRESSION_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"

#include <string>
#include <functional>
using namespace std;

#include "sni_base.h"

namespace SN
{
	class SN_Parameter;
	typedef vector<SN_Parameter> SN_ParameterList;

	class SN_Expression;
	typedef vector<SN_Expression> SN_ExpressionList;

	class SN_Value;
	typedef vector<SN_Value> SN_ValueList;

	class SN_ValueSet;
	class SN_Cartesian;

	class SN_Error;

	class LogContext;
}

namespace SNI
{
	class SNI_ValueSet;
	class SNI_Value;
	class SNI_World;
	class SNI_WorldSet;
	class SNI_Cartesian;
	class SNI_Cart;
	class SNI_Splitter;
	class SNI_Frame;
	class SNI_Class;
	class SNI_DelayedCall;

	class SNI_Replacement;
	typedef vector<SNI_Replacement> SNI_ReplacementList;

	class SNI_Variable;
	typedef vector<const SNI_Variable *> SNI_VariableConstPointerList;
	typedef vector<SNI_Variable *> SNI_VariablePointerList;

	typedef void OnErrorHandler(SN::SN_Error p_Result);

	class SNI_UnifyContext
	{
	public:
		SN::SN_ExpressionList *m_ParamList;
		SNI_World             *m_World;
	};

	class SNI_Expression : public SNI_Base
	{
		PGC_CLASS(SNI_Expression);

	public:
		SNI_Expression();
		SNI_Expression(unsigned long p_Id);
		SNI_Expression(const SNI_Expression &p_Expression);

		virtual ~SNI_Expression();

		//---------------------------------------------------------------
		// Logging
		//---------------------------------------------------------------
		virtual string GetTypeName() const;
		virtual string GetValueTypeName() const;
		virtual string GetReferredName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual string DisplayValueSN(long, SNI_DisplayOptions & p_DisplayOptions) const;

		//---------------------------------------------------------------
		// Debugging
		//---------------------------------------------------------------
		void CreateId();
		virtual string GetBreakPoint(long p_Index) const;
		virtual string GetBreakPointJS(long p_Index) const;
		virtual unsigned long GetId() const;
		virtual string GetDebugId() const;

		string SetBreakPoint(const string &p_Caption, SNI_DisplayOptions & p_DisplayOptions, const SNI_Expression *p_DebugSource, long p_Index) const;
		string SetStaticBreakPoint(const string & p_Caption, SNI_DisplayOptions & p_DisplayOptions, const SNI_Expression * p_DebugSource, long p_Index) const;

		virtual long GetPriority() const;
		virtual string GetOperator() const;
		virtual string DisplaySN0() const;

		//---------------------------------------------------------------
		// Members
		//---------------------------------------------------------------
		virtual SNI_Expression * LoadParameters(SN::SN_ExpressionList * p_ParameterList) const;
		virtual SNI_Expression * LoadFormalParameters(SN::SN_ExpressionList & p_FormalParameterList);
		virtual SN::SN_Expression * LoadParametersCall(SN::SN_ExpressionList * p_ParameterList) const;
		virtual SN::SN_Expression * LoadParametersUnify(SN::SN_ExpressionList * p_ParameterList) const;
		virtual string DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, size_t p_NumParams, SN::SN_Expression * p_ParamList, const SNI_Expression *p_DebugSource) const;
		virtual SNI_Expression * Clone(SNI_Frame *p_Frame, bool &p_Changed);
		virtual SNI_Expression * Clone(const SNI_Expression *p_Function, SNI_Expression *p_Result);
		virtual bool GetBool() const;
		virtual string GetString() const;
		virtual size_t Count() const;
		virtual size_t Length() const;
		virtual void Simplify();
		virtual void Validate();
		virtual SN::SN_Expression SimplifyValue();
		virtual void AttachDelayedCall(SNI_DelayedCall * p_Call);
		virtual bool IsRequested() const;
		virtual void Request();
		virtual SNI_WorldSet *GetWorldSet();

		virtual bool IsNull() const;
		virtual bool IsKnownValue() const;
		virtual bool IsLeftKnownValue() const;
		virtual bool IsRightKnownValue() const;
		virtual bool IsKnownTypeValue() const;
		virtual bool IsLambdaValue() const;
		virtual bool IsComplete() const;
		virtual void Complete();
		virtual bool IsVariable() const;
		virtual bool IsString() const;
		virtual bool IsStringValue() const;
		virtual bool IsNullValue() const;
		virtual bool IsReferableValue() const;
		virtual SN::SN_Expression GetVariableValue(bool p_IfComplete);
		virtual const SNI_Expression *GetSafeValue() const;
		virtual bool IsError() const;
		virtual bool IsFixed() const;
		virtual bool AllValues() const;
		virtual SN::SN_Error AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World **p_World, SNI_WorldSet *p_WorldSet);
		virtual bool MarkComplete();
		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual size_t Hash() const;
		
		//---------------------------------------------------------------
		// Cardinality
		//---------------------------------------------------------------
		virtual size_t Cardinality(size_t p_MaxCardinality = CARDINALITY_MAX) const;
		virtual size_t LeftCardinality(size_t p_MaxCardinality) const;
		virtual size_t RightCardinality(size_t p_MaxCardinality) const;
		virtual SN::SN_Error ForEach(std::function<SN::SN_Error(const SN::SN_Expression &p_Param, SNI_World *p_World)> p_Action);
		virtual SN::SN_Error ForEachCart(long p_Depth, SNI_Cart * p_Cart);

	public:
		virtual void ForEachSplit(SNI_Splitter * p_Splitter);
		virtual SN::SN_Cartesian CartProd(long p_Index, SNI_FunctionDef *p_FunctionDef = NULL);

		//---------------------------------------------------------------
		// Base
		//---------------------------------------------------------------
		virtual SN::SN_Error DoAssert();
		virtual SN::SN_Error DoPartialAssert();
		virtual SN::SN_Expression DoEvaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Expression DoPartialEvaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Expression DoMeta(long p_MetaLevel = 0);

		virtual SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression Unify(SN::SN_ExpressionList * p_ParameterList);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Expression, bool p_Define = false);
		virtual SN::SN_Error AssertValue(const SN::SN_Expression &p_Value);
		virtual SN::SN_Error AssertIsA(const SN::SN_Expression &p_Value);
		virtual SN::SN_Error SelfAssert();
		virtual SN::SN_Error PartialAssertValue(const SN::SN_Expression &p_Expression, bool p_Define = false);

		virtual void Do();
		virtual void Throw();
		virtual void DoWithHandler(OnErrorHandler * p_ErrorHandler);

		virtual SN::SN_Expression Meta(long p_MetaLevel = 0);

		virtual void Fix(SN::SN_Expression p_Value);
		virtual void Fix();
		//---------------------------------------------------------------
		// Implementation
		//---------------------------------------------------------------

		// Numbers
		virtual SN::SN_Value DoAdd(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoSubtract(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoDivide(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoMultiply(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoNegative() const;
		virtual SN::SN_Value DoSquare() const;
		virtual SN::SN_Value DoSquareRoot() const;

		// Logic
		virtual SN::SN_Value DoAnd(SNI_Expression * p_Other) const;
		virtual SN::SN_Value DoOr(SNI_Expression * p_Other) const;
		virtual SN::SN_Value DoIf(SNI_Expression * p_PositiveCase, SNI_Expression * p_NegativeCase) const;
		virtual SN::SN_Value DoImplies(SNI_Expression * p_PositiveCase) const;
		virtual SN::SN_Value DoNot() const;
		virtual SN::SN_Value DoRevAnd(SNI_Expression * p_PositiveCase) const;
		virtual SN::SN_Value DoRevOr(SNI_Expression * p_PositiveCase) const;
		virtual SN::SN_Value DoCollapse();
		virtual SN::SN_Value DoUnaryAnd() const;
		virtual SN::SN_Value DoUnaryOr() const;

		// Comparison
		virtual SN::SN_Value DoEquals(SNI_Value *p_Other) const;
		virtual SN::SN_Value DoLessThan(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoGreaterThan(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoLessEquals(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoGreaterEquals(SNI_Value * p_Other) const;

		// Strings
		virtual SN::SN_Value DoConcat(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoSubtractLeft(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoSubtractRight(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoSubtractLeftChar() const;
		virtual SN::SN_Value DoSubtractRightChar() const;
		virtual	SN::SN_Value DoSelectLeftChar() const;
		virtual	SN::SN_Value DoSelectRightChar() const;
		virtual	SN::SN_Value DoLookaheadLeft() const;
		virtual	SN::SN_Value DoLookaheadRight() const;
		virtual	SN::SN_Value DoFile() const;

		// Conversions
		virtual SN::SN_Value DoEscape(enum skynet::EscapeType p_EscapeType) const;
		virtual SN::SN_Value DoUnescape(enum skynet::EscapeType p_EscapeType) const;
		virtual	SN::SN_Value DoIntToString() const;
		virtual	SN::SN_Value DoStringToInt() const;
		virtual	SN::SN_Value DoDoubleToString() const;
		virtual	SN::SN_Value DoStringToDouble() const;

		// Inheritance
		virtual SN::SN_Error AssertIsAValue(const SNI_Value * p_Parent, SN::SN_Expression p_Result);
		virtual SN::SN_Value DoIsA(const SNI_Value * p_Parent) const;
		virtual SN::SN_Value DoHasA(SNI_Value * p_Member, SNI_Value * p_Name) const;

		// Sets
		virtual SN::SN_Value DoBuildSet() const;
		virtual SN::SN_Value DoHasMember(SNI_Value * p_Member) const;

		// Value sets
		virtual bool DoIsEmpty() const;
		virtual SN::SN_ValueSet DoRemove(const SN::SN_Value &p_Other);

		virtual SN::SN_Error AssertSubscriptValue(const SNI_Value *p_Index, SN::SN_Expression p_Result);

		// Mapping/vector
		virtual SN::SN_Value DoSubscriptCall(const SN::SN_Value & p_Index) const;
		virtual SN::SN_Value DoReverseSubscript(const SN::SN_Value & p_Result) const;
		virtual SN::SN_Value DoCountIf(SN::SN_Expression p_Value) const;
		virtual SN::SN_Value DoCountAll() const;
		virtual SN::SN_Value DoSum() const;

	protected:
		static SN::SN_Expression AddLambdasPartial(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result);
		static SN::SN_Expression AddLambdas(SN::SN_ExpressionList * p_ParameterList);

		string Bracket(long p_Priority, const string &p_Expression, SNI_DisplayOptions & p_DisplayOptions, const SNI_Expression *p_DebugSource) const;
		string BracketStatic(long p_Priority, const string &p_Expression, SNI_DisplayOptions & p_DisplayOptions, const SNI_Expression *p_DebugSource) const;

	private:
		void HandleAction(SN::SN_Expression p_Result, OnErrorHandler * p_ErrorHandler);
		static map<string, unsigned long> m_IdMap;

	private:
		unsigned long m_Id;

	};
}

#endif // !defined(SNI_EXPRESSION_H_INCLUDED)
