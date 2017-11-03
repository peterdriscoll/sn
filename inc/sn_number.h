#if !defined(SN_NUMBER_H_INCLUDED)
#define SN_NUMBER_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"

#include "sn_macros.h"

#include <string>
#include <vector>
using namespace std;

namespace SNI
{
	class SNI_Number;
	class SNI_Expression;
}

namespace SN
{
	class SN_Expression;
	class SN_String;
	class SN_Null;
	class SN_Error;
	class SN_Value;

	template <typename T>
	class SN_Real;

	SN_APPLY_TYPES(SN_Expression, DUMMY, SN_FORWARD)

	class SN_EXPORT SN_Number
	{
	public:
		SN_Number();
		SN_Number(const SN_Expression &p_other);
		SN_Number(const SN_Number &p_other);

		SN_APPLY_TYPES(SN_Number, DUMMY, SN_CONSTRUCTOR)
		
		SN_Number(const SN_Value &p_Value);
		SN_Number(SNI::SNI_Number *p_other);
		SN_Number(SNI::SNI_Expression *p_other);

		SN_Number(const bool &p_Bool);
		SN_Number(const long &p_long);
		SN_Number(const int &p_int);

		SN_Number(const SN_Error &p_Error);

		virtual ~SN_Number();

		SN_Expression operator ==(const SN_Expression &p_Other) const;
		SN_Expression operator +(const SN_Expression &p_Other) const;
		SN_Expression operator&&(const SN_Expression &right) const;
		SN_Expression operator||(const SN_Expression &right) const;

		bool Equivalent(const SN_Expression &p_Other) const;

		SNI::SNI_Number * GetSNI_Number() const;
		string GetString() const;

	private:
		SNI::SNI_Number * m_Number;
	};

	typedef vector<SN_Number> SN_NumberList;
}

#endif // !defined(SN_NUMBER_H_INCLUDED)
