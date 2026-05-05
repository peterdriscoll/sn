#include "sn_domain.h"

#include "sn_variable.h"
#include "sni_domain.h"

#include "sn_pch.h"

namespace SN
{
	/*static*/ SN_Class SN_Domain::ExprClass()
    {
        return SNI_Domain::ExprClass();
    }

    /*static*/ SN_Class SN_Domain::Class()
	{
		return SNI_Domain::Class();
    }

    SN_Domain SN_Domain::GetCurrent()
    {
        return SNI_Domain::GetCurrent();  
    }

	SN_Domain::SN_Domain(SNI::SNI_Domain* p_Domain)
		: SN_Base(p_Domain)
	{
    }

    SN_Domain::SN_Domain(SNI::SNI_Domain *p_Domain, const std::string &p_Name)
		: SN_Base(new SNI::SNI_Domain(p_Domain, p_Name))
    {
    }

	SN_Domain::SN_Domain(const std::string &p_Name)
		: SN_Base(new SNI::SNI_Domain(p_Name))
	{
	}

	SN_Domain::~SN_Domain()
	{
	}

	SN_Variable SN_Domain::LookupVariable(const std::string & p_Name)
	{
        return GetSNI_Domain()->LookupVariable(p_Name);
    }

	SN_Domain SN_Domain::LookupDomain(const std::string & p_Name)
	{
        return GetSNI_Domain()->LookupDomain(p_Name);
    }

	SN_Domain SN_Domain::DeclareDomain(const std::string & p_Name)
	{
		return GetSNI_Domain()->DeclareDomain(p_Name);
    }

	SN_Domain SN_Domain::LocalDomain(const std::string & p_Name)
	{
		return GetSNI_Domain()->LocalDomain(p_Name);
    }

	SN_Variable SN_Domain::DeclareVariable(const std::string & p_Name)
	{
		return GetSNI_Domain()->DeclareVariable(p_Name);
    }

	SN_Variable SN_Domain::LocalVariable(const std::string & p_Name)
	{
		return GetSNI_Domain()->LocalVariable(p_Name);
    }

	SN_Expression SN_Domain::operator[](const std::string & p_Index)
	{
		return (*GetSNI_Domain())[p_Index];
	}

	SN_Expression SN_Domain::CreateMetaVariable(const SN_Expression & p_Index)
	{
		return SN_Operators::SubscriptCall(*this, p_Index);
	}

	SN_Expression SN_Domain::IsA(const SN_Expression & p_Parent) const
	{
		return SN_Operators::IsA(*this, p_Parent);
	}

	SNI::SNI_Domain* SN_Domain::GetSNI_Domain()
	{
		return dynamic_cast<SNI::SNI_Domain*>(m_Expression);
	}

	SNI::SNI_Domain* SN_Domain::GetSNI_Domain() const
	{
		return dynamic_cast<SNI::SNI_Domain*>(m_Expression);
	}
}
