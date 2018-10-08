#include "sn_vector.h"

#include "sni_vector.h"

#include "sn_pch.h"

namespace SN
{
	/*static*/ SN_Class SN_Vector::Class()
	{
		return SNI_Vector::Class();
	}

	SN_Vector::SN_Vector()
		: SN_Base(new SNI::SNI_Vector())
	{
	}

	SN_Vector::~SN_Vector()
	{
	}

	SN_Expression SN_Vector::operator[](const SN_Expression & p_Index)
	{
		return SN_Operators::SubscriptCall(*this, p_Index);
	}

	SN_Expression SN_Vector::ReverseSubscript(const SN_Expression & p_Value)
	{
		return SN_Operators::ReverseSubscript(*this, p_Value);
	}

	SN_Expression SN_Vector::CountIf(const SN_Expression & p_Value)
	{
		return SN_Operators::CountIf(*this, p_Value);
	}

	SN_Expression SN_Vector::CountAll()
	{
		return SN_Operators::CountAll(*this);
	}

	SN_Expression SN_Vector::Sum()
	{
		return SN_Operators::Sum(*this);
	}

	SN_Expression SN_Vector::IsA(const SN_Expression & p_Parent) const
	{
		return SN_Operators::IsA(*this, p_Parent);
	}

	SNI::SNI_Vector * SN_Vector::GetSNI_Vector() const
	{
		return dynamic_cast<SNI::SNI_Vector *>(m_Expression);
	}
}
