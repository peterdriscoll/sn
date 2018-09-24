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
		static SNI_Class *Class();

		SNI_String();
		SNI_String(const SNI_String &p_Other);
		SNI_String(const SNI_StringRef &p_Other);
		SNI_String(const string &p_Other);
		virtual ~SNI_String();

		// Logging
		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

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
		virtual SN::SN_Error AssertValue(const SN::SN_Expression &p_Value);

		// Inheritance
		virtual SN::SN_Error AssertIsAValue(const SNI_Value * p_Parent, SN::SN_Expression p_Result);
		virtual SN::SN_Value DoIsA(const SNI_Value * p_Parent) const;

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
		virtual	SN::SN_Value DoFile() const;

		void DoWriteFile(const SN::SN_Value & p_Contents) const;

	private:
		static SNI_Class *m_Class;

		string m_String;
	};

	typedef vector<SNI_String *> SNI_StringList;
}

#endif // !defined(SNI_STRING_H_INCLUDED)
