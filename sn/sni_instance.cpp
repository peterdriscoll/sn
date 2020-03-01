#include "sni_instance.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ SNI_Class *SNI_Instance::Class()
	{
		if (!m_Class)
		{
			m_Class = new SNI_Class("Instance");
		}
		return m_Class;
	}

	SNI_Class *SNI_Instance::VClass()
	{
		return Class();
	}

	SNI_Instance::SNI_Instance()
	: m_Fixed(false)
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

	bool SNI_Instance::IsKnownTypeValue() const
	{
		return true;
	}

	SN::SN_Error SNI_Instance::AssertIsAValue(const SNI_Value * p_Parent, SN::SN_Expression p_Result)
	{
		const SNI_Instance *instance = dynamic_cast<const SNI_Instance *>(p_Parent);
		if (DoIsA(instance).GetBool())
		{
			return p_Result.AssertValue(skynet::True);
		}
		return m_Class->AssertValue(p_Result);
	}

	// Inheritance
	SN::SN_Value SNI_Instance::DoIsA(const SNI_Value * p_Parent) const
	{
		const SNI_Instance *instance = dynamic_cast<const SNI_Instance *>(p_Parent);
		if (this == instance)
		{
			return skynet::True;
		}
		if (Equivalent(const_cast<SNI_Value *>(p_Parent)))
		{
			return skynet::True;
		}
		if (m_Class)
		{
			if (m_Class->DoIsA(p_Parent).GetBool())
			{
				return skynet::True;
			}
		}
		return skynet::False;
	}

	SN::SN_Value SNI_Instance::DoHasA(SNI_Value * p_Member, SNI_Value * p_Name) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " HasA function not implemented.");
	}

	void SNI_Instance::Fix()
	{
		m_Fixed = true;
	}

	void SNI_Instance::PromoteMembers()
	{
		REQUESTPROMOTION(m_Class);
	}
}
