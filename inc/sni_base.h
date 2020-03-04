#if !defined(SNI_BASE_H_INCLUDED)
#define SNI_BASE_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"

#include <string>
#include <functional>
using namespace std;

#include "../sn/sni_object.h"
#include "../sn/sni_displayoptions.h"

#define CARDINALITY_MAX ULONG_MAX
#define CARDINALITY_SQUARE_ROOT_MAX USHRT_MAX

namespace SN
{
	class SN_ValueSet;
	class SN_Cartesian;

	class SN_Error;

	class LogContext;

	class SN_Parameter;
	typedef vector<SN_Parameter> SN_ParameterList;

	class SN_Expression;
	typedef vector<SN_Expression> SN_ExpressionList;

	class SN_Value;
	typedef vector<SN_Value> SN_ValueList;
}

namespace SNI
{
	class SNI_World;
	class SNI_FunctionDef;

	class SNI_Variable;
	typedef vector<const SNI_Variable *> SNI_VariableConstPointerList;

	class SNI_Base : public SNI_Object
	{
	public:
		SNI_Base() {};
		SNI_Base(const SNI_Base &p_Expression)
			: SNI_Object(p_Expression)
		{};

		virtual ~SNI_Base() {};

		//---------------------------------------------------------------
		// Logging
		//---------------------------------------------------------------
		virtual string GetTypeName() const = 0;
		virtual string GetValueTypeName() const = 0;
		virtual string DisplayCpp() const = 0;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const = 0;
		virtual string DisplayValueSN(long, SNI_DisplayOptions & p_DisplayOptions) const = 0;
		virtual long GetPriority() const = 0;
		virtual string GetOperator() const = 0;
		virtual const SNI_Expression *GetSafeValue() const = 0;

		//---------------------------------------------------------------
		// Cardinality
		//---------------------------------------------------------------
		virtual size_t Cardinality(size_t p_MaxCardinality = CARDINALITY_MAX) const = 0;
		virtual size_t LeftCardinality(size_t p_MaxCardinality = CARDINALITY_MAX) const = 0;
		virtual size_t RightCardinality(size_t p_MaxCardinality = CARDINALITY_MAX) const = 0;
		virtual SN::SN_Error ForEach(std::function<SN::SN_Error(const SN::SN_Expression &p_Param, SNI_World *p_World)> p_Action) = 0;
		virtual SN::SN_Cartesian CartProd(long p_Index, SNI_FunctionDef *p_FunctionDef = NULL) = 0;

		//---------------------------------------------------------------
		// Status
		//---------------------------------------------------------------
		virtual bool IsNull() const = 0;
		virtual bool IsNullValue() const = 0;
		virtual bool IsKnownValue() const = 0;
		virtual bool IsLeftKnownValue() const = 0;
		virtual bool IsRightKnownValue() const = 0;
		virtual bool IsKnownTypeValue() const = 0;
		virtual bool IsReferableValue() const = 0;
		virtual bool IsLambdaValue() const = 0;
		virtual bool IsVariable() const = 0;
		virtual bool IsString() const = 0;
		virtual bool IsStringValue() const = 0;
		virtual bool IsComplete() const = 0;
		virtual bool IsError() const = 0;
		virtual bool IsSignificantError() const = 0;
		virtual bool IsFixed() const = 0;

		//---------------------------------------------------------------
		// Base
		//---------------------------------------------------------------
		virtual SN::SN_Error DoAssert() = 0;
		virtual SN::SN_Error DoPartialAssert() = 0;
		virtual SN::SN_Expression DoEvaluate(long p_MetaLevel = 0) const = 0;
		virtual SN::SN_Expression DoPartialEvaluate(long p_MetaLevel = 0) const = 0;
		virtual SN::SN_Expression DoMeta(long p_MetaLevel = 0) = 0;

		virtual SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const = 0;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const = 0;
		virtual SN::SN_Expression Unify(SN::SN_ExpressionList * p_ParameterList) = 0;
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Expression, bool p_Define) = 0;
		virtual SN::SN_Error AssertValue(const SN::SN_Expression &p_Value) = 0;
		virtual SN::SN_Error PartialAssertValue(const SN::SN_Expression &p_Expression, bool p_Define = false) = 0;

		virtual void Fix(SN::SN_Expression p_Value) = 0;
		virtual void Fix() = 0;
		virtual bool Equivalent(SNI_Object * p_Other) const = 0;

		virtual void Throw() = 0;
		virtual void Do() = 0;
	};
}

#endif // !defined(SNI_BASE_H_INCLUDED)
