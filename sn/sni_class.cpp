#include "sni_class.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Inherits::SNI_Inherits(const SNI_Value * p_Parent, SNI_Value *p_Result)
		: m_Parent(p_Parent)
		, m_Result(p_Result)
	{
	}

	SNI_Inherits::~SNI_Inherits()
	{
	}

	const SNI_Value * SNI_Inherits::GetParent() const
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

	SNI_Class::SNI_Class(const string &p_ClassName)
	    : m_ClassName(p_ClassName)
		, m_Fixed(false)
	{
	}

	SNI_Class::~SNI_Class()
	{

	}

	SNI_Class::SNI_Class(const SNI_Class &p_Other)
	{
	}

	string SNI_Class::GetTypeName() const
	{
		return "Class";
	}

	string SNI_Class::DisplayCpp() const
	{
		return m_ClassName+"::Class()";
	}

	string SNI_Class::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return GetTypeName() +"(" + m_ClassName + ")";
	}

	long SNI_Class::GetPriority() const
	{
		return 100;
	}

	bool SNI_Class::IsKnownTypeValue() const
	{
		return true;
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
		return SN::SN_Error(GetTypeName() + " HasA function not implemented.");
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
