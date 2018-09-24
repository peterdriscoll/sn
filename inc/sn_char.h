#if !defined(SN_CHAR_H_INCLUDED)
#define SN_CHAR_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_base.h"

#include <string>
using namespace std;

namespace SNI
{
	class SNI_Char;
}

namespace SN
{
	class SN_Expression;
	class SN_Value;

	class SN_EXPORT SN_Char : public SN_Base<SNI::SNI_Char, SN_Expression, SN_Error>
	{
	public:
		static SN_Class Class();

		SN_Char();
		SN_Char(const SN_Char &p_Other);
		SN_Char(const SN_Value &p_other);
		SN_Char(string::value_type p_other);

		virtual ~SN_Char();

		SN_Expression operator ==(const SN_Expression &p_Other) const;
		SN_Expression operator <(const SN_Expression &p_Other) const;
		SN_Expression operator >(const SN_Expression &p_Other) const;
		SN_Expression operator <=(const SN_Expression &p_Other) const;
		SN_Expression operator >=(const SN_Expression &p_Other) const;
		SN_Expression operator +(const SN_Expression &p_Other) const;
		SN_Expression operator ||(const SN_Expression &p_Other) const;

		SN_Expression IsA(const SN_Expression &p_Parent) const;

		string::value_type GetChar() const;
		SNI::SNI_Char *GetSNI_Char() const;

	private:
		SNI::SNI_Char *m_Char;
	};
}

#endif // !defined(SN_CHAR_H_INCLUDED)
