#if !defined(SNI_STRING_H_INCLUDED)
#define SNI_STRING_H_INCLUDED

#pragma once

#include <string>
using namespace std;

#include "sni_value.h"

namespace SN
{
	class SN_Error;
}

namespace SNI
{
	class SNI_StringRef;

	class SNI_String : public SNI_Value
	{
		PGC_CLASS(SNI_String);
	public:
		static SNI_Class *m_Class;
		static SNI_Class *Class();
		virtual SNI_Class *VClass();

		SNI_String();
		SNI_String(const SNI_String &p_Other);
		SNI_String(const SNI_StringRef &p_Other);
		SNI_String(const string &p_Other);
		virtual ~SNI_String();

		virtual SNI_Expression * Copy() const;

		// Logging
		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

		// Status
		virtual bool IsString() const;


		// Comparisons
		SN::SN_Value DoEquals(SNI_Value * p_Other) const;
		SN::SN_Value DoNotEquals(SNI_Value * p_Other) const;
		SN::SN_Value DoLessThan(SNI_Value * p_Other) const;
		SN::SN_Value DoGreaterThan(SNI_Value * p_Other) const;
		SN::SN_Value DoLessEquals(SNI_Value * p_Other) const;
		SN::SN_Value DoGreaterEquals(SNI_Value * p_Other) const;

		// Members
		virtual string GetString() const;
		virtual size_t Length() const;
		virtual SN::SN_String GetSource() const;
		virtual	SN::SN_Expression GetStart() const;
		virtual	SN::SN_Expression GetEnd() const;
		virtual const string &GetSourceString() const;
		virtual long GetLeftMostPos() const;
		virtual long GetRightMostPos() const;

		// Base
		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual size_t Hash() const;
		virtual SN::SN_Error AssertValue(const SN::SN_Expression &p_Value);

		// Implementation
		virtual SN::SN_Value DoAdd(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoSubtract(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoConcat(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoSubtractLeft(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoSubtractRight(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoSubtractLeftChar() const;
		virtual SN::SN_Value DoSubtractRightChar() const;
		virtual	SN::SN_Value DoSelectLeftChar() const;
		virtual	SN::SN_Value DoSelectRightChar() const;
		virtual	SN::SN_Value DoLookaheadLeft() const;
		virtual	SN::SN_Value DoLookaheadRight() const;
		virtual	SN::SN_Value DoLookStringLeft(SNI_Value * p_Other) const;
		virtual	SN::SN_Value DoLookStringRight(SNI_Value * p_Other) const;
		virtual	SN::SN_Value DoFile() const;

		virtual void DoWriteFile(const SN::SN_Value & p_Contents) const;

		virtual SN::SN_Value DoEscape(enum skynet::EscapeType p_EscapeType) const;
		virtual SN::SN_Value DoUnescape(enum skynet::EscapeType p_EscapeType) const;
		virtual SN::SN_Value DoStringToInt() const;
		virtual SN::SN_Value DoStringToDouble() const;

	private:
		string m_String;
	};

	typedef vector<SNI_String *> SNI_StringList;
}

#endif // !defined(SNI_STRING_H_INCLUDED)
