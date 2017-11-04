#if !defined(SN_OPERATORS_H_INCLUDED)
#define SN_OPERATORS_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_base.h"

namespace SN
{
	namespace SN_Operators
	{
		SN_EXPORT SN::SN_Value GeneralDoOr(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right);
	
		// Logic
		SN_EXPORT SN::SN_Expression operator !(const SN::SN_Expression &p_Other);
		SN_EXPORT SN::SN_Expression operator-(const SN::SN_Expression & p_Condition);
		SN_EXPORT SN::SN_Expression operator &&(const SN::SN_Expression &left, const SN::SN_Expression &right);
		SN_EXPORT SN::SN_Expression operator ||(const SN::SN_Expression &left, const SN::SN_Expression &right);
		SN_EXPORT SN::SN_Expression If(const SN::SN_Expression &p_Condition, const SN::SN_Expression &p_PositiveCase, const SN::SN_Expression &p_NegativeCase);
		SN_EXPORT SN::SN_Expression Implies(const SN::SN_Expression &p_Condition, const SN::SN_Expression &p_PositiveCase);
		SN_EXPORT SN::SN_Expression RevAnd(const SN::SN_Expression &p_Condition, const SN::SN_Expression &p_PositiveCase);
		SN_EXPORT SN::SN_Expression RevOr(const SN::SN_Expression &p_PositiveCase, const SN::SN_Expression &p_Condition);
		SN_EXPORT SN::SN_Expression UnaryAnd(const SN::SN_Expression &p_Condition);
		SN_EXPORT SN::SN_Expression UnaryOr(const SN::SN_Expression &p_Result);
		SN_EXPORT SN::SN_Expression Collapse(const SN::SN_Expression &p_Value);

		// Comparison
		SN_EXPORT SN::SN_Expression operator ==(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right);
		SN_EXPORT SN::SN_Expression operator <(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right);
		SN_EXPORT SN::SN_Expression operator >(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right);
		SN_EXPORT SN::SN_Expression operator >=(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right);
		SN_EXPORT SN::SN_Expression operator <=(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right);
		SN_EXPORT SN::SN_Expression operator +(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right);
		SN_EXPORT SN::SN_Expression operator -(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right);
		SN_EXPORT SN::SN_Expression operator *(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right);
		SN_EXPORT SN::SN_Expression operator /(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right);
		SN_EXPORT SN::SN_Expression operator -(const SN::SN_Expression &p_Param);

		// Strings
		SN_EXPORT SN::SN_Expression Concat(const SN::SN_Expression & p_Result, const SN::SN_Expression & p_Left);
		SN_EXPORT SN::SN_Expression SubtractLeft(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Left);
		SN_EXPORT SN::SN_Expression SubtractRight(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Right);
		SN_EXPORT SN::SN_Expression SubtractLeftChar(const SN::SN_Expression &p_Left);
		SN_EXPORT SN::SN_Expression SubtractRightChar(const SN::SN_Expression &p_Left);
		SN_EXPORT SN::SN_Expression SelectLeftChar(const SN::SN_Expression &p_Left);
		SN_EXPORT SN::SN_Expression SelectRightChar(const SN::SN_Expression &p_Left);
		SN_EXPORT SN::SN_Expression LookaheadLeft(const SN::SN_Expression &p_Left);
		SN_EXPORT SN::SN_Expression LookaheadRight(const SN::SN_Expression &p_Left);
		SN_EXPORT SN::SN_Expression File(const SN::SN_Expression &p_Left);

		// Numbers
		SN_EXPORT SN::SN_Expression Square(const SN::SN_Expression &p_Expression);
		SN_EXPORT SN::SN_Expression SquareRoot(const SN::SN_Expression &p_Expression);

		// Sets
		SN::SN_Expression BuildSet(const SN::SN_Expression &p_Expression);
		SN::SN_Expression HasMember(const SN::SN_Expression &p_Set, const SN::SN_Expression &p_Member);

		// Conversions
		SN::SN_Expression IntToString(const SN::SN_Expression &p_Left);
		SN::SN_Expression StringToInt(const SN::SN_Expression &p_Left);
		SN::SN_Expression DoubleToString(const SN::SN_Expression &p_Left);
		SN::SN_Expression StringToDouble(const SN::SN_Expression &p_Left);

		// Function calls
		SN::SN_Expression FunctionCall(const SN::SN_Expression &p_Function, const SN::SN_Expression &p_Parameter);
		SN::SN_Expression FunctionCall(const SN::SN_Expression &p_Function, const SN::SN_Expression &p_Parameter, const SN::SN_Expression &p_Condition);
	}
}

#endif // !defined(SN_OPERATORS_H_INCLUDED)
