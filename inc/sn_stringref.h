#if !defined(SN_STRINGREF_H_INCLUDED)
#define SN_STRINGREF_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_base.h"

#include <vector>
using namespace std;

namespace SNI
{
	class SNI_StringRef;
	class SNI_String;
	class SNI_Value;
	class SNI_Object;
}

namespace SN
{
	class SN_Expression;
	class SN_Char;
	class SN_String;
	class SN_Value;
	class SN_Error;

	class SN_EXPORT SN_StringRef : public SN_Base<SNI::SNI_StringRef, SN_Expression, SN_Error>
	{
	public:
		static SN_Class Class();

		SN_StringRef();
		SN_StringRef(const SN_Value &p_Source, const SN_Expression &p_Start, const SN_Expression &p_End);
		SN_StringRef(const SN_Expression &p_other);
		virtual ~SN_StringRef();

		// Constructionm
		SN_Expression operator ==(const SN_Expression &p_Other) const;
		SN_Expression operator +(const SN_Expression &p_Other) const;
		SN_Expression operator -(const SN_Expression &p_Other) const;

		SN_Expression Concat(const SN_Expression &p_Other) const;
		SN_Expression SubtractLeft(const SN_Expression &p_Other) const;
		SN_Expression SubtractRight(const SN_Expression &p_Other) const;
		SN_Expression SubtractLeftChar() const;
		SN_Expression SubtractRightChar() const;
		SN_Expression SelectLeftChar() const;
		SN_Expression SelectRightChar() const;
		SN_Expression LookaheadLeft() const;
		SN_Expression LookaheadRight() const;
		SN_Expression File() const;

		// Members
		string GetString() const;
		SN_String GetSource() const;
		SN_Expression GetStart() const;
		SN_Expression GetEnd() const;

		// Inheritance
		SN_Expression IsA(const SN_Expression &p_Parent) const;

		SNI::SNI_StringRef * GetSNI_StringRef() const;
	};

	typedef vector<SN_StringRef> SN_StringRefList;
}

#endif // !defined(SN_STRING_H_INCLUDED)
