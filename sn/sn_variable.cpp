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
		GetSNI_Variable()->AssertValue(p_Value.Evaluate(0));
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

	string SN_Variable::FrameName()
	{
		return GetSNI_Variable()->FrameName();
	}

	SN_Expression SN_Variable::GetValue() const
	{
		return GetSNI_Variable()->GetValue();
	}

	SNI::SNI_Variable* SN_Variable::GetSNI_Variable() const
	{
		return dynamic_cast<SNI::SNI_Variable*>(m_Expression);
	}

	SN_Expression SN_Variable::PartialEvaluate(long p_MetaLevel /* = 0 */) const
	{
		return m_Expression->PartialEvaluate(p_MetaLevel);
	}

	void SN_Variable::Simplify()
	{
		GetSNI_Variable()->Simplify();
	}
}
