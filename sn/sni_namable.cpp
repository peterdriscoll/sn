#include "sni_namable.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Namable::SNI_Namable()
	{
	}

	SNI_Namable::~SNI_Namable()
	{
	}

	string SNI_Namable::GetName() const
	{
		return m_Name;
	}

	void SNI_Namable::SetName(const string &p_Name)
	{
		m_Name = p_Name;
	}
}
