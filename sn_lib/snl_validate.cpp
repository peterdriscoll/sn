#include "snl_validate.h"
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
		SN_LINK(IsUnsignedInteger);
		SN_LINK(IsFloatingPoint);
		SN_LINK(IsFloatingPointExtension);
		SN_LINK(IsExponential);
		SN_LINK(IsExponentialExtension);

		SN_LINK(IsString);
		SN_LINK(IsWhiteSpaceOnly);
		SN_LINK(IsSimpleComment);
		SN_LINK(IsLineComment);
		SN_LINK(IsComment);
		SN_LINK(IsWhiteSpace);
		SN_LINK(IsWhiteSpaceContinuation);
		SN_LINK(IsName);
		SN_LINK(IsPath);

		SN_LINK(IsCall);
		SN_LINK(IsLambda);

	}

	void SNL_Validate::Init()
	{
		DefineWhiteSpace();
		DefineNumber();
		DefineString();
		DefineNamePath();
	}

	void SNL_Validate::DefineWhiteSpace()
	{
		{
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
				&& Local(t, Local(u, Let(
					s.SubtractLeft(String("/*")) == t + u
					, IsSimpleCommentContent(t)
					&& u.LookaheadLeft() != ""
					&& (u == String("*/"))
				)))
				).Collapse()).PartialAssert().Do();

			(Define(IsSimpleCommentContent)(s) == (
				s.LookaheadLeft() != String("")
				&& s.LookStringLeft(String("*/"))
				.If(s == String("")
					, IsSimpleCommentContent(s.SubtractLeftChar()))
				)).PartialAssert().Do();
		}

		{
			SN_LOCAL(IsLineCommentContent);
			SN_LOCAL(s);
			SN_LOCAL(t);
			SN_LOCAL(u);
			SN_LOCAL(v);

			(Define(IsLineComment)(s) ==
				(s.LookStringLeft(String("//"))
					&& Local(t, Local(u, Let(
						s.SubtractLeft(String("//")) == t + u
						, IsLineCommentContent(t)
						&& u.LookaheadLeft() != ""
						&&	u == String("\n")
					)))
					).Collapse()
				).PartialAssert().Do();

			(Define(IsLineCommentContent)(s) == (
				s.LookaheadLeft() != String("")
				&& (s.LookaheadLeft() == String("\n"))
				.If(s == String("")
					, IsLineCommentContent(s.SubtractLeftChar()))
				)).PartialAssert().Do();
		}

		{
			SN_LOCAL(s);

			(Define(IsComment)(s) == (IsSimpleComment(s) || IsLineComment(s))).PartialAssert().Do();
		}

		{
			SN_LOCAL(s);

			SN_LOCAL(m);
			SN_LOCAL(n);

			(Define(IsWhiteSpace)(s) == (
				m_CharacterSet.White(s.LookaheadLeft()) && IsWhiteSpaceContinuation(s.SubtractLeftChar())
			||	Local(m, Local(n, Let(s == m + n,
					IsComment(m) && IsWhiteSpace(n)
				)))
			)).PartialAssert().Do();

			(Define(IsWhiteSpaceContinuation)(s) == (
				IsWhiteSpace(s).If(True, s == String(""))
			)).PartialAssert().Do();
		}
	}

	void SNL_Validate::DefineString()
	{
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
				&& Local(t, Local(u, Let(
					s.SubtractLeftChar() == t + u
					, IsStringContent(t)
					&& u.LookaheadLeft() != ""
					&& (u == String("\""))
				)))
				).Collapse()).PartialAssert().Do();

			(Define(IsStringContent)(s) == (
				(s.LookaheadLeft() != String(""))
				&& (s.LookaheadLeft() == String("\""))
				.If(s == String("")
					, (s.LookaheadLeft() == String("\\"))
					.If(s.SubtractLeftChar().LookaheadLeft() != String("")
						&& IsStringContent(s.SubtractLeftChar().SubtractLeftChar())
						, IsStringContent(s.SubtractLeftChar())
					))
				)).PartialAssert().Do();
		}
	}
	void SNL_Validate::DefineNumber()
	{
		{
			SN_LOCAL(i);
			(Define(IsInteger)(i) == (i.LookaheadLeft() != "" && (((m_CharacterSet.Sign(i.LookaheadLeft()) && IsUnsignedInteger(i.SubtractLeftChar())) || IsUnsignedInteger(i))))).PartialAssert().Do();
		}

		{
			SN_LOCAL(IsIntegerContinuation);
			SN_LOCAL(i);
			(Define(IsUnsignedInteger)(i) == (i.LookaheadLeft() != "" && m_CharacterSet.Digit(i.SelectLeftChar()) && IsIntegerContinuation(i.SubtractLeftChar()))).PartialAssert().Do();
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
					, IsInteger(t)
					&& (IsFloatingPointExtension(u) || u == String(""))
				)))
				).PartialAssert().Do();

			(Define(IsFloatingPointExtension)(f) == (
				(f.LookaheadLeft() == String(".")) && IsUnsignedInteger(f.SubtractLeftChar())
				)).PartialAssert().Do();
		}

		{
			SN_LOCAL(f);
			SN_LOCAL(t);
			SN_LOCAL(u);

			(Define(IsExponential)(f) == (
				Local(t, Local(u, Let(
					f == t + u
					, IsFloatingPoint(t)
					&& (IsExponentialExtension(u) || u == String(""))
				)))
				)).PartialAssert().Do();

			(Define(IsExponentialExtension)(f) == (
				((f.LookaheadLeft() == (String("e") || String("E"))).Collapse() && IsInteger(f.SubtractLeftChar()))
				)).PartialAssert().Do();
		}
	}

	void SNL_Validate::DefineNamePath()
	{
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
			SN_LOCAL(i);

			SN_LOCAL(m);
			SN_LOCAL(n);

			(Define(IsPath)(i) ==
				Local(m, Local(n, Let(i == m + n,
					IsName(m)
				&&	(	(n.LookaheadLeft() == String(".")
					&&	IsPath(n.SubtractLeftChar())
					|| n == String("")))
				)))
			).PartialAssert().Do();
		}
	}
}