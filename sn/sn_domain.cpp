#include "SN_Domain.h"

#include "sni_domain.h"

#include "sn_pch.h"

namespace SN
{
	/*static*/ SN_Class SN_Domain::Class()
	{
		return SNI_Domain::Class();
	}

	SN_Domain::SN_Domain(const string &p_Name)
		: SN_Base(new SNI::SNI_Domain(p_Name))
	{
	}

	SN_Domain::~SN_Domain()
	{
	}

	SN_Expression SN_Domain::operator[](const string & p_Index)
	{
		return (*m_Expression)[p_Index];
	}

	SN_Expression SN_Domain::CreateMetaVariable(const SN_Expression & p_Index)
	{
		return SN_Operators::SubscriptCall(*this, p_Index);
	}

	SN_Expression SN_Domain::IsA(const SN_Expression & p_Parent) const
	{
		return SN_Operators::IsA(*this, p_Parent);
	}

	SNI::SNI_Domain * SN_Domain::GetSNI_Domain() const
	{
		return dynamic_cast<SNI::SNI_Domain *>(m_Expression);
	}
}
