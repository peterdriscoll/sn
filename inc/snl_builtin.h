#if !defined(SNL_BUILTIN_H_INCLUDED)
#define SNL_BUILTIN_H_INCLUDED

#pragma once

#include "exp_ctrl_sn_lib.h"
#include "sn.h"

namespace SNL
{
	class SN_LIB_EXPORT SNL_BuiltIn: public skynet::Domain
	{
	public:
		SNL_BuiltIn();

		// Arithmetic
		skynet::FunctionDef Negative;
		skynet::FunctionDef Add;
		skynet::FunctionDef Subtract;
		skynet::FunctionDef Multiply;
		skynet::FunctionDef Divide;
		skynet::FunctionDef Square;
		skynet::FunctionDef SquareRoot;

		// * Commands
		skynet::FunctionDef Assert;
		skynet::FunctionDef PartialAssert;
		skynet::FunctionDef Evaluate;
		skynet::FunctionDef PartialEvaluate;
		skynet::FunctionDef MetaLevel;

		// * Logic
		skynet::FunctionDef Not;
		skynet::FunctionDef Or;
		skynet::FunctionDef OrNonPartial;
		skynet::FunctionDef And;
		skynet::FunctionDef If;
		skynet::FunctionDef Implies;
		skynet::FunctionDef RevAnd;
		skynet::FunctionDef RevOr;
		skynet::FunctionDef UnaryAnd;
		skynet::FunctionDef UnaryOr;
		skynet::FunctionDef Collapse;
		skynet::FunctionDef Same;

		// * Comparisons
		skynet::FunctionDef Equals;
		skynet::FunctionDef NotEquals;
		skynet::FunctionDef LessThan;
		skynet::FunctionDef GreaterThan;
		skynet::FunctionDef LessEqual;
		skynet::FunctionDef GreaterEqual;

		// * Strings
		skynet::FunctionDef Concat;
		skynet::FunctionDef SubtractLeft;
		skynet::FunctionDef SubtractRight;
		skynet::FunctionDef SubtractLeftChar;
		skynet::FunctionDef SubtractRightChar;
		skynet::FunctionDef SelectLeftChar;
		skynet::FunctionDef SelectRightChar;
		skynet::FunctionDef LookaheadLeft;
		skynet::FunctionDef LookaheadRight;
		skynet::FunctionDef LookStringLeft;
		skynet::FunctionDef LookStringRight;
		skynet::FunctionDef File;

		// * Conversions
		skynet::FunctionDef EscapeCPP;
		skynet::FunctionDef UnescapeCPP;
		skynet::FunctionDef EscapeJSON;
		skynet::FunctionDef UnescapeJSON;
		skynet::FunctionDef IntToString;
		skynet::FunctionDef StringToInt;
		skynet::FunctionDef DoubleToString;
		skynet::FunctionDef StringToDouble;

		// * Inheritance
		skynet::FunctionDef IsA;
		skynet::FunctionDef HasA;

		// * Mapping, vector.
		skynet::FunctionDef Subscript;
		skynet::FunctionDef ReverseSubscript;
		skynet::FunctionDef CountIf;
		skynet::FunctionDef CountAll;
		skynet::FunctionDef Sum;

		// * Sets
		skynet::FunctionDef BuildSet;
		skynet::FunctionDef HasMember;

	private:
		void Link();
	};
}

#endif // !defined(SNL_BUILTIN_H_INCLUDED)
