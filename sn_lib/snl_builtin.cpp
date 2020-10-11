#include "snl_builtin.h"

#include "snl_pch.h"

using namespace std;
using namespace skynet;
using namespace PGCX;

#define SN_INIT(n) \
	n(skynet::n)

#define SN_MAP(n) \
	(*this)[#n] = n;

namespace SNL
{
	SNL_BuiltIn::SNL_BuiltIn()
		: skynet::Domain("BuiltIn")

		// Arithmetic
		, SN_INIT(Negative)
		, SN_INIT(Add)
		, SN_INIT(Subtract)
		, SN_INIT(Multiply)
		, SN_INIT(Divide)
		, SN_INIT(Square)
		, SN_INIT(SquareRoot)

		// * Commands
		, SN_INIT(Assert)
		, SN_INIT(PartialAssert)
		, SN_INIT(Evaluate)
		, SN_INIT(PartialEvaluate)
		, SN_INIT(MetaLevel)

		// * Logic
		, SN_INIT(Not)
		, SN_INIT(Or)
		, SN_INIT(OrNonPartial)
		, SN_INIT(And)
		, SN_INIT(If)
		, SN_INIT(Implies)
		, SN_INIT(RevAnd)
		, SN_INIT(RevOr)
		, SN_INIT(UnaryAnd)
		, SN_INIT(UnaryOr)
		, SN_INIT(Collapse)
		, SN_INIT(Same)

		// * Comparisons
		, SN_INIT(Equals)
		, SN_INIT(NotEquals)
		, SN_INIT(LessThan)
		, SN_INIT(GreaterThan)
		, SN_INIT(LessEqual)
		, SN_INIT(GreaterEqual)

		// * Strings
		, SN_INIT(Concat)
		, SN_INIT(SubtractLeft)
		, SN_INIT(SubtractRight)
		, SN_INIT(SubtractLeftChar)
		, SN_INIT(SubtractRightChar)
		, SN_INIT(SelectLeftChar)
		, SN_INIT(SelectRightChar)
		, SN_INIT(LookaheadLeft)
		, SN_INIT(LookaheadRight)
		, SN_INIT(LookStringLeft)
		, SN_INIT(LookStringRight)
		, SN_INIT(File)

		// * Conversions
		, SN_INIT(EscapeCPP)
		, SN_INIT(UnescapeCPP)
		, SN_INIT(EscapeJSON)
		, SN_INIT(UnescapeJSON)
		, SN_INIT(IntToString)
		, SN_INIT(StringToInt)
		, SN_INIT(DoubleToString)
		, SN_INIT(StringToDouble)

		// * Inheritance
		, SN_INIT(IsA)
		, SN_INIT(HasA)

		// * Mapping, vector.
		, SN_INIT(Subscript)
		, SN_INIT(ReverseSubscript)
		, SN_INIT(CountIf)
		, SN_INIT(CountAll)
		, SN_INIT(Sum)

		// * Sets
		, SN_INIT(BuildSet)
		, SN_INIT(HasMember)
	{
		Link();
	}

	void SNL_BuiltIn::Link()
	{
		// Arithmetic
		SN_MAP(Negative);
		SN_MAP(Add);
		SN_MAP(Subtract);
		SN_MAP(Multiply);
		SN_MAP(Divide);
		SN_MAP(Square);
		SN_MAP(SquareRoot);

		// * Commands
		SN_MAP(Assert);
		SN_MAP(PartialAssert);
		SN_MAP(Evaluate);
		SN_MAP(PartialEvaluate);
		SN_MAP(MetaLevel);

		// * Logic
		SN_MAP(Not);
		SN_MAP(Or);
		SN_MAP(OrNonPartial);
		SN_MAP(And);
		SN_MAP(If);
		SN_MAP(Implies);
		SN_MAP(RevAnd);
		SN_MAP(RevOr);
		SN_MAP(UnaryAnd);
		SN_MAP(UnaryOr);
		SN_MAP(Collapse);
		SN_MAP(Same);

		// * Comparisons
		SN_MAP(Equals);
		SN_MAP(NotEquals);
		SN_MAP(LessThan);
		SN_MAP(GreaterThan);
		SN_MAP(LessEqual);
		SN_MAP(GreaterEqual);

		// * Strings
		SN_MAP(Concat);
		SN_MAP(SubtractLeft);
		SN_MAP(SubtractRight);
		SN_MAP(SubtractLeftChar);
		SN_MAP(SubtractRightChar);
		SN_MAP(SelectLeftChar);
		SN_MAP(SelectRightChar);
		SN_MAP(LookaheadLeft);
		SN_MAP(LookaheadRight);
		SN_MAP(LookStringLeft);
		SN_MAP(LookStringRight);
		SN_MAP(File);

		// * Conversions
		SN_MAP(EscapeCPP);
		SN_MAP(UnescapeCPP);
		SN_MAP(EscapeJSON);
		SN_MAP(UnescapeJSON);
		SN_MAP(IntToString);
		SN_MAP(StringToInt);
		SN_MAP(DoubleToString);
		SN_MAP(StringToDouble);

		// * Inheritance
		SN_MAP(IsA);
		SN_MAP(HasA);

		// * Mapping, vector.
		SN_MAP(Subscript);
		SN_MAP(ReverseSubscript);
		SN_MAP(CountIf);
		SN_MAP(CountAll);
		SN_MAP(Sum);

		// * Sets
		SN_MAP(BuildSet);
		SN_MAP(HasMember);
	}
}