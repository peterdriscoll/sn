#include "sni_class.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Inherits::SNI_Inherits(const SNI_Expression * p_Parent, SNI_Value *p_Result)
		: m_Parent(p_Parent)
		, m_Result(p_Result)
	{
	}

	SNI_Inherits::~SNI_Inherits()
	{
	}

	const SNI_Expression * SNI_Inherits::GetParent() const
	{
		return m_Parent;
	}

	const SNI_Value * SNI_Inherits::GetResult() const
	{
		return m_Result;
	}

	SN::SN_Error SNI_Inherits::AssertValue(SN::SN_Expression p_Result)
	{
		return m_Result->AssertValue(p_Result);
	}

	void SNI_Inherits::PromoteMembersExternal(PGC::PGC_Base *p_Base)
	{
		p_Base->REQUESTPROMOTION(m_Parent);
		p_Base->REQUESTPROMOTION(m_Result);
	}

	SNI_Class::SNI_Class(const std::string &p_Name, const std::string &p_DomainName)
		: m_Fixed(false)
	{
		SetName(p_Name);
 		SetDomainName(p_DomainName);
	}

	SNI_Class::SNI_Class(SNI_Domain *p_Domain, const std::string &p_Name)
		: m_Domain(p_Domain)
		, m_Fixed(false)
	{
		SetName(p_Name);
	}

	SNI_Class::~SNI_Class()
	{
	}

	SNI_Class::SNI_Class(const SNI_Class &p_Other)
		: m_Fixed(false)
	{
	}

	void SNI_Class::InitClass(SNI_Class *p_Parent)
    {
		m_InheritList.push_back(SNI_Inherits(p_Parent, skynet::True.GetSNI_Bool()));
	}

	SNI_Expression * SNI_Class::Copy() const
	{
		return new SNI_Class(*this);
	}

	std::string SNI_Class::GetTypeName() const
	{
		return "Class";
	}

	std::string SNI_Class::DisplayCpp() const
	{
		if (GetDomainName().empty())
		{
			return GetName();
		}
		return GetDomainName() + "::" + GetName();
	}

	std::string SNI_Class::DisplaySN(long priority, SNI_DisplayOptions & p_DisplayOptions) const
	{
		std::string name;
		if (m_Domain)
        {
			name = m_Domain->DisplaySN(priority, p_DisplayOptions) + ".";
        }
		return name + GetName();
	}

	long SNI_Class::GetPriority() const
	{
		return 100;
	}

	bool SNI_Class::IsClass() const
	{
		return true;
	}
	bool SNI_Class::IsKnownTypeValue() const
	{
		return false;
	}

	bool SNI_Class::Equivalent(SNI_Object* p_Other) const
	{
		SNI_Class* other = dynamic_cast<SNI_Class*>(p_Other);
		if (other)
		{
			return GetDomainName() == other->GetDomainName() && GetName() == other->GetName();
		}
		return false;
	}

	SN::SN_Error SNI_Class::AssertIsAValue(const SNI_Value * p_Parent, SN::SN_Expression p_Result)
	{
		const SNI_Class *instance = dynamic_cast<const SNI_Class *>(p_Parent);
		if (DoIsA(instance).GetBool())
		{
			return p_Result.AssertValue(skynet::True);
		}
		else if (m_Fixed)
		{
			return p_Result.AssertValue(skynet::False);
		}
		else
		{
			for (SNI_Inherits i : m_InheritList)
			{
				if (i.GetParent() == p_Parent)
				{
					return i.AssertValue(p_Result);
				}
			}
			if (p_Result.IsKnownValue())
			{
				m_InheritList.push_back(SNI_Inherits(p_Parent, p_Result.GetSNI_Value()));
			}
			else
			{
				m_InheritList.push_back(SNI_Inherits(p_Parent, skynet::False.GetSNI_Bool()));
				p_Result.AssertValue(skynet::False);
			}
		}
		return skynet::OK;
	}

	// Inheritance
	SN::SN_Value SNI_Class::DoIsA(const SNI_Value * p_Parent) const
	{
		if (dynamic_cast<const SNI_UniversalClass*>(p_Parent))
		{
			return skynet::True;
		}
		const SNI_Class *instance = dynamic_cast<const SNI_Class *>(p_Parent);
		if (this == instance)
		{
			return skynet::True;
		}
		for (SNI_Inherits i : m_InheritList)
		{
			if (i.GetParent()->DoIsA(p_Parent).GetBool())
			{
				if (i.GetResult())
				{
					return skynet::True;
				}
			}
		}
		return skynet::False;
	}

	SN::SN_Value SNI_Class::DoHasA(SNI_Value * p_Member, SNI_Value * p_Name) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " HasA function not implemented.");
	}

	SN::SN_Value SNI_Class::DoHasMember(SNI_Expression *p_Member) const
	{
		return p_Member->Type().GetSNI_Expression()->DoIsA(GetSNI_Value());
	}

	SN::SN_Error SNI_Class::AssertHasMemberValue(SNI_Expression* p_Member, SN::SN_Expression p_Result)
	{
		if (!p_Member)
        {
            return SN::SN_Error(false, false, "Member must not be null.");
        }
        SNI_Class *memberType = p_Member->Type().GetSNI_Class();
		if (!memberType)
        {
            return SN::SN_Error(false, false, "Members must have a class type.");
        }
	 	if (p_Member->SupportsTypeNarrowing())
        {
			SN::SN_Error err = skynet::OK;
			if (p_Result.GetBool())
			{
				if (DoIsA(memberType).GetBool())
				{
					// If this is a sub class of the members type, narrow it. 
					err = p_Member->SetType(this, false);
					if (err.IsError())
					{
						return err;
					}
				}
				else if (memberType->DoIsA(this).GetBool())
				{
					// If the members type is a subclass of this class, no narrowing
				}
                else if (p_Member->HasSyntheticType())
                {
                   memberType->AssertIsAValue(this, skynet::True);
                }
				else
				{
					// if the members type and this class are not in the same hierarchy,
					// create a synthetic class that is a subtype of both narrow the 
					// members type to it.
					SNI_Class *synthetic = new SNI_Class(nullptr, "synthetic_" + std::to_string(GetId()));
					synthetic->AssertIsAValue(p_Member->Type().GetSNI_Value(), skynet::True);
					synthetic->AssertIsAValue(this, skynet::True);
                    err = p_Member->SetType(synthetic, true);
					if (err.IsError())
					{
						return err;
					}
				}
			}
            else
            {
				if (p_Member->HasSyntheticType())
                {
                   memberType->AssertIsAValue(this, skynet::False);
                }
				else
				{
				    SNI_Class *synthetic = new SNI_Class(nullptr, "synthetic_" + std::to_string(GetId()));
					synthetic->AssertIsAValue(p_Member->Type().GetSNI_Value(), skynet::True);
					synthetic->AssertIsAValue(this, skynet::False);
                    err = p_Member->SetType(synthetic, true);
					if (err.IsError())
					{
						return err;
					}
				}
            }
		}
        else
        {
            return p_Result.AssertValue(memberType->DoIsA(this));
		}
		return skynet::OK;
	}

	void SNI_Class::Fix()
	{
		m_Fixed = true;
	}

	void SNI_Class::PromoteMembers()
	{
		for (SNI_Inherits &r : m_InheritList)
		{
			r.PromoteMembersExternal(this);
		}
	}
}
