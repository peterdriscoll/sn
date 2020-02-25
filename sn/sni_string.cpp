#include "sni_string.h"

#include "sn_char.h"
#include "sn_error.h"

#include "sni_stringref.h"
#include "sni_char.h"
#include "sni_valueset.h"
#include "sni_variable.h"
#include "sni_null.h"
#include "sni_error.h"
#include "sni_class.h"
#include "utility.h"

#include <fstream>
#include <sstream>

#include "sn_pch.h"

namespace SNI
{
	/*static*/ SNI_Class *SNI_String::m_Class = NULL;
	/*static*/ SNI_Class *SNI_String::Class()
	{
		if (!m_Class)
		{
			m_Class = new SNI_Class("String");
		}
		return m_Class;
	}

	SNI_String::SNI_String()
	{
	}

	SNI_String::SNI_String(const SNI_String &p_Other)
		: m_String(p_Other.m_String)
	{
	}

	SNI_String::SNI_String(const SNI_StringRef &p_Other)
		: m_String(p_Other.GetString())
	{
	}

	SNI_String::SNI_String(const string &p_Other)
		: m_String(p_Other)
	{
	}

	SNI_String::~SNI_String()
	{
	}

	//-----------------------------------------------------------------------
	// Logging
	//-----------------------------------------------------------------------

	string SNI_String::GetTypeName() const
	{
		return "String";
	}

	string SNI_String::DisplayCpp() const
	{
		return "\"" + EscapeStringToCPP(m_String) + "\"";
	}

	string SNI_String::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		string quotedString = EscapeStringToJSON(m_String);
		return "\"" + EscapeStringToCPP(m_String) + "\"";
	}

	long SNI_String::GetPriority() const
	{
		return 100;
	}

	bool SNI_String::IsString() const
	{
		return true;
	}

	// Comparisons
	SN::SN_Value SNI_String::DoEquals(SNI_Value *p_Other) const
	{
		if (dynamic_cast<SNI_String *>(p_Other))
		{
			SNI_String * l_string = dynamic_cast<SNI_String *>(p_Other);
			return SN::SN_Bool(m_String == l_string->m_String);
		}
		if (dynamic_cast<SNI_StringRef *>(p_Other))
		{
			SNI_StringRef * l_string = dynamic_cast<SNI_StringRef *>(p_Other);
			return SN::SN_Bool(m_String == l_string->GetString());
		}
		if (dynamic_cast<SNI_Char *>(p_Other))
		{
			SNI_Char * l_char = dynamic_cast<SNI_Char *>(p_Other);
			return SN::SN_Bool(m_String == l_char->GetString());
		}
		return SN::SN_Error(GetTypeName() + " Equals not implemented.");
	}

	SN::SN_Value SNI_String::DoNotEquals(SNI_Value *p_Other) const
	{
		if (dynamic_cast<SNI_String *>(p_Other))
		{
			SNI_String * l_string = dynamic_cast<SNI_String *>(p_Other);
			return SN::SN_Bool(m_String != l_string->m_String);
		}
		if (dynamic_cast<SNI_StringRef *>(p_Other))
		{
			SNI_StringRef * l_string = dynamic_cast<SNI_StringRef *>(p_Other);
			return SN::SN_Bool(m_String != l_string->GetString());
		}
		if (dynamic_cast<SNI_Char *>(p_Other))
		{
			SNI_Char * l_char = dynamic_cast<SNI_Char *>(p_Other);
			return SN::SN_Bool(m_String != l_char->GetString());
		}
		return SN::SN_Error(GetTypeName() + " Equals not implemented.");
	}

	SN::SN_Value SNI_String::DoLessThan(SNI_Value * p_Other) const
	{
		if (dynamic_cast<SNI_String *>(p_Other))
		{
			SNI_String * l_string = dynamic_cast<SNI_String *>(p_Other);
			return SN::SN_Bool(m_String < l_string->m_String);
		}
		if (dynamic_cast<SNI_StringRef *>(p_Other))
		{
			SNI_StringRef * l_string = dynamic_cast<SNI_StringRef *>(p_Other);
			return SN::SN_Bool(m_String < l_string->GetString());
		}
		if (dynamic_cast<SNI_Char *>(p_Other))
		{
			SNI_Char * l_char = dynamic_cast<SNI_Char *>(p_Other);
			return SN::SN_Bool(m_String < l_char->GetString());
		}
		return SN::SN_Error(GetTypeName() + " Equals not implemented.");
	}

	SN::SN_Value SNI_String::DoGreaterThan(SNI_Value * p_Other) const
	{
		if (dynamic_cast<SNI_String *>(p_Other))
		{
			SNI_String * l_string = dynamic_cast<SNI_String *>(p_Other);
			return SN::SN_Bool(m_String > l_string->m_String);
		}
		if (dynamic_cast<SNI_StringRef *>(p_Other))
		{
			SNI_StringRef * l_string = dynamic_cast<SNI_StringRef *>(p_Other);
			return SN::SN_Bool(m_String > l_string->GetString());
		}
		if (dynamic_cast<SNI_Char *>(p_Other))
		{
			SNI_Char * l_char = dynamic_cast<SNI_Char *>(p_Other);
			return SN::SN_Bool(m_String > l_char->GetString());
		}
		return SN::SN_Error(GetTypeName() + " Greater than not implemented.");
	}

	SN::SN_Value SNI_String::DoLessEquals(SNI_Value * p_Other) const
	{
		if (dynamic_cast<SNI_String *>(p_Other))
		{
			SNI_String * l_string = dynamic_cast<SNI_String *>(p_Other);
			return SN::SN_Bool(m_String <= l_string->m_String);
		}
		if (dynamic_cast<SNI_StringRef *>(p_Other))
		{
			SNI_StringRef * l_string = dynamic_cast<SNI_StringRef *>(p_Other);
			return SN::SN_Bool(m_String <= l_string->GetString());
		}
		if (dynamic_cast<SNI_Char *>(p_Other))
		{
			SNI_Char * l_char = dynamic_cast<SNI_Char *>(p_Other);
			return SN::SN_Bool(m_String <= l_char->GetString());
		}
		return SN::SN_Error(GetTypeName() + " Less than or equal not implemented.");
	}

	SN::SN_Value SNI_String::DoGreaterEquals(SNI_Value * p_Other) const
	{
		if (dynamic_cast<SNI_String *>(p_Other))
		{
			SNI_String * l_string = dynamic_cast<SNI_String *>(p_Other);
			return SN::SN_Bool(m_String >= l_string->m_String);
		}
		if (dynamic_cast<SNI_StringRef *>(p_Other))
		{
			SNI_StringRef * l_string = dynamic_cast<SNI_StringRef *>(p_Other);
			return SN::SN_Bool(m_String >= l_string->GetString());
		}
		if (dynamic_cast<SNI_Char *>(p_Other))
		{
			SNI_Char * l_char = dynamic_cast<SNI_Char *>(p_Other);
			return SN::SN_Bool(m_String >= l_char->GetString());
		}
		return SN::SN_Error(GetTypeName() + " Greater than or equal not implemented.");
	}

	//-----------------------------------------------------------------------
	// Members
	//-----------------------------------------------------------------------

	string SNI_String::GetString() const
	{
		return m_String;
	}

	const string &SNI_String::GetSourceString() const
	{
		return m_String;
	}

	long SNI_String::GetLeftMostPos() const
	{
		return 0;
	}

	long SNI_String::GetRightMostPos() const
	{
		return (long) m_String.length();
	}

	size_t SNI_String::Length() const
	{
		return (long) m_String.length();
	}

	SN::SN_String SNI_String::GetSource() const
	{
		return this;
	}

	SN::SN_Expression SNI_String::GetStart() const
	{
		return SN::SN_Long(0);
	}

	SN::SN_Expression SNI_String::GetEnd() const
	{
		return SN::SN_Long((long) Length());
	}

	bool SNI_String::Equivalent(SNI_Object * p_Other) const
	{
		if (dynamic_cast<SNI_String *>(p_Other))
		{
			SNI_String * l_string = dynamic_cast<SNI_String *>(p_Other);
			return m_String == l_string->m_String;
		}
		if (dynamic_cast<SNI_StringRef *>(p_Other))
		{
			SNI_StringRef * l_string = dynamic_cast<SNI_StringRef *>(p_Other);
			return m_String == l_string->GetString();
		}
		if (dynamic_cast<SNI_Char *>(p_Other))
		{
			if (m_String.length() == 1)
			{
				SNI_Char * l_char = dynamic_cast<SNI_Char *>(p_Other);
				return m_String[0] == l_char->GetChar();
			}
			return false;
		}
		if (dynamic_cast<SNI_ValueSet *>(p_Other))
		{
			return p_Other->Equivalent(dynamic_cast<SNI_Object *>(const_cast<SNI_String *>(this)));
		}
		return false;
	}

	size_t SNI_String::Hash() const
	{
		return _Hash_array_representation(m_String.c_str(), m_String.size());
	}

	SN::SN_Error SNI_String::AssertValue(const SN::SN_Expression &p_Value)
	{
		if (p_Value.IsError())
		{
			return p_Value;
		}
		if (SN::Is<SNI_StringRef*>(p_Value) || SN::Is<SNI_ValueSet*>(p_Value) || SN::Is<SNI_Variable*>(p_Value) || !p_Value.IsKnownValue())
		{
			SN::SN_Expression value(p_Value);
			return value.AssertValue(this);
		}
		return Equivalent(p_Value.GetSNI_Expression());
	}

	// Inheritance
	SN::SN_Error  SNI_String::AssertIsAValue(const SNI_Value * p_Parent, SN::SN_Expression p_Result)
	{
		return p_Result.AssertValue(Class()->DoIsA(p_Parent));
	}

	SN::SN_Value SNI_String::DoIsA(const SNI_Value * p_Parent) const
	{
		return Class()->DoIsA(p_Parent);
	}

	//-----------------------------------------------------------------------
	// Implementation
	//-----------------------------------------------------------------------

	SN::SN_Value SNI_String::DoAdd(SNI_Value * p_Other) const
	{
		return DoConcat(p_Other);
	}

	SN::SN_Value SNI_String::DoSubtract(SNI_Value * p_Other) const
	{
		return DoSubtractRight(p_Other);
	}

	SN::SN_Value SNI_String::DoConcat(SNI_Value * p_Other) const
	{
		if (dynamic_cast<SNI_String *>(p_Other))
		{
			SNI_String * l_string = dynamic_cast<SNI_String *>(p_Other);
			return SN::SN_String(m_String + l_string->m_String);
		}
		if (dynamic_cast<SNI_StringRef *>(p_Other))
		{
			SNI_StringRef * l_string = dynamic_cast<SNI_StringRef *>(p_Other);
			return SN::SN_String(m_String + l_string->GetString());
		}
		if (dynamic_cast<SNI_Char *>(p_Other))
		{
			SNI_Char * l_char = dynamic_cast<SNI_Char *>(p_Other);
			return SN::SN_String(m_String + l_char->GetChar());
		}
		return SN::SN_Error(GetTypeName() + " Concat not implemented.");
	}

	SN::SN_Value SNI_String::DoSubtractLeft(SNI_Value * p_Other) const
	{
		if (dynamic_cast<SNI_String *>(p_Other))
		{
			string part = dynamic_cast<SNI_String *>(p_Other)->m_String;
			size_t part_length = part.length();
			if (m_String.substr(0, part_length) == part)
			{
				return SN::SN_String(m_String.substr(part_length));
			}
			return SN::SN_Error(GetTypeName() + " Subtract left did not match.");
		}
		return SN::SN_Error(GetTypeName() + " - operator not implemented.");
	}

	SN::SN_Value SNI_String::DoSubtractRight(SNI_Value * p_Other) const
	{
		if (dynamic_cast<SNI_String *>(p_Other))
		{
			string part = dynamic_cast<SNI_String *>(p_Other)->m_String;
			size_t part_length = part.length();
			if (m_String.substr(m_String.length() - part_length) == part)
			{
				string result = m_String.substr(0, m_String.length() - part_length);
				return SN::SN_String(result);
			}
			return SN::SN_Error(GetTypeName() + " Subtract right did not match.");
		}
		return SN::SN_Error(GetTypeName() + " - operator not implemented.");
	}

	SN::SN_Value SNI_String::DoSubtractLeftChar() const
	{
		if (0 < m_String.length())
		{
			return SN::SN_String(m_String.substr(1));
		}
		return SN::SN_Error(GetTypeName() + " SubtractLeftChar method failed. String too short.");
	}

	SN::SN_Value SNI_String::DoSubtractRightChar() const
	{
		if (0 < m_String.length())
		{
			return SN::SN_String(m_String.substr(0, m_String.length() - 1));
		}
		return SN::SN_Error(GetTypeName() + " SubtractRightChar method failed. String too short.");
	}

	SN::SN_Value SNI_String::DoSelectLeftChar() const
	{
		if (0 < m_String.length())
		{
			return SN::SN_Char(m_String[0]);
		}
		return SN::SN_Error(GetTypeName() + " SelectLeftChar operator not implemented on a null string.");
	}

	SN::SN_Value SNI_String::DoSelectRightChar() const
	{
		size_t length = m_String.length();
		if (0 < length)
		{
			return SN::SN_Char(m_String[length - 1]);
		}
		return SN::SN_Error(GetTypeName() + " SelectRightChar operator not implemented on a null string.");
	}

	SN::SN_Value SNI_String::DoLookaheadLeft() const
	{
		if (m_String.length())
		{
			return SN::SN_Char(m_String[0]);
		}
		return SN::SN_String("");
	}

	SN::SN_Value SNI_String::DoLookaheadRight() const
	{
		size_t length = m_String.length();
		if (length)
		{
			return SN::SN_Char(m_String[length - 1]);
		}
		return SN::SN_String("");
	}

	SN::SN_Value SNI_String::DoLookStringLeft(SNI_Value * p_Other) const
	{
		string other = p_Other->GetString();
		size_t length = m_String.length();
		size_t otherLength = other.length();
		if (m_String.length())
		{
			return SN::SN_Bool(m_String.substr(0, otherLength) == other);
		}
		return skynet::False;
	}

	SN::SN_Value SNI_String::DoLookStringRight(SNI_Value * p_Other) const
	{
		string other = p_Other->GetString();
		size_t length = m_String.length();
		size_t otherLength = other.length();
		if (otherLength < length)
		{
			return SN::SN_Bool(m_String.substr(length-otherLength, otherLength) == other);
		}
		return skynet::False;
	}

	SN::SN_Value SNI_String::DoFile() const
	{
		// In math, a single expression can only have a single value. Calling the File
		// method on a string can only have one value. So if the file is reread the
		// same value must be returned.
		// The solution to this to include the time, but I don't yet have a time class
		// or a branching time object to allow me to safely retrieve it. Later.
		auto &preventReread = SNI_Thread::GetThread()->GetProcessor()->GetPreventReread();
		auto it  = preventReread.find(GetString());
		if (it == preventReread.end())
		{
			ifstream inFile;
			inFile.open(GetString()); // open the input file
			if (!inFile.is_open())
			{
				return skynet::Null;
			}

			stringstream strStream;
			strStream << inFile.rdbuf(); // read the file
			SN::SN_String contents(strStream.str());

			preventReread[GetString()] = contents;
			return contents;
		}
		return it->second;
	}

	void SNI_String::DoWriteFile(const SN::SN_Value &p_Contents) const
	{
		std::ofstream out(GetString());
		out << p_Contents.GetString();
		out.close();
		auto &preventReread = SNI_Thread::GetThread()->GetProcessor()->GetPreventReread();
		preventReread[GetString()] = p_Contents;
	}

	SN::SN_Value SNI_String::DoEscape(enum skynet::EscapeType p_EscapeType) const
	{
		switch (p_EscapeType)
		{
		case skynet::CPP:
			return SN::SN_String(EscapeStringToCPP(GetString()));
		case skynet::JSON:
			return SN::SN_String(EscapeStringToJSON(GetString()));
		}
		return SN::SN_Error("Bad escape type for escape conversion");
	}

	SN::SN_Value SNI_String::DoUnescape(enum skynet::EscapeType p_EscapeType) const
	{
		switch (p_EscapeType)
		{
		case skynet::CPP:
			return SN::SN_String(UnescapeStringToCPP(GetString()));
		case skynet::JSON:
			return SN::SN_String(UnescapeStringToJSON(GetString()));
		}
		return SN::SN_Error("Bad escape type for escape conversion");
	}

	SN::SN_Value SNI_String::DoStringToInt() const
	{
		size_t len = m_String.size();

		if (len < 10)
		{
			return SN::SN_Long(stol(m_String));
		}
		return SN::SN_LongLong(stoll(m_String));
	}
}
