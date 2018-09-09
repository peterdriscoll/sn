#include "sni_instance.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Instance::SNI_Instance()
	{

	}

	SNI_Instance::~SNI_Instance()
	{

	}

	SNI_Instance::SNI_Instance(const SNI_Instance &p_Other)
	{
	}

	string SNI_Instance::GetTypeName() const
	{
		return "Instance";
	}

	string SNI_Instance::DisplayCpp() const
	{
		return to_string((unsigned long)this);
	}

	string SNI_Instance::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return to_string((unsigned long)this);
	}

	long SNI_Instance::GetPriority() const
	{
		return 100;
	}
}
