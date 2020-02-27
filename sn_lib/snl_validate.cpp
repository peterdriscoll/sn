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
		SN_LINK(IsFloatingPoint);
		SN_LINK(IsFloatingPointExtension);
		SN_LINK(IsExponential);
		SN_LINK(IsExponentialExtension);

		SN_LINK(IsString);
		SN_LINK(IsName);
		SN_LINK(IsWhiteSpaceOnly);
		SN_LINK(IsSimpleComment);
		SN_LINK(IsLineComment);
		SN_LINK(IsComment);
		SN_LINK(IsWhiteSpace);
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
			SN_LOCAL(f);
			SN_LOCAL(t);
			SN_LOCAL(u);

			(Define(IsFloatingPoint)(f) ==
				Local(t, Local(u, Let(
					f == t + u
				,	IsInteger(t)
				&&	IsFloatingPointExtension(u)
				)))
			).PartialAssert().Do();

			(Define(IsFloatingPointExtension)(f) == (
				(f.LookaheadLeft() == String("."))
				.If(IsInteger(f.SubtractLeftChar())
					, f == String(""))
			)).PartialAssert().Do();
		}

		{
			SN_LOCAL(f);
			SN_LOCAL(t);
			SN_LOCAL(u);

			(Define(IsExponential)(f) == (Let(
				f == t + u
				, IsFloatingPoint(t)
				&& IsExponentialExtension(u)
			))).PartialAssert().Do();

			(Define(IsExponentialExtension)(f) == (
				f.LookaheadLeft() == (String("e") || String("E"))
				&& IsInteger(f.SubtractLeftChar())
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

			(Define(IsString)(s) == (
				s.LookaheadLeft() == String("\"")
			&&	Local(t, Local(u, Let(
					s.SubtractLeftChar() == t + u
				,	IsStringContent(t)
				&&	u.LookaheadLeft() != ""
				&&	(u == String("\""))
				)))
			).Collapse()).PartialAssert().Do();

			(Define(IsStringContent)(s) == (
				(s.LookaheadLeft() != String(""))
			&&	(s.LookaheadLeft() == String("\""))
				.If (s == String("")
				,	(s.LookaheadLeft() == String("\\"))
					.If (s.SubtractLeftChar().LookaheadLeft() != String("")
					&&	IsStringContent(s.SubtractLeftChar().SubtractLeftChar())
					,	IsStringContent(s.SubtractLeftChar())
				))
			)).PartialAssert().Do();
		}

		{
			SN_LOCAL(IsWhiteSpaceOnlyContinuation);
			SN_LOCAL(i);
			(Define(IsWhiteSpaceOnly)(i) == (
				i.LookaheadLeft() != "" && m_CharacterSet.White(i.SelectLeftChar()) && IsWhiteSpaceOnlyContinuation(i.SubtractLeftChar()))).PartialAssert().Do();
			(Define(IsWhiteSpaceOnlyContinuation)(i) == (
				(i.LookaheadLeft() != "" && m_CharacterSet.White(i.LookaheadLeft()))
				.If(IsWhiteSpaceOnlyContinuation(i.SubtractLeftChar()), i == "")
				)).PartialAssert().Do();
		}

		{
			SN_LOCAL(IsSimpleCommentContent);
			SN_LOCAL(s);
			SN_LOCAL(t);
			SN_LOCAL(u);
			SN_LOCAL(v);

			(Define(IsSimpleComment)(s) == (
				s.LookStringLeft(String("/*"))
			&&	Local(t, Local(u, Let(
					s.SubtractLeft(String("/*")) == t + u
				,	IsSimpleCommentContent(t)
				&&	u.LookaheadLeft() != ""
				&&	(u == String("*/"))
				)))
			).Collapse()).PartialAssert().Do();
			
			(Define(IsSimpleCommentContent)(s) == (
				s.LookaheadLeft() != String("")
			&&	s.LookStringLeft(String("*/"))
				.If (s == String("")
				,	IsSimpleCommentContent(s.SubtractLeftChar()))
			)).PartialAssert().Do();
		}

		{
			SN_LOCAL(IsLineCommentContent);
			SN_LOCAL(s);
			SN_LOCAL(t);
			SN_LOCAL(u);
			SN_LOCAL(v);

			(Define(IsLineComment)(s) ==
				(	s.LookStringLeft(String("//"))
				&&	Local(t, Local(u, Let(
						s.SubtractLeft(String("//")) == t + u
					,	IsLineCommentContent(t)
					&&	u.LookaheadLeft() != ""
					&&	u == String("\n")
					)))
				).Collapse()
			).PartialAssert().Do();

			(Define(IsLineCommentContent)(s) == (
					s.LookaheadLeft() != String("")
				&&	(s.LookaheadLeft() == String("\n"))
					.If (s == String("")
					,	IsLineCommentContent(s.SubtractLeftChar()))
			)).PartialAssert().Do();
		}

		{
			SN_LOCAL(s);

			(Define(IsComment)(s) == (IsSimpleComment(s) || IsLineComment(s))).PartialAssert().Do();
		}

		{
			SN_LOCAL(IsWhiteSpaceContinuation);

			SN_LOCAL(IgnoreText);
			SN_LOCAL(s);
			(Define(IgnoreText)(s) == (m_CharacterSet.White(s) || IsComment(s))).PartialAssert().Do();

			SN_LOCAL(i);
			(Define(IsWhiteSpace)(i) == (i.LookaheadLeft() != "" && m_CharacterSet.White(i.SelectLeftChar()) && IsWhiteSpaceContinuation(i.SubtractLeftChar()))).PartialAssert().Do();
			(Define(IsWhiteSpaceContinuation)(i) == (
				(i.LookaheadLeft() != "" && IgnoreText(i.LookaheadLeft()))
				.If(IsWhiteSpaceContinuation(i.SubtractLeftChar()), i == "")
				)).PartialAssert().Do();
		}
	}
}