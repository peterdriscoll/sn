#include "sn_class.h"

#include "sni_class.h"

#include "sn_pch.h"

namespace SN
{
	SN_Class::SN_Class()
		: SN_Base(NULL)
	{
	}

	SN_Class::SN_Class(const SN_Value &p_Value)
		: SN_Base(dynamic_cast<SNI::SNI_Class *>(p_Value.GetSNI_Value()))
	{
	}

	SN_Class::SN_Class(SNI::SNI_Class* p_Class)
		: SN_Base(p_Class)
	{
	}

	SN_Class::SN_Class(const SN_Class &p_Class)
		: SN_Base(p_Class.m_Expression)
	{
	}

	SN_Class::~SN_Class()
	{
	}

	SN_Expression SN_Class::IsA(const SN_Expression &p_Parent) const
	{
		return SN_Operators::IsA(*this, p_Parent);
	}

	SNI::SNI_Class* SN_Class::GetSNI_Class()
	{
		return static_cast<SNI::SNI_Class*>(m_Expression);
	}

	SNI::SNI_Class* SN_Class::GetSNI_Class() const
	{
		return static_cast<SNI::SNI_Class*>(const_cast<SNI::SNI_Base*>(m_Expression));
	}
}
