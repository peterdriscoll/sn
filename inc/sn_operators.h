#pragma once

#include "exp_ctrl_sn.h"
#include "sn_function.h"
#include "sn_functiondef.h"

namespace SN
{
	class SN_Expression;

	enum EscapeType
	{
		CPP, JSON
	};

	extern SN_FunctionDef EscapeCPP;
	extern SN_FunctionDef UnescapeCPP;
	extern SN_FunctionDef EscapeJSON;
	extern SN_FunctionDef UnescapeJSON;

	namespace SN_Operators
	{
		SN_EXPORT SN_Value GeneralDoOr(const SN_Value &p_Left, const SN_Value &p_Right);

		// Commands
		SN_EXPORT SN_Expression Assert(const SN_Expression &p_Expression);
		SN_EXPORT SN_Expression PartialAssert(const SN_Expression &p_Expression);
		SN_EXPORT SN_Expression Evaluate(const SN_Expression &p_Expression);
		SN_EXPORT SN_Expression PartialEvaluate(const SN_Expression &p_Expression);
		SN_EXPORT SN_Expression BuildMeta(const SN_Expression &p_Expression, const SN_Expression &p_MetaLevel);
		SN_EXPORT SN_Expression Notes(const SN_Expression &p_Expression, const SN_Expression &p_Before, const SN_Expression &p_After);

		// Logic
		SN_EXPORT SN_Expression operator !(const SN_Expression &p_Other);
		SN_EXPORT SN_Expression operator-(const SN_Expression & p_Condition);
		SN_EXPORT SN_Expression operator &&(const SN_Expression &left, const SN_Expression &right);
		SN_EXPORT SN_Expression operator ||(const SN_Expression &left, const SN_Expression &right);
		SN_EXPORT SN_Expression If(const SN_Expression &p_Condition, const SN_Expression &p_PositiveCase, const SN_Expression &p_NegativeCase);
		SN_EXPORT SN_Expression Implies(const SN_Expression &p_Condition, const SN_Expression &p_PositiveCase);
		SN_EXPORT SN_Expression RevAnd(const SN_Expression &p_Condition, const SN_Expression &p_PositiveCase);
		SN_EXPORT SN_Expression RevOr(const SN_Expression &p_PositiveCase, const SN_Expression &p_Condition);
		SN_EXPORT SN_Expression UnaryAnd(const SN_Expression &p_Condition);
		SN_EXPORT SN_Expression UnaryOr(const SN_Expression &p_Result);
		SN_EXPORT SN_Expression Collapse(const SN_Expression &p_Value);

		// Comparison
		SN_EXPORT SN_Expression operator ==(const SN_Expression &p_Left, const SN_Expression &p_Right);
		SN_EXPORT SN_Expression operator !=(const SN_Expression &p_Left, const SN_Expression &p_Right);
		SN_EXPORT SN_Expression operator <(const SN_Expression &p_Left, const SN_Expression &p_Right);
		SN_EXPORT SN_Expression operator >(const SN_Expression &p_Left, const SN_Expression &p_Right);
		SN_EXPORT SN_Expression operator >=(const SN_Expression &p_Left, const SN_Expression &p_Right);
		SN_EXPORT SN_Expression operator <=(const SN_Expression &p_Left, const SN_Expression &p_Right);
		SN_EXPORT SN_Expression operator +(const SN_Expression &p_Left, const SN_Expression &p_Right);
		SN_EXPORT SN_Expression operator -(const SN_Expression &p_Left, const SN_Expression &p_Right);
		SN_EXPORT SN_Expression operator *(const SN_Expression &p_Left, const SN_Expression &p_Right);
		SN_EXPORT SN_Expression operator /(const SN_Expression &p_Left, const SN_Expression &p_Right);
		SN_EXPORT SN_Expression operator -(const SN_Expression &p_Param);

		// Strings
		SN_EXPORT SN_Expression Concat(const SN_Expression & p_Result, const SN_Expression & p_Left);
		SN_EXPORT SN_Expression SubtractLeft(const SN_Expression &p_Result, const SN_Expression &p_Left);
		SN_EXPORT SN_Expression SubtractRight(const SN_Expression &p_Result, const SN_Expression &p_Right);
		SN_EXPORT SN_Expression SubtractLeftChar(const SN_Expression &p_Left);
		SN_EXPORT SN_Expression SubtractRightChar(const SN_Expression &p_Left);
		SN_EXPORT SN_Expression SelectLeftChar(const SN_Expression &p_Left);
		SN_EXPORT SN_Expression SelectRightChar(const SN_Expression &p_Left);
		SN_EXPORT SN_Expression LookaheadLeft(const SN_Expression &p_Left);
		SN_EXPORT SN_Expression LookaheadRight(const SN_Expression &p_Left);
		SN_EXPORT SN_Expression LookStringLeft(const SN_Expression &p_Left, const SN_Expression &p_Right);
		SN_EXPORT SN_Expression LookStringRight(const SN_Expression &p_Left, const SN_Expression &p_Right);
		SN_EXPORT SN_Expression File(const SN_Expression &p_Left);

		// Numbers
		SN_EXPORT SN_Expression Square(const SN_Expression &p_Expression);
		SN_EXPORT SN_Expression SquareRoot(const SN_Expression &p_Expression);

		// Inheritance
		SN_Expression IsA(const SN_Expression &p_Child, const SN_Expression &p_Parent);
		SN_Expression HasA(const SN_Expression &p_Parent, const SN_Expression &p_Component, const SN_Expression &p_Name);

		// Sets
		SN_Expression BuildSet(const SN_Expression &p_Expression);
		SN_Expression HasMember(const SN_Expression &p_Set, const SN_Expression &p_Member);

		// Mapings
		SN_Expression SubscriptCall(const SN_Expression &p_Mapping, const SN_Expression &p_Index);
		SN_Expression ReverseSubscript(const SN_Expression &p_Mapping, const SN_Expression &p_Index);
		SN_Expression CountIf(const SN_Expression &p_Mapping, const SN_Expression &p_Lambda);
		SN_Expression CountAll(const SN_Expression &p_Mapping);
		SN_Expression Sum(const SN_Expression &p_Mapping);

		SN_Expression Escape(enum EscapeType p_EscapeType, const SN_Expression &p_Left);
		SN_Expression Unescape(enum EscapeType p_EscapeType, const SN_Expression &p_Left);

		// Conversions
		SN_Expression IntToString(const SN_Expression &p_Left);
		SN_Expression StringToInt(const SN_Expression &p_Left);
		SN_Expression DoubleToString(const SN_Expression &p_Left);
		SN_Expression StringToDouble(const SN_Expression &p_Left);

		// Function calls
		SN_Function FunctionCall(const SN_Expression& p_Function, const SN_Expression& p_Parameter);
//		SN_Function AssignmentCall(const SN_Expression& p_Left, const SN_Expression& p_Right, const SN_Expression& p_State);

		// Attributes
		SN_Expression GetState(const SN_Expression& p_Left);
		SN_Expression GetValue(const SN_Expression& p_Left);

		// Imperative
		SN_Expression Assign(const SN_Expression& p_Left, const SN_Expression& p_Right);
	}
}
