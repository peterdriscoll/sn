#if !defined(SN_STRING_H_INCLUDED)
#define SN_STRING_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_base.h"

namespace SNI
{
	class SNI_String;
	class SNI_Value;
}

namespace SN
{
	class SN_Expression;
	class SN_Char;

	class SN_EXPORT SN_String : public SN_Base<SNI::SNI_String, SN_Expression, SN_Error>
	{
	public:
		static SN_Class Class();

		SN_String();
		SN_String(const SN_String &p_Other);
		SN_String(const SN_Expression &p_Other);
		SN_String(const SN_Value &p_other);
		SN_String(const SN_Char &p_Char);

		SN_String(const SNI::SNI_Value *p_Value);

		SN_String(const string &p_other);
		SN_String(const char *p_String);
		SN_String(const char p_Char);
		virtual ~SN_String();

		SN::SN_Expression operator ==(const SN::SN_Expression &p_Other) const;
		SN::SN_Expression operator +(const SN::SN_Expression &p_Other) const;
		SN::SN_Expression operator -(const SN::SN_Expression &p_Other) const;

		SN::SN_Expression operator ||(const SN::SN_Expression &p_Other) const;

		SN::SN_Expression Concat(const SN::SN_Expression & p_Other) const;
		SN::SN_Expression SubtractLeft(const SN::SN_Expression & p_Other) const;
		SN::SN_Expression SubtractRight(const SN::SN_Expression & p_Other) const;
		SN::SN_Expression SubtractLeftChar() const;
		SN::SN_Expression SubtractRightChar() const;
		SN::SN_Expression SelectLeftChar() const;
		SN::SN_Expression SelectRightChar() const;
		SN::SN_Expression LookaheadLeft() const;
		SN::SN_Expression LookaheadRight() const;
		SN::SN_Expression File() const;

		// Conversions
		SN_Expression Escape(enum EscapeType p_EscapeType) const;
		SN_Expression Unescape(enum EscapeType p_EscapeType) const;
		SN_Expression StringToInt() const;
		SN_Expression StringToDouble() const;

		string GetString() const;
		size_t Length() const;

		SN_Expression IsA(const SN_Expression &p_Parent) const;

		SNI::SNI_String * GetSNI_String() const;
	};
}

#endif // !defined(SN_STRING_H_INCLUDED)
