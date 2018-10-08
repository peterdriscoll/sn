#include "sn_mapping.h"

#include "sni_mapping.h"

#include "sn_pch.h"

namespace SN
{
	/*static*/ SN_Class SN_Mapping::Class()
	{
		return SNI_Mapping::Class();
	}

	SN_Mapping::SN_Mapping()
		: SN_Base(new SNI::SNI_Mapping())
	{
	}

	SN_Mapping::~SN_Mapping()
	{
	}

	SN_Expression SN_Mapping::operator[](const SN_Expression & p_Index)
	{
		return SN_Operators::SubscriptCall(*this, p_Index);
	}

	SN_Expression SN_Mapping::ReverseSubscript(const SN_Expression & p_Value)
	{
		return SN_Operators::ReverseSubscript(*this, p_Value);
	}

	SN_Expression SN_Mapping::CountIf(const SN_Expression & p_Value)
	{
		return SN_Operators::CountIf(*this, p_Value);
	}

	SN_Expression SN_Mapping::CountAll()
	{
		return SN_Operators::CountAll(*this);
	}

	SN_Expression SN_Mapping::Sum()
	{
		return SN_Operators::Sum(*this);
	}

	SN_Expression SN_Mapping::IsA(const SN_Expression & p_Parent) const
	{
		return SN_Operators::IsA(*this, p_Parent);
	}

	SNI::SNI_Mapping * SN_Mapping::GetSNI_Mapping() const
	{
		return dynamic_cast<SNI::SNI_Mapping *>(m_Expression);
	}
}
