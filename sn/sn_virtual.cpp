#include "sn_virtual.h"

#include "sni_virtual.h"

#include "sn_pch.h"

namespace SN
{
	/*static*/ SN_Class SN_Virtual::Class()
	{
		return SNI_Virtual::Class();
	}

	SN_Virtual::SN_Virtual()
		: SN_Base(new SNI::SNI_Virtual())
	{
	}

	SN_Virtual::~SN_Virtual()
	{
	}

	SN_Expression SN_Virtual::IsA(const SN_Expression & p_Parent) const
	{
		return SN_Operators::IsA(*this, p_Parent);
	}

	SNI::SNI_Virtual * SN_Virtual::GetSNI_Virtual() const
	{
		return dynamic_cast<SNI::SNI_Virtual *>(m_Expression);
	}
}
