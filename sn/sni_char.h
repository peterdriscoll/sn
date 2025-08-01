#if !defined(SNI_CHAR_H_INCLUDED)
#define SNI_CHAR_H_INCLUDED

#pragma once

#include "sni_value.h"

namespace SNI
{
	class SNI_Char : public SNI_Value
	{
		PGC_CLASS(SNI_Char);
	public:
		static SNI_Class * m_Class;
		static SNI_Class * Class();
		SNI_Class *VClass();

		SNI_Char();
		SNI_Char(const SNI_Char &p_Other);
		SNI_Char(string::value_type p_Other);

		SNI_Expression * Copy() const;

		virtual ~SNI_Char();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

		virtual bool IsString() const;
		virtual bool Equivalent(SNI_Object * p_Other) const;

		virtual size_t Hash() const;

		virtual SN::SN_Value DoAdd(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoConcat(SNI_Value * p_Other) const;

		virtual string::value_type GetChar() const;
		virtual string GetString() const;

	private:
		string::value_type m_Char;
	};

	typedef vector<SNI_Char> SNI_CharList;
}

#endif // !defined(SNI_CHAR_H_INCLUDED)
