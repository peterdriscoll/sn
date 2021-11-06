#include "sn_function.h"

#include "sni_function.h"

#include "sn_pch.h"

namespace SN
{
	/*static*/ SN_Class SN_Function::Class()
	{
		return SNI_Value::Class();
	}

	SN_Function::SN_Function(const SN_Function &p_Function)
		: SN_Expression(p_Function.m_Expression)
	{
		m_Expression->CreateId();
	}

	SN_Function::SN_Function(const SN_Expression &p_Function)
		: SN_Expression(dynamic_cast<SNI::SNI_Function *>(p_Function.GetSNI_Expression()))
	{
		m_Expression->CreateId();
	}

	SN_Function::SN_Function(const SN_Expression &p_Function, const SN_Expression &p_Parameter)
		: SN_Expression(new SNI::SNI_Function(p_Function.GetSNI_Expression(), p_Parameter.GetSNI_Expression()))
	{
		m_Expression->CreateId();
	}

	SN_Function::~SN_Function()
	{
	}

	SN_Function & SN_Function::Condition(const SN_Expression & p_Condition)
	{
		GetSNI_Function()->Condition(p_Condition.GetSNI_Expression());
		return *this;
	}

	SNI::SNI_Function * SN_Function::GetSNI_Function() const
	{
		return dynamic_cast<SNI::SNI_Function *>(m_Expression);
	}

	SN_Expression SN_Function::GetFunction() const
	{
		return GetSNI_Function()->GetFunction();
	}

	SN_Expression SN_Function::GetParameter() const
	{
		return GetSNI_Function()->GetParameter();
	}
}
