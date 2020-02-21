#include "sn_functiondef.h"

#include "sn_parameter.h"
#include "sn_error.h"

#include "sni_functiondef.h"
#include "sni_binary.h"
#include "sni_unary.h"
#include "sni_object.h"
#include "sni_value.h"
#include "sni_variable.h"

#include "sn_pch.h"

namespace SN
{
	SN_FunctionDef::SN_FunctionDef(const SN_FunctionDef &p_FunctionDef)
		: m_FunctionDef(p_FunctionDef.m_FunctionDef)
	{
	}

	SN_FunctionDef::SN_FunctionDef(const SNI::SNI_FunctionDef *p_FunctionDef)
		: m_FunctionDef(const_cast<SNI::SNI_FunctionDef *>(p_FunctionDef))
	{
	}

	SN_FunctionDef::~SN_FunctionDef()
	{
	}

	// Comparison
	SN_Expression SN_FunctionDef::operator==(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator==(*this, p_Other);
	}

	SN_Expression SN_FunctionDef::operator!=(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator!=(*this, p_Other);
	}

	SN_Expression SN_FunctionDef::operator()(const SN_Expression &p_Parameter) const
	{
		return SN_Operators::FunctionCall(*this, p_Parameter);
	}

	SN::SN_Expression SN_FunctionDef::operator||(const SN::SN_Expression & p_Other) const
	{
		return SN_Operators::operator ||(*this, p_Other);
	}

	SNI::SNI_FunctionDef * SN_FunctionDef::GetSNI_FunctionDef() const
	{
		return m_FunctionDef;
	}

	SNI::SNI_Binary * SN_FunctionDef::GetSNI_Binary() const
	{
		return dynamic_cast<SNI::SNI_Binary *>(m_FunctionDef);
	}

	SNI::SNI_Unary * SN_FunctionDef::GetSNI_Unary() const
	{
		return dynamic_cast<SNI::SNI_Unary *>(m_FunctionDef);
	}
}
