#include "sn_char.h"

#include "sn_value.h"
#include "sn_string.h"

#include "sni_char.h"

#include "sn_pch.h"

namespace SN
{
	/*static*/ SN_Class SN_Char::ExprClass()
	{
		return SNI_Char::ExprClass();
	}

    /*static*/ SN_Class SN_Char::Class()
	{
		return SNI_Char::Class();
	}

	SN_Char::SN_Char()
		: SN_Base(NULL)
	{
	}

	SN_Char::SN_Char(const SN_Char &p_Char)
		: SN_Base(p_Char.GetSNI_Char())
	{
	}

	SN_Char::SN_Char(const SN_Value &p_Value)
		: SN_Base(dynamic_cast<SNI::SNI_Char *>(p_Value.GetSNI_Value()))
	{
	}

	SN_Char::SN_Char(const SN_Expression& p_Expression)
		: SN_Base(p_Expression.GetSNI_Char())
	{
	}

	SN_Char::SN_Char(std::string::value_type p_Char)
		: SN_Base(new SNI::SNI_Char(p_Char))
	{
	}

	SN_Char::~SN_Char()
	{
	}

	SNI::SNI_Char *SN_Char::GetSNI_Char() const
	{
		return dynamic_cast<SNI::SNI_Char*>(GetSNI_Expression());
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

	std::string::value_type SN_Char::GetChar() const
	{
		return GetSNI_Char()->GetChar();
	}

	std::string SN_Char::GetString() const
	{
		return GetSNI_Char()->GetString();
	}


	SN_Expression SN_Char::IsA(const SN_Expression &p_Parent) const
	{
		return SN_Operators::IsA(*this, p_Parent);
	}
}
