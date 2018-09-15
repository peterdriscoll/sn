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

	// Inheritance
	SN::SN_Value SNI_Instance::DoIsA(const SNI_Value * p_Parent) const
	{
		const SNI_Instance *instance = dynamic_cast<const SNI_Instance *>(p_Parent);
		return SN::SN_Bool(this == instance);
	}

	SN::SN_Value SNI_Instance::DoHasA(SNI_Value * p_Member, SNI_Value * p_Name) const
	{
		return SN::SN_Error(GetTypeName() + " HasA function not implemented.");
	}
}
