#include "SNL_Validate.h"
#include "snl_CharacterSet.h"

#include "snl_pch.h"

using namespace std;
using namespace skynet;
using namespace PGCX;

namespace SNL
{
	SNL_Validate::SNL_Validate(const SNL_CharacterSet &p_CharacterSet)
		: skynet::Domain("Parser")
		, m_CharacterSet(p_CharacterSet)
	{
		Setup();
	}

	void SNL_Validate::Setup()
	{
		Link();
		Init();
	}

	void SNL_Validate::Link()
	{
		SN_LINK(IsInteger);
		SN_LINK(IsString);
		SN_LINK(IsName);
	}

	void SNL_Validate::Init()
	{
		{
			SN_LOCAL(IsIntegerContinuation);
			SN_LOCAL(i);
			(Define(IsInteger)(i) == (i.LookaheadLeft() != "" && m_CharacterSet.Digit(i.SelectLeftChar()) && IsIntegerContinuation(i.SubtractLeftChar()))).PartialAssert().Do();
			(Define(IsIntegerContinuation)(i) == (
				(i.LookaheadLeft() != "" && m_CharacterSet.Digit(i.LookaheadLeft()))
				.If(IsIntegerContinuation(i.SubtractLeftChar()), i == "")
			)).PartialAssert().Do();
		}

		{
			SN_LOCAL(IsNameContinuation);
			SN_LOCAL(i);

			(Define(IsName)(i) == (i.LookaheadLeft() != "" && m_CharacterSet.AlphaUnder(i.SelectLeftChar()) && IsNameContinuation(i.SubtractLeftChar()))).PartialAssert().Do();
			(Define(IsNameContinuation)(i) == (
				(i.LookaheadLeft() != "" && m_CharacterSet.AlphaUnderNumeric(i.LookaheadLeft()))
				.If(IsNameContinuation(i.SubtractLeftChar()), i == "")
			)).PartialAssert().Do();

		}

		{
			//	IsString.
			//	The Let expression is designed  to never fail, but the expression
			//	cannot quite deduce that the input is definitely a string. If
			//	it is asserted then it works but if you assert the result equal
			//  to a variable, it thinks it could be true or false. This is not an
			//	error, it is logically correct.
			//	Collapse converts [true] or [true, false], to true. It returns false
			//	otherwise.

			SN_LOCAL(IsStringContent);
			SN_LOCAL(s);
			SN_LOCAL(t);
			SN_LOCAL(u);
			SN_LOCAL(v);

			(Define(IsString)(s) == (((s.SelectLeftChar() == String("\"")) && Local(t, Local(u, Let(s.SubtractLeftChar() == t + u, IsStringContent(t) && (u == String("\""))))))).Collapse()).PartialAssert().Do();
			(Define(IsStringContent)(s) == ((s.LookaheadLeft() == String("\"")).If(s == String(""), (s.LookaheadLeft() == String("\\")).If(IsStringContent(s.SubtractLeftChar().SubtractLeftChar()), IsStringContent(s.SubtractLeftChar()))))).PartialAssert().Do();
		}
	}
}