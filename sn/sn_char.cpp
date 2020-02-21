#include "sn_char.h"

#include "sn_value.h"
#include "sn_string.h"

#include "sni_char.h"

#include "sn_pch.h"

namespace SN
{
	/*static*/ SN_Class SN_Char::Class()
	{
		return SNI_Char::Class();
	}

	SN_Char::SN_Char()
		: m_Char(NULL)
	{
	}

	SN_Char::SN_Char(const SN_Char &p_Char)
		: m_Char(p_Char.m_Char)
	{
	}

	SN_Char::SN_Char(const SN_Value &p_Value)
		: m_Char(dynamic_cast<SNI::SNI_Char *>(p_Value.GetSNI_Value()))
	{
	}

	SN_Char::SN_Char(string::value_type p_Char)
		: m_Char(new SNI::SNI_Char(p_Char))
	{
	}

	SN_Char::~SN_Char()
	{
	}

	SNI::SNI_Char *SN_Char::GetSNI_Char() const
	{
		return m_Char;
	}

	// Comparison
	SN_Expression SN_Char::operator==(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator==(*this, p_Other);
	}

	SN_Expression SN_Char::operator!=(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator!=(*this, p_Other);
	}

	SN_Expression SN_Char::operator <(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator <(*this, p_Other);
	}

	SN_Expression SN_Char::operator >(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator >(*this, p_Other);
	}

	SN_Expression SN_Char::operator<=(const SN_Expression & p_Other) const
	{
		return SN_Operators::operator <=(*this, p_Other);
	}

	SN_Expression SN_Char::operator>=(const SN_Expression & p_Other) const
	{
		return SN_Operators::operator >=(*this, p_Other);
	}

	SN_Expression SN_Char::operator ||(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator ||(*this, p_Other);
	}

	SN_Expression SN_Char::operator +(const SN_Expression  &p_Other) const
	{
		return SN_Operators::operator +(*this, p_Other);

	}

	string::value_type SN_Char::GetChar() const
	{
		return m_Char->GetChar();
	}

	string SN_Char::GetString() const
	{
		return m_Char->GetString();
	}


	SN_Expression SN_Char::IsA(const SN_Expression &p_Parent) const
	{
		return SN_Operators::IsA(*this, p_Parent);
	}
}
