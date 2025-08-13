#include "sni_namable.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Namable::SNI_Namable()
	{
	}

	SNI_Namable::SNI_Namable(const std::string &p_Name)
	{
		m_Name = p_Name;
	}

	SNI_Namable::~SNI_Namable()
	{
	}

	std::string SNI_Namable::GetName() const
	{
		return m_Name;
	}

	void SNI_Namable::SetName(const std::string &p_Name)
	{
		m_Name = p_Name;
		SNI_Thread::GetThread()->RegisterChange(dynamic_cast<SNI_Variable *>(this));
	}

	std::string SNI_Namable::GetDomainName() const
	{
		return m_DomainName;
	}

	void SNI_Namable::SetDomainName(const std::string &p_DomainName)
	{
		m_DomainName = p_DomainName;
	}
}
