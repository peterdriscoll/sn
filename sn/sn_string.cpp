#include "sn_string.h"
#include "sn.h"

#include "sni_string.h"
#include "sni_stringref.h"
#include "sni_char.h"
#include "sni_function.h"

#include "sn_pch.h"

namespace SN
{
	/*static*/ SN_Class SN_String::Class()
	{
		return SNI_String::Class();
	}

	SN_String::SN_String()
		: SN_Base()
	{
	}

	SN_String::SN_String(const SN_String &p_String)
		: SN_Base(p_String.m_Expression)
	{
	}

	SN_String::SN_String(const SN_Char &p_Char)
		: SN_Base(dynamic_cast<SNI::SNI_String *>(p_Char.GetSNI_Char()))
	{
	}

	SN_String::SN_String(const SN_Expression &p_Expression)
		: SN_Base(dynamic_cast<SNI::SNI_String *>(p_Expression.GetSNI_Expression()))
	{
	}

	SN_String::SN_String(const SN_Value &p_Value)
		: SN_Base(dynamic_cast<SNI::SNI_String *>(p_Value.GetSNI_Value()))
	{
	}

	SN_String::SN_String(const SNI::SNI_Value *p_Value)
		: SN_Base(dynamic_cast<SNI::SNI_String *>(const_cast<SNI::SNI_Value *>(p_Value)))
	{
	}

	SN_String::SN_String(const std::string &p_String)
		: SN_Base(new SNI::SNI_String(p_String))
	{
	}

	SN_String::SN_String(const char *p_String)
		: SN_Base(new SNI::SNI_String(std::string(p_String)))
	{
	}

	SN_String::SN_String(const char p_Char)
		: SN_Base(new SNI::SNI_String(std::string(p_Char, 1)))
	{
	}

	SN_String::~SN_String()
	{
	}

	SNI::SNI_String* SN_String::GetSNI_String()
	{
		return static_cast<SNI::SNI_String*>(m_Expression);
	}

	// Members
	SNI::SNI_String * SN_String::GetSNI_String() const
	{
		return static_cast<SNI::SNI_String*>(m_Expression);
	}

	SNI::SNI_StringRef * SN_String::GetSNI_StringRef() const
	{
		return static_cast<SNI::SNI_StringRef*>(m_Expression);
	}

	std::string SN_String::GetString() const
	{
		return GetSNI_Expression()->GetString();
	}

	size_t SN_String::Length() const
	{
		return GetString().length();
	}

	// Construction
	SN_Expression SN_String::operator ||(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator ||(*this, p_Other);
	}

	SN_Expression SN_String::operator ==(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator ==(*this, p_Other);
	}

	SN_Expression SN_String::operator !=(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator !=(*this, p_Other);
	}

	SN_Expression SN_String::operator <(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator <(*this, p_Other);
	}

	SN_Expression SN_String::operator >(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator >(*this, p_Other);
	}

	SN_Expression SN_String::operator <=(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator <=(*this, p_Other);
	}

	SN_Expression SN_String::operator >=(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator >=(*this, p_Other);
	}

	SN_Expression SN_String::operator +(const SN_Expression  &p_Other) const
	{
		return SN_Operators::operator +(*this, p_Other);
	}

	SN_Expression SN_String::operator-(const SN_Expression & p_Other) const
	{
		return SN_Operators::SubtractRight(*this, p_Other);
	}

	SN_Expression SN_String::Concat(const SN_Expression &p_Other) const
	{
		return SN_Operators::Concat(*this, p_Other);
	}

	SN_Expression SN_String::SubtractLeft(const SN_Expression &p_Other) const
	{
		return SN_Operators::SubtractLeft(*this, p_Other);
	}

	SN_Expression SN_String::SubtractRight(const SN_Expression &p_Other) const
	{
		return SN_Operators::SubtractRight(*this, p_Other);
	}

	SN_Expression SN_String::SubtractLeftChar() const
	{
		return SN_Operators::SubtractLeftChar(*this);
	}

	SN_Expression SN_String::SubtractRightChar() const
	{
		return SN_Operators::SubtractRightChar(*this);
	}

	SN_Expression SN_String::SelectLeftChar() const
	{
		return SN_Operators::SelectLeftChar(m_Expression);
	}

	SN_Expression SN_String::SelectRightChar() const
	{
		return SN_Operators::SelectRightChar(m_Expression);
	}

	SN_Expression SN_String::LookaheadLeft() const
	{
		return SN_Operators::LookaheadLeft(m_Expression);
	}

	SN_Expression SN_String::LookaheadRight() const
	{
		return SN_Operators::LookaheadRight(m_Expression);
	}

	SN_Expression SN_String::LookStringLeft(const SN_Expression &p_Other) const
	{
		return SN_Operators::LookStringLeft(m_Expression, p_Other);
	}

	SN_Expression SN_String::LookStringRight(const SN_Expression &p_Other) const
	{
		return SN_Operators::LookStringRight(m_Expression, p_Other);
	}

	SN_Expression SN_String::File() const
	{
		return SN_Operators::File(m_Expression);
	}

	// Conversion
	SN_Expression SN_String::Escape(enum SN::EscapeType p_EscapeType) const
	{
		return SN_Operators::Escape(p_EscapeType, *this);
	}

	SN_Expression SN_String::Unescape(enum SN::EscapeType p_EscapeType) const
	{
		return SN_Operators::Unescape(p_EscapeType, *this);
	}

	SN_Expression SN_String::StringToInt() const
	{
		return SN_Operators::StringToInt(*this);
	}

	SN_Expression SN_String::StringToDouble() const
	{
		return SN_Operators::StringToDouble(*this);
	}

	SN_Expression SN_String::IsA(const SN_Expression &p_Parent) const
	{
		return SN_Operators::IsA(*this, p_Parent);
	}

	SN_Expression SN_String::Assign(const SN_Expression& p_Right) const
	{
		return SN_Operators::Assign(*this, p_Right);
	}

}
