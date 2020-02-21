#include "sn_set.h"

#include "sni_set.h"
#include "sni_error.h"
#include "sni_null.h"

#include "sn_pch.h"

namespace SN
{
	/*static*/ SN_Class SN_Set::Class()
	{
		return SNI_Set::Class();
	}

	SN_Set::SN_Set()
		: SN_Base(new SNI::SNI_Set())
	{
	}

	SN_Set::SN_Set(const SN_Set &p_Value)
		: SN_Base(p_Value.m_Expression)
	{
	}

	SN_Set::SN_Set(const SN_Value &p_Value)
		: SN_Base(new SNI::SNI_Set(p_Value))
	{
	}

	SN_Set::SN_Set(const SN_ValueSet &p_Value)
		: SN_Base(new SNI::SNI_Set(p_Value))
	{
	}

	SN_Set::~SN_Set()
	{
	}

	SN_Expression SN_Set::operator ==(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator ==(*this, p_Other);
	}

	SN_Expression SN_Set::operator !=(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator !=(*this, p_Other);
	}

	SN_Expression SN_Set::operator +(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator +(*this, p_Other);
	}

	SN_Expression SN_Set::operator -(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator -(*this, p_Other);
	}

	SN_Expression SN_Set::HasMember(const SN_Expression &p_Set) const
	{
		return SN_Operators::HasMember(*this, p_Set);
	}

	SN_Expression SN_Set::IsA(const SN_Expression &p_Parent) const
	{
		return SN_Operators::IsA(*this, p_Parent);
	}

	size_t SN_Set::Count()
	{
		return m_Expression->Count();
	}

	SN_Value SN_Set::operator[](long p_index)
	{
		return (*m_Expression)[p_index];
	}

	SNI::SNI_Set * SN_Set::GetSNI_Set() const
	{
		return m_Expression;
	}
}
