#include "sn_function.h"

#include "sni_function.h"

#include "sn_pch.h"

namespace SN
{
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

	SN_Function::SN_Function(const SN_Expression &p_Function, const SN_Expression &p_Parameter, const SN_Expression &p_Condition)
		: SN_Expression(new SNI::SNI_Function(p_Function.GetSNI_Expression(), p_Parameter.GetSNI_Expression(), p_Condition.GetSNI_Expression(), 0))
	{
		m_Expression->CreateId();
	}

	SN_Function::~SN_Function()
	{
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
