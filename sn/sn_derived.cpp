#include "sn_derived.h"

#include "sni_derived.h"

#include "sn_pch.h"

namespace SN
{
	/*static*/ SN_Class SN_Derived::Class()
	{
		return SNI_Derived::Class();
	}

	SN_Derived::SN_Derived()
		: SN_Base(new SNI::SNI_Derived())
	{
	}

	SN_Derived::~SN_Derived()
	{
	}

	SN_Expression SN_Derived::IsA(const SN_Expression & p_Parent) const
	{
		return SN_Operators::IsA(*this, p_Parent);
	}

	SNI::SNI_Derived * SN_Derived::GetSNI_Derived() const
	{
		return dynamic_cast<SNI::SNI_Derived *>(m_Expression);
	}
}
