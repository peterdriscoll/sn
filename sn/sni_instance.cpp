#include "sni_instance.h"
#include "sni_class.h"

#include "sn_pch.h"
#include <cstdint>

namespace SNI
{
	SN::SN_Expression SNI_Instance::Type() const
	{
		return m_Class;
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

    SNI_Instance::SNI_Instance(const std::string &p_Name, const std::string &p_DomainName)
    {
		SetName(p_Name);
 		SetDomainName(p_DomainName);
    }

	SNI_Expression * SNI_Instance::Copy() const
	{
		return new SNI_Instance(*this);
	}

	std::string SNI_Instance::GetTypeName() const
	{
		return "Instance";
	}

	std::string SNI_Instance::DisplayCpp() const
	{
		if (GetDomainName().empty())
        {
			return GetName();
        }
		return GetDomainName()+"."+GetName();
	}

	std::string SNI_Instance::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		if (GetDomainName().empty())
        {
			return GetName();
        }
		return GetDomainName()+"."+GetName();
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
		if (!m_Class.IsKnownValue())
		{
			m_Class.SetValue(dynamic_cast<const SNI_Class *>(p_Parent));
		}
		return m_Class.GetSNI_Class()->AssertIsAValue(p_Parent, p_Result);
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
		if (m_Class.IsKnownValue())
		{
			if (m_Class.GetSNI_Class()->DoIsA(p_Parent).GetBool())
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
