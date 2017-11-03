#include "sn_object.h"

#include "sni_object.h"

#include "sn_pch.h"

namespace SN
{
	SN_Object::SN_Object()
		: m_Object(NULL)
	{
	}

	SN_Object::SN_Object(const SN_Object &p_Object)
		: m_Object(p_Object.m_Object)
	{
	}

	SN_Object::~SN_Object()
	{
	}

	bool SN_Object::Equivalent(const SN_Object &p_Other) const
	{
		return m_Object->Equivalent(p_Other.m_Object);
	}

}
