#pragma once

#include "exp_ctrl_sn.h"

#include "sni_base.h"

#include <string>

namespace SNI
{
	class SNI_Expression;
	class SNI_Bool;
	class SNI_Error;
	class SNI_Base;
}

namespace SN
{
	class SN_Expression;
	class SN_Value;

	class SN_EXPORT SN_Base
	{
	protected:
		SNI::SNI_Base *m_Expression;

		SNI::SNI_Base* GetSNI_Base();
		SNI::SNI_Base* GetSNI_Base() const;

	public:
		SN_Base();

		SN_Base(SNI::SNI_Base* p_Expression);

		~SN_Base();

		SNI::SNI_Expression* GetSNI_Expression();
		SNI::SNI_Expression* GetSNI_Expression() const;
		SNI::SNI_Base*& GetSNI_ExpressionRef();

		std::string GetTypeName() const;

		std::string GetValueTypeName() const;

		std::string DisplayCpp() const;

		std::string DisplaySN() const;

		std::string DisplaySN(long p_Priority, SNI::SNI_DisplayOptions& p_DisplayOptions) const;

		std::string DisplaySN(SNI::SNI_DisplayOptions& p_DisplayOptions) const;

		std::string DisplayValueSN() const;

		std::string DisplayValueSN(SNI::SNI_DisplayOptions& p_DisplayOptions) const;

		std::string LogDisplaySN() const;

		bool IsNull() const;

		bool IsNullValue() const;

		bool IsKnownValue() const;

		bool IsLeftKnownValue() const;

		bool IsRightKnownValue() const;

		bool IsKnownTypeValue() const;

		bool IsReferableValue() const;

		bool IsLambdaValue() const;

		bool IsInteger() const;

		bool IsInline() const;

		bool IsVariable() const;

		bool IsValueHolder() const;

		bool IsValueSet() const;

		bool IsString() const;

		bool IsStringValue() const;

		bool IsComplete() const;

		bool IsMeta() const;

		bool IsError() const;

		bool IsSignificantError();

		bool IsFixed() const;;

		bool AllValuesBoolean() const;

		bool Equivalent(const SN_Expression& p_Other) const;

		SN_Expression In(const SN_Expression& p_Set) const;

		SN_Expression BuildMeta(const SN_Expression p_MetaLevel) const;

		SN_Expression Notes(const SN_Expression p_Before, const SN_Expression p_After) const;

		size_t Cardinality(size_t p_MaxCardinality = CARDINALITY_MAX) const;

		size_t LeftCardinality(size_t p_MaxCardinality = CARDINALITY_MAX) const;

		size_t RightCardinality(size_t p_MaxCardinality = CARDINALITY_MAX) const;

		SN_Expression GetSafeValue() const;

		SN_Expression GetValue() const;

		SN_Expression DoEvaluate(long p_MetaLevel = 0) const;

		SN_Expression DoPartialEvaluate(long p_MetaLevel = 0) const;

		SN_Error DoAssert();

		SN_Error AssertValue(const SN_Expression& p_Expression);

		SN_Error DoPartialAssert();

		SN_Expression DoQuery(long p_MetaLevel = 0) const;

		SN_Expression DoReduce(long p_MetaLevel = 0) const;

		SN_Expression DoBuildMeta(long p_MetaLevel = 0);

		SN_Error PartialAssertValue(const SN_Expression& p_Expression, bool p_Define = false);

		void Throw();

		SN_Error GetError() const;

		void Do();

		void Fix(const SN_Expression &p_Value);

		void Fix();
	};
}