#pragma once

#include "exp_ctrl_sn.h"
#include "sn_operators.h"
#include "sni_base.h"

#include <string>

#include "sn_pch.h"

namespace SNI
{
	class SNI_Expression;
	class SNI_Bool;
	class SNI_Error;
}

namespace SN
{
	class SN_Expression;
	class SN_Value;

	SN_Base::SN_Base()
		: m_Expression(NULL)
	{
	}

	SN_Base::SN_Base(SNI::SNI_Base * p_Expression)
		: m_Expression(p_Expression)
	{
	}

	SN_Base::~SN_Base()
	{
	}

	SNI::SNI_Expression* SN_Base::GetSNI_Expression()
	{
		return dynamic_cast<SNI::SNI_Expression*>(m_Expression);
	}

	SNI::SNI_Expression* SN_Base::GetSNI_Expression() const
	{
		return dynamic_cast<SNI::SNI_Expression*>(const_cast<SNI::SNI_Base*>(m_Expression));
	}

	SNI::SNI_Base* SN_Base::GetSNI_Base()
	{
		return m_Expression;
	}

	SNI::SNI_Base* SN_Base::GetSNI_Base() const
	{
		return const_cast<SNI::SNI_Base*>(m_Expression);
	}

	SNI::SNI_Base *& SN_Base::GetSNI_ExpressionRef()
	{
		return m_Expression;
	}
		
	std::string SN_Base::GetTypeName() const
	{
		if (GetSNI_Base())
		{
			return GetSNI_Base()->GetTypeName();
		}
		return "";
	}

	std::string SN_Base::GetValueTypeName() const
	{
		if (GetSNI_Base())
		{
			return GetSNI_Base()->GetValueTypeName();
		}
		return "";
	}

	std::string SN_Base::DisplayCpp() const
	{
		return GetSNI_Base()->DisplayCpp();
	}

	std::string SN_Base::DisplaySN() const
	{
		if (GetSNI_Base())
		{
			SNI::SNI_DisplayOptions l_DisplayOptions(SNI::doTextOnly);
			return GetSNI_Base()->DisplaySN(0, l_DisplayOptions);
		}
		return "";
	}

	std::string SN_Base::DisplaySN(long p_Priority, SNI::SNI_DisplayOptions &p_DisplayOptions) const
	{
		if (GetSNI_Base())
		{
			return GetSNI_Base()->DisplaySN(p_Priority, p_DisplayOptions);
		}
		return "";
	}

	std::string SN_Base::DisplaySN(SNI::SNI_DisplayOptions &p_DisplayOptions) const
	{
		if (GetSNI_Base())
		{
			return GetSNI_Base()->DisplaySN(0, p_DisplayOptions);
		}
		return "";
	}

	std::string SN_Base::DisplayValueSN() const
	{
		if (GetSNI_Base())
		{
			SNI::SNI_DisplayOptions l_DisplayOptions(SNI::doTextOnly);
			return GetSNI_Base()->DisplayValueSN(0, l_DisplayOptions);
		}
		return "";
	}

	std::string SN_Base::DisplayValueSN(SNI::SNI_DisplayOptions &p_DisplayOptions) const
	{
		if (GetSNI_Base())
		{
			return GetSNI_Base()->DisplayValueSN(0, p_DisplayOptions);
		}
		return "";
	}

	std::string SN_Base::LogDisplaySN() const
	{
		if (GetSNI_Base())
		{
			LOGGING(SNI::SNI_DisplayOptions l_DisplayOptions(doTextOnly));
			LOGGING(SN::LogContext context(GetSNI_Base()->DisplaySN(0, l_DisplayOptions)));
		}
		return "";
	}

	bool SN_Base::IsNull() const
	{
		return !GetSNI_Base() || GetSNI_Base()->IsNull();
	}

	bool SN_Base::IsNullValue() const
	{
		return !GetSNI_Base() || GetSNI_Base()->IsNullValue();
	}

	bool SN_Base::IsKnownValue() const
	{
		return GetSNI_Base() && GetSNI_Base()->IsKnownValue();
	}

	bool SN_Base::IsLeftKnownValue() const
	{
		return GetSNI_Base() && GetSNI_Base()->IsLeftKnownValue();
	}

	bool SN_Base::IsRightKnownValue() const
	{
		return GetSNI_Base() && GetSNI_Base()->IsRightKnownValue();
	}

	bool SN_Base::IsKnownTypeValue() const
	{
		return GetSNI_Base() && GetSNI_Base()->IsKnownTypeValue();
	}

	bool SN_Base::IsReferableValue() const
	{
		return m_Expression && m_Expression->IsReferableValue();
	}

	bool SN_Base::IsLambdaValue() const
	{
		return GetSNI_Base() && GetSNI_Base()->IsLambdaValue();
	}

	bool SN_Base::IsInteger() const
	{
		return GetSNI_Base() && GetSNI_Base()->IsInteger();
	}

	bool SN_Base::IsInline() const
	{
		return GetSNI_Base() && GetSNI_Base()->IsInline();
	}

	bool SN_Base::IsVariable() const
	{
		return GetSNI_Base() && GetSNI_Base()->IsVariable();
	}

	bool SN_Base::IsValueHolder() const
	{
		return GetSNI_Base() && GetSNI_Base()->IsValueHolder();
	}

	bool SN_Base::IsValueSet() const
	{
		return GetSNI_Base() && GetSNI_Base()->IsValueSet();
	}

	bool SN_Base::IsString() const
	{
		return GetSNI_Base() && GetSNI_Base()->IsString();
	}

	bool SN_Base::IsStringValue() const
	{
		return GetSNI_Base() && GetSNI_Base()->IsStringValue();
	}

	bool SN_Base::IsComplete() const
	{
		return GetSNI_Base() && GetSNI_Base()->IsComplete();
	}

	bool SN_Base::IsMeta() const
	{
		return GetSNI_Base() && GetSNI_Base()->IsMeta();
	}

	bool SN_Base::IsError() const
	{
		return GetSNI_Base() && GetSNI_Base()->IsError();
	}

	bool SN_Base::IsSignificantError()
	{
		return GetSNI_Base() && GetSNI_Base()->IsSignificantError();
	}

	bool SN_Base::IsFixed() const
	{
		return !GetSNI_Base() || GetSNI_Base()->IsFixed();
	}

	bool SN_Base::AllValuesBoolean() const
	{
		return GetSNI_Base() && GetSNI_Base()->AllValuesBoolean();
	}

	bool SN_Base::Equivalent(const SN_Expression &p_Other) const
	{
		return GetSNI_Base()->Equivalent(dynamic_cast<SNI::SNI_Object *>(p_Other.GetSNI_Expression()));
	}

	SN_Expression SN_Base::In(const SN_Expression &p_Set) const
	{
		return SN_Operators::HasMember(p_Set, GetSNI_Expression());
	}

	SN_Expression SN_Base::BuildMeta(const SN_Expression p_MetaLevel) const
	{
		return SN::SN_Operators::BuildMeta(m_Expression, p_MetaLevel);
	}

	SN_Expression SN_Base::Notes(const SN_Expression p_Before, const SN_Expression p_After) const
	{
		return SN::SN_Operators::Notes(m_Expression, p_Before, p_After);
	}

	size_t SN_Base::Cardinality(size_t p_MaxCardinality) const
	{
		if (!GetSNI_Base())
		{
			return p_MaxCardinality;
		}
		return GetSNI_Base()->Cardinality(p_MaxCardinality);
	}

	size_t SN_Base::LeftCardinality(size_t p_MaxCardinality) const
	{
		if (!GetSNI_Base())
		{
			return p_MaxCardinality;
		}
		return GetSNI_Base()->LeftCardinality(p_MaxCardinality);
	}

	size_t SN_Base::RightCardinality(size_t p_MaxCardinality) const
	{
		if (!GetSNI_Base())
		{
			return p_MaxCardinality;
		}
		return GetSNI_Base()->RightCardinality(p_MaxCardinality);
	}

	SN_Expression SN_Base::GetSafeValue() const
	{
		if (GetSNI_Base())
		{
			return GetSNI_Base()->GetSafeValue();
		}
		return skynet::Null;
	}

	SN_Expression SN_Base::GetValue() const
	{
		if (GetSNI_Base())
		{
			return GetSNI_Base()->GetValue();
		}
		return skynet::Null;
	}

	SN_Expression SN_Base::DoEvaluate(long p_MetaLevel) const
	{
		return GetSNI_Base()->DoEvaluate(p_MetaLevel);
	}

	SN_Expression SN_Base::DoPartialEvaluate(long p_MetaLevel) const
	{
		return GetSNI_Base()->DoPartialEvaluate(p_MetaLevel);
	}

	SN_Error SN_Base::DoAssert()
	{
		return GetSNI_Base()->DoAssert();
	}

	SN_Error SN_Base::AssertValue(const SN_Expression &p_Expression)
	{
		return GetSNI_Base()->AssertValue(p_Expression);
	}

	SN_Error SN_Base::DoPartialAssert()
	{
		return GetSNI_Base()->DoPartialAssert();
	}

	SN_Expression SN_Base::DoQuery(long p_MetaLevel) const
	{
		if (p_MetaLevel)
		{
			SN_Expression l(dynamic_cast<SNI::SNI_Expression*>(GetSNI_Base()));
			return SN_Meta(p_MetaLevel, l).DoEvaluate();
		}
		return GetSNI_Base()->DoEvaluate();
	}

	SN_Expression SN_Base::DoReduce(long p_MetaLevel) const
	{
		if (p_MetaLevel)
		{
			SN_Expression l(dynamic_cast<SNI::SNI_Expression*>(GetSNI_Base()));
			return SN_Meta(p_MetaLevel, l).DoPartialEvaluate();
		}
		return GetSNI_Base()->DoPartialEvaluate();
	}

	SN_Expression SN_Base::DoBuildMeta(long p_MetaLevel)
	{
		return GetSNI_Base()->DoBuildMeta(p_MetaLevel);
	}

	SN_Error SN_Base::PartialAssertValue(const SN_Expression &p_Expression, bool p_Define)
	{
		return GetSNI_Base()->PartialAssertValue(p_Expression, p_Define);
	}

	void SN_Base::Throw()
	{
		return GetSNI_Base()->Throw();
	}

	SN_Error SN_Base::GetError() const
	{
		SNI::SNI_Error *err = dynamic_cast<SNI::SNI_Error *>(GetSNI_Base());
		if (err)
		{
			return SN_Error(err);
		}
		SNI::SNI_Bool *result = dynamic_cast<SNI::SNI_Bool *>(GetSNI_Base());
		if (result)
		{
			return SN_Error(result->GetBool(), false);
		}
		return SN_Error(false, false, "Error object expected.");
	}

	void SN_Base::Do()
	{
		GetSNI_Base()->Do();
	}

	void SN_Base::Fix(const SN_Expression& p_Value)
	{
		GetSNI_Base()->Fix(p_Value);
	}

	void SN_Base::Fix()
	{
		GetSNI_Base()->Fix();
	}
}