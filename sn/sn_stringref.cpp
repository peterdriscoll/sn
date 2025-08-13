#include "sn_stringref.h"
#include "sn_char.h"
#include "sn_string.h"
#include "sn_value.h"
#include "sn_error.h"

#include "sni_stringref.h"
#include "sni_char.h"
#include "sni_string.h"
#include "sni_value.h"

#include "sn_pch.h"

namespace SN
{
	/*static*/ SN_Class SN_StringRef::Class()
	{
		return SNI_StringRef::Class();
	}

	SN_StringRef::SN_StringRef()
		: SN_Base()
	{
	}

	SN_StringRef::SN_StringRef(const SN_Value & p_Source, const SN_Expression & p_Start, const SN_Expression &p_End)
		: SN_Base(new SNI::SNI_StringRef(p_Source, p_Start, p_End))
	{
	}

	SN_StringRef::SN_StringRef(const SN_Expression &p_Expression)
		: SN_Base(dynamic_cast<SNI::SNI_StringRef *>(p_Expression.GetSNI_Expression()))
	{
	}

	SN_StringRef::SN_StringRef(SNI::SNI_StringRef* p_StringRef)
		: SN_Base(p_StringRef)
	{
	}

	SN_StringRef::~SN_StringRef()
	{
	}

	SN_Expression SN_StringRef::IsA(const SN_Expression &p_Parent) const
	{
		return SN_Operators::IsA(*this, p_Parent);
	}

	// Members
	SNI::SNI_StringRef * SN_StringRef::GetSNI_StringRef() const
	{
		return static_cast<SNI::SNI_StringRef *>(m_Expression);
	}

	std::string SN_StringRef::GetString() const
	{
		return GetSNI_StringRef()->GetString();
	}

	SN_String SN_StringRef::GetSource() const
	{
		return GetSNI_StringRef()->GetSource();
	}

	SN_Expression SN_StringRef::GetStart() const
	{
		return GetSNI_StringRef()->GetStart();
	}

	SN_Expression SN_StringRef::GetEnd() const
	{
		return GetSNI_StringRef()->GetEnd();
	}

	// Construction
	SN_Expression SN_StringRef::operator +(const SN_Expression  &p_Other) const
	{
		return SN_Operators::operator +(*this, p_Other);
	}

	SN_Expression SN_StringRef::operator-(const SN_Expression & p_Other) const
	{
		return SN_Operators::operator -(*this, p_Other);
	}

	SN_Expression SN_StringRef::operator ==(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator ==(*this, p_Other);
	}

	SN_Expression SN_StringRef::operator !=(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator !=(*this, p_Other);
	}

	SN_Expression SN_StringRef::operator <(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator <(*this, p_Other);
	}

	SN_Expression SN_StringRef::operator >(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator >(*this, p_Other);
	}

	SN_Expression SN_StringRef::operator <=(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator <=(*this, p_Other);
	}

	SN_Expression SN_StringRef::operator >=(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator >=(*this, p_Other);
	}

	SN_Expression SN_StringRef::Concat(const SN_Expression &p_Other) const
	{
		return SN_Operators::Concat(*this, p_Other);
	}

	SN_Expression SN_StringRef::SubtractLeft(const SN_Expression &p_Other) const
	{
		return SN_Operators::SubtractLeft(*this, p_Other);
	}

	SN_Expression SN_StringRef::SubtractRight(const SN_Expression &p_Other) const
	{
		return SN_Operators::SubtractRight(*this, p_Other);
	}

	SN_Expression SN_StringRef::SubtractLeftChar() const
	{
		return SN_Operators::SubtractLeftChar(*this);
	}

	SN_Expression SN_StringRef::SubtractRightChar() const
	{
		return SN_Operators::SubtractRightChar(*this);
	}

	SN_Expression SN_StringRef::SelectLeftChar() const
	{
		return SN_Operators::SelectLeftChar(*this);
	}

	SN_Expression SN_StringRef::SelectRightChar() const
	{
		return SN_Operators::SelectRightChar(*this);
	}

	SN_Expression SN_StringRef::LookaheadLeft() const
	{
		return SN_Operators::LookaheadLeft(*this);
	}

	SN_Expression SN_StringRef::LookaheadRight() const
	{
		return SN_Operators::LookaheadRight(*this);
	}

	SN_Expression SN_StringRef::LookStringLeft(const SN_Expression &p_Other) const
	{
		return SN_Operators::LookStringLeft(m_Expression, p_Other);
	}

	SN_Expression SN_StringRef::LookStringRight(const SN_Expression &p_Other) const
	{
		return SN_Operators::LookStringRight(m_Expression, p_Other);
	}

	SN_Expression SN_StringRef::File() const
	{
		return SN_Operators::File(*this);
	}

	// Conversion
	SN_Expression SN_StringRef::Escape(enum SN::EscapeType p_EscapeType) const
	{
		return SN_Operators::Escape(p_EscapeType, *this);
	}

	SN_Expression SN_StringRef::Unescape(enum SN::EscapeType p_EscapeType) const
	{
		return SN_Operators::Unescape(p_EscapeType, *this);
	}

	SN_Expression SN_StringRef::StringToInt() const
	{
		return SN_Operators::StringToInt(*this);
	}

	SN_Expression SN_StringRef::StringToDouble() const
	{
		return SN_Operators::StringToDouble(*this);
	}
}
