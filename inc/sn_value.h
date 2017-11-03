#if !defined(SN_VALUE_H_INCLUDED)
#define SN_VALUE_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_expression.h"

#include <string>
#include <functional>
using namespace std;

namespace SNI
{
	class SNI_Value;
	class SNI_Local;
	class SNI_Char;
	class SNI_Expression;
	class SNI_Variable;
}

namespace SN
{
	class SN_String;
	class SN_Bool;
	class SN_Null;
	class SN_Error;
	class SN_Number;
	class SN_Char;
	class SN_Variable;
	class SN_Meta;
	class SN_StringRef;
	class SN_ValueSet;

	SN_APPLY_TYPES(DUMMY, DUMMY, SN_FORWARD)

	class SN_EXPORT SN_Value : public SN_Expression
	{
	public:
		SN_Value();
		SN_Value(const SN_Expression &p_other);
		SN_Value(const SN_Value &p_other);
		SN_Value(const SN_String &p_String);
		SN_Value(const SN_StringRef &p_String);
		SN_Value(const SN_Bool &p_Bool);
		SN_Value(const SN_Null &p_Null);
		SN_Value(const SN_Error &p_Error);
		SN_Value(const SN_Char &p_Char);
		SN_Value(const SN_Variable &p_Variable);
		SN_Value(const SN_Meta &p_Meta);
		SN_Value(const SN_ValueSet &p_ValueSet);
		SN_Value(const SN_Number &p_Number);

		SN_APPLY_TYPES(SN_Value, DUMMY, SN_CONSTRUCTOR)

		SN_Value(const SNI::SNI_Expression *p_other);

		virtual ~SN_Value();

		SNI::SNI_Value * GetSNI_Value() const;
	};

	typedef vector<SN_Value> SN_ValueList;
}

#endif // !defined(SN_VALUE_H_INCLUDED)
