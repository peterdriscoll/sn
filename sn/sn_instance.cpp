#include "sn_instance.h"

#include "sni_instance.h"

#include "sn_pch.h"

namespace SN
{
	SN_Instance::SN_Instance()
		: SN_Base(NULL)
	{
	}

	SN_Instance::SN_Instance(const SN_Value &p_Value)
		: SN_Base(dynamic_cast<SNI::SNI_Instance *>(p_Value.GetSNI_Value()))
	{
	}

	SN_Instance::SN_Instance(const SN_Instance &p_Instance)
		: SN_Base(p_Instance.m_Expression)
	{
	}

	SN_Instance::~SN_Instance()
	{
	}

	SN_Expression SN_Instance::IsA(const SN_Expression &p_Parent) const
	{
		return SN_Operators::IsA(*this, p_Parent);
	}

	SNI::SNI_Instance *SN_Instance::GetSNI_Instance() const
	{
		return static_cast<SNI::SNI_Instance *>(m_Expression);
	}
}
