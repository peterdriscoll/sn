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
		SNI_Char(std::string::value_type p_Other);

		SNI_Expression * Copy() const;

		virtual ~SNI_Char();

		virtual std::string GetTypeName() const;
		virtual std::string DisplayCpp() const;
		virtual std::string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

		virtual bool IsString() const;
		virtual bool Equivalent(SNI_Object * p_Other) const;

		virtual size_t Hash() const;

		virtual SN::SN_Value DoAdd(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoConcat(SNI_Value * p_Other) const;

		virtual std::string::value_type GetChar() const;
		virtual std::string GetString() const;

	private:
		std::string::value_type m_Char;
	};

	typedef std::vector<SNI_Char> SNI_CharList;
}

#endif // !defined(SNI_CHAR_H_INCLUDED)
