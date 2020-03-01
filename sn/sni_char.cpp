#include "sni_char.h"

#include "sn_error.h" 
#include "sni_valueset.h"
#include "sni_stringref.h"
#include "sni_string.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ SNI_Class *SNI_Char::m_Class = NULL;
	/*static*/ SNI_Class *SNI_Char::Class()
	{
		if (!m_Class)
		{
			m_Class = new SNI_Class("Char");
		}
		return m_Class;
	}

	SNI_Char::SNI_Char()
	{

	}

	SNI_Char::~SNI_Char()
	{

	}

	SNI_Char::SNI_Char(const SNI_Char &p_Other)
		: m_Char(p_Other.m_Char)
	{

	}

	SNI_Char::SNI_Char(string::value_type p_Other)
		: m_Char(p_Other)
	{

	}

	string::value_type SNI_Char::GetChar() const
	{
		return m_Char;
	}

	string SNI_Char::GetString() const
	{
		return string(1, m_Char);
	}

	string SNI_Char::GetTypeName() const
	{
		return "Char";
	}

	string SNI_Char::DisplayCpp() const
	{
		if (m_Char)
		{
			string quotedString(1, m_Char);
			return "\"" + EscapeStringToCPP(quotedString)+ "\"";
		}
		return "\"\\\0\"";
	}

	string SNI_Char::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		if (m_Char)
		{
			string quotedString(1, m_Char);
			return "\"" + EscapeStringToCPP(quotedString) + "\"";
		}
		return "\"\\\\0\"";
	}

	long SNI_Char::GetPriority() const
	{
		return 100;
	}

	bool SNI_Char::IsString() const
	{
		return true;
	}

	bool SNI_Char::Equivalent(SNI_Object * p_Other) const
	{
		if (dynamic_cast<SNI_Char *>(p_Other))
		{
			SNI_Char *l_Char = dynamic_cast<SNI_Char *>(p_Other);
			return m_Char == l_Char->m_Char;
		}
		if (dynamic_cast<SNI_String *>(p_Other))
		{
			SNI_String *l_String = dynamic_cast<SNI_String *>(p_Other);
			return l_String->GetString().size() == 1 && m_Char == l_String->GetString()[0];
		}
		if (dynamic_cast<SNI_StringRef *>(p_Other))
		{
			SNI_StringRef *l_String = dynamic_cast<SNI_StringRef *>(p_Other);
			return l_String->GetString().size() == 1 && m_Char == l_String->GetString()[0];
		}
		return false;
	}

	size_t SNI_Char::Hash() const
	{
		return _Hash_representation(m_Char);
	}

	// Inheritance
	SN::SN_Error  SNI_Char::AssertIsAValue(const SNI_Value * p_Parent, SN::SN_Expression p_Result)
	{
		return p_Result.AssertValue(Class()->DoIsA(p_Parent));
	}

	SN::SN_Value SNI_Char::DoIsA(const SNI_Value * p_Parent) const
	{
		return Class()->DoIsA(p_Parent);
	}

	SN::SN_Value SNI_Char::DoAdd(SNI_Value * p_Other) const
	{
		return DoConcat(p_Other);
	}

	SN::SN_Value SNI_Char::DoConcat(SNI_Value * p_Other) const
	{
		if (dynamic_cast<SNI_String *>(p_Other))
		{
			SNI_String * l_String = dynamic_cast<SNI_String *>(p_Other);
			return SN::SN_String(string(1, m_Char) + l_String->GetString());
		}
		if (dynamic_cast<SNI_Char *>(p_Other))
		{
			SNI_Char *l_Char = dynamic_cast<SNI_Char *>(p_Other);
			return SN::SN_String(string(1, m_Char) + l_Char->m_Char);
		}
		if (dynamic_cast<SNI_StringRef *>(p_Other))
		{
			SNI_StringRef * l_string = dynamic_cast<SNI_StringRef *>(p_Other);
			return SN::SN_String(string(1, m_Char) + l_string->GetString());
		}
		return SN::SN_Error(false, false, GetTypeName() + " Concat not implemented.");
	}
}
