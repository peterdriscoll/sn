#include "sni_object.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Object::SNI_Object()
	{
	}

	SNI_Object::SNI_Object(const SNI_Object & /* p_Other */)
	{
	}

	SNI_Object::~SNI_Object()
	{

	}

	void SNI_Object::PromoteMembers()
	{
	}

	bool SNI_Object::Equivalent(SNI_Object * /*p_Other*/) const
	{
		return false;
	}
}
