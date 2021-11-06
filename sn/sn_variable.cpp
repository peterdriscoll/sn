#include "sn_variable.h"
#include "sn_expression.h"
#include "sn_value.h"
#include "sn_real.h"
#include "sn_error.h"

#include "sni_variable.h"
#include "sni_expression.h"
#include "sni_value.h"
#include "sni_real.h"
#include "sni_helpers.h"

#include "sn_pch.h"

namespace SN
{
	SN_Variable::SN_Variable()
		: SN_Expression(new SNI::SNI_Variable())
	{
	}

	SN_Variable::SN_Variable(const SN_Variable &p_Variable)
		: SN_Expression(p_Variable.GetSNI_Variable())
	{
	}

	SN_Variable::SN_Variable(const SN_Expression &p_Value, bool)
		: SN_Expression(new SNI::SNI_Variable())
	{
		GetSNI_Variable()->AssertValue(p_Value.DoEvaluate(0));
	}

	SN_Variable::SN_Variable(const string &p_Name, const string &p_DomainName)
		: SN_Expression(SNI_Frame::Top()->CreateVariable(p_Name, p_DomainName))
	{
	}

	SN_Variable::SN_Variable(const string &p_Name, const SN_Expression &p_Value, enum skynet::DefineType p_DefineType, bool p_Local)
	{
		if (p_Local)
		{
			m_Expression = new SNI_Variable(p_Name, "", p_DefineType);;
		}
		else
		{
			m_Expression = SNI_Frame::Top()->CreateVariable(p_Name, "", p_DefineType);
		}
		GetSNI_Variable()->AssertValue(p_Value.DoEvaluate());
	}

	SN_Variable::SN_Variable(const SN_Expression &p_Value)
		: SN_Expression(dynamic_cast<SNI::SNI_Variable *>(p_Value.GetSNI_Expression()))
	{
	}

	SN_APPLY_TYPES(Variable, SN_Expression, SN_CONSTRUCTOR_IMPL)


	SN_Variable::SN_Variable(SNI::SNI_Variable* p_Variable)
		: SN_Expression(p_Variable)
	{
	}

	SN_Variable::~SN_Variable()
	{
	}

	bool SN_Variable::GetBool() const
	{
		return GetSNI_Variable()->GetBool();
	}

	string SN_Variable::GetString() const
	{
		return GetSNI_Variable()->GetString();
	}

	size_t SN_Variable::Count() const
	{
		return GetSNI_Variable()->Count();
	}

	size_t SN_Variable::Length() const
	{
		return GetSNI_Variable()->Length();
	}

	string SN_Variable::GetName()
	{
		return GetSNI_Variable()->GetName();
	}

	void SN_Variable::SetName(const string &p_Name)
	{
		GetSNI_Variable()->SetName(p_Name);
	}

	string SN_Variable::GetDomainName()
	{
		return GetSNI_Variable()->GetDomainName();
	}

	void SN_Variable::SetDomainName(const string & p_Name)
	{
		GetSNI_Variable()->SetDomainName(p_Name);
	}

	string SN_Variable::FrameName()
	{
		return GetSNI_Variable()->FrameName();
	}

	SN_Expression SN_Variable::GetValue() const
	{
		return GetSNI_Variable()->GetValue();
	}

	void SN_Variable::SetValue(SN_Expression p_Value, enum skynet::DefineType p_DefineType)
	{
		GetSNI_Variable()->SetValue(p_Value);
		GetSNI_Variable()->SetInline(p_DefineType == skynet::Inline);
	}

	SNI::SNI_Variable* SN_Variable::GetSNI_Variable() const
	{
		return dynamic_cast<SNI::SNI_Variable*>(m_Expression);
	}

	SN_Expression SN_Variable::DoPartialEvaluate(long p_MetaLevel /* = 0 */) const
	{
		return m_Expression->DoPartialEvaluate(p_MetaLevel);
	}

	void SN_Variable::Simplify()
	{
		GetSNI_Variable()->Simplify();
	}
}
