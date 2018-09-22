#include "sni_char.h"

#include "sn_error.h" 
#include "sni_valueset.h"
#include "sni_stringref.h"
#include "sni_string.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ SNI_Instance *SNI_Char::m_Instance = NULL;
	/*static*/ SNI_Instance *SNI_Char::Instance()
	{
		if (!m_Instance)
		{
			m_Instance = new SNI_Instance();
		}
		return m_Instance;
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

	string SNI_Char::GetTypeName() const
	{
		return "Char";
	}

	string SNI_Char::DisplayCpp() const
	{
		string quotedString(m_Char, 1);
		ReplaceAll(quotedString, "\"", "\\\"");
		return "\"" + quotedString + "\"";
	}

	string SNI_Char::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		string quotedString(m_Char, 1);
		ReplaceAll(quotedString, "\"", "\\\"");
		return "\"" + quotedString + "\"";
	}

	long SNI_Char::GetPriority() const
	{
		return 100;
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

	// Inheritance
	SN::SN_Error  SNI_Char::AssertIsAValue(const SNI_Value * p_Parent, SN::SN_Expression p_Result)
	{
		return p_Result.AssertValue(Instance()->DoIsA(p_Parent));
	}

	SN::SN_Value SNI_Char::DoIsA(const SNI_Value * p_Parent) const
	{
		return Instance()->DoIsA(p_Parent);
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
		return SN::SN_Error(GetTypeName() + " Concat not implemented.");
	}
}
