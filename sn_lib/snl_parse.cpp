#include "snl_parse.h"

#include "snl_pch.h"

#include "snl_characterset.h"

using namespace std;
using namespace skynet;
using namespace PGCX;

namespace SNL
{
	SNL_Parse::SNL_Parse(const SNL_CharacterSet p_CharacterSet, const SNL_Validate &p_Validate)
		: skynet::Domain("Parse")
		, m_Validate(p_Validate)
		, m_CharacterSet(p_CharacterSet)
	{
		Setup();
	}

	void SNL_Parse::Setup()
	{
		Link();
		Init();
	}

	void SNL_Parse::Link()
	{
		SN_LINK(AsExpression);
		SN_LINK(AsLambda);
		SN_LINK(AsLet);
		SN_LINK(AsLocal);
		SN_LINK(AsIf);

		SN_LINK(AsBooleanFactor);
		SN_LINK(AsBooleanTerm);
		SN_LINK(AsBooleanExpression);

		SN_LINK(AsComparisonExpression);

		SN_LINK(AsArithmeticExpression);
		SN_LINK(AsArithmeticTerm);
		SN_LINK(AsArithmeticFactor);

		SN_LINK(AsFunctionCall);
		SN_LINK(AsSubscript);
		SN_LINK(AsPath);

		SN_LINK(AsFactor);

		SN_LINK(AsBracket);
		SN_LINK(AsName);
		SN_LINK(AsValue);

		SN_LINK(AsBoolean);

		SN_LINK(AsInteger);
		SN_LINK(AsScientific);
		SN_LINK(AsNumber);

		SN_LINK(AsString);

		SN_LINK(AsValueRef);
	}

	void SNL_Parse::Init()
	{
		DefineExpression();
		DefineBooleanExpression();
		DefineComparisonExpression();
		DefineArithmeticExpression();

		DefineFunctionCall();
		DefineSubscript();
		DefinePath();

		DefineFactor();

		DefineValue();
	}

	void SNL_Parse::DefineExpression()
	{
		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			(Define(AsExpression)(d)(s)(i) == (
				AsLambda(d)(s)(i)
			||	AsLet(d)(s)(i)
			||	AsLocal(d)(s)(i)
			||	AsIf(d)(s)(i)
			||	AsBooleanExpression(d)(s)(i)
			)).PartialAssert().Do();
		}
		string sAsExpression = AsExpression.DisplayValueSN();

		// Lambda
		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			SN_LOCAL(m);
			SN_LOCAL(n);
			SN_LOCAL(l);
			SN_LOCAL(c);

			SN_LOCAL(p);
			SN_LOCAL(q);

			SN_LOCAL(w1);
			SN_LOCAL(w2);

			(Define(AsLambda)(d)(s)(i) == (
				Local(m, Local(n, Local(l, Local(p, Local(q, Local(c, Let(
					s == w1 + l + m + c + w2 + n
				,	m_Validate.IsWhiteSpaceContinuation(w1)
				&&	m_CharacterSet.Lambda(l)
				&&	AsName(d)(m)(p)
				&&	c == Char('.')
				&&	m_Validate.IsWhiteSpaceContinuation(w2)
				&&	AsExpression(d)(n)(q)
				&&	i == Meta(1, Lambda(Meta(-1, p), Meta(-1, q))).Notes(w1, w2)
				)))))))
			)).PartialAssert().Do();
		}
		string sAsLambda = AsLambda.DisplayValueSN();

		// Let
		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			SN_LOCAL(m);
			SN_LOCAL(n);

			SN_LOCAL(l);
			SN_LOCAL(k);

			SN_LOCAL(p);
			SN_LOCAL(q);

			(Define(AsLet)(d)(s)(i) == (
				Local(m, Local(n, Local(p, Local(q, Local(l, Local(k, Let(
					s == l + m + k + n
				,	l == String("let")
				&&	AsExpression(d)(m)(p)
				&&	k == String("in")
				&&	AsExpression(d)(n)(q)
				&&	i == Meta(1, Let(Meta(-1, p), Meta(-1, q))
				))))))))
			)).PartialAssert().Do();
		}
		string sAsLet = AsLet.DisplayValueSN();

		// Local
		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			SN_LOCAL(m);
			SN_LOCAL(n);

			SN_LOCAL(p);
			SN_LOCAL(q);

			SN_LOCAL(l);

			SN_LOCAL(AsLocalInternal);

			(Define(AsLocal)(d)(s)(i) == (
				Local(m, Local(l, Let(
					s == l + m
				,	l == String("local")
				&&	AsLocalInternal(d)(m)(i)
				)))
			)).PartialAssert().Do();

			(Define(AsLocalInternal)(d)(s)(i) == (
				Local(m, Local(n, Local(p, Local(q, Let(s == m + n,
					AsName(d)(m)(p)
				&&	(	n.LookaheadLeft() == Char(',') && AsLocalInternal(d)(n.SubtractLeftChar())(q)
					||	n.LookStringLeft(String("in")) && AsExpression(d)(n.SubtractLeft(String("in"))(q)))
				&& i == Meta(1, Local(Meta(-1, p), Meta(-1, q)))
				)))))
			)).PartialAssert().Do();
			string sAsLocalInternal = AsLocalInternal.DisplayValueSN();
		}
		string sAsLocal = AsLocal.DisplayValueSN();

		// If
		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			SN_LOCAL(m);
			SN_LOCAL(n);
			SN_LOCAL(o);

			SN_LOCAL(p);
			SN_LOCAL(q);
			SN_LOCAL(r);

			SN_LOCAL(c);
			SN_LOCAL(t);
			SN_LOCAL(e);

			(Define(AsIf)(d)(s)(i) == (
				Local(m, Local(n, Local(o, Local(p, Local(q, Local(r, Local(c, Local(t, Local(e, Let(s == c + m + t + n + e + o,
					c == String("if")
				&&	AsExpression(d)(m)(p)
				&&	t == String("then")
				&&	AsExpression(d)(n)(q)
				&&	e == String("else")
				&&	AsExpression(d)(o)(r)
				&&	i == Meta(1, Meta(-1, p).If(Meta(-1, q), Meta(-1, r)))
				))))))))))
			)).PartialAssert().Do();
		}
		string sAsIf = AsIf.DisplayValueSN();
	}

	void SNL_Parse::DefineBooleanExpression()
	{
		// Boolean expression ||
		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			SN_LOCAL(m);
			SN_LOCAL(n);

			SN_LOCAL(p);
			SN_LOCAL(q);

			(Define(AsBooleanExpression)(d)(s)(i) ==
				Local(m, Local(n, Local(p, Local(q, Let(s == m + n,
					AsBooleanTerm(d)(m)(p)
					&& (n.LookaheadLeft() == Char('|')).If(
						AsBooleanExpression(d)(n.SubtractLeftChar())(q)
					&&	i == Meta(1, Meta(-1, p) || Meta(-1, q))
					, (n == String("") && i == p))
				)))))
			).PartialAssert().Do();
		}
		string sAsBooleanExpression = AsBooleanExpression.DisplayValueSN();

		// Boolean term &&
		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			SN_LOCAL(m);
			SN_LOCAL(n);

			SN_LOCAL(p);
			SN_LOCAL(q);

			(Define(AsBooleanTerm)(d)(s)(i) ==
				Local(m, Local(n, Local(p, Local(q, Let(s == m + n,
					AsBooleanFactor(d)(m)(p)
					&& (n.LookaheadLeft() == Char('&')).If(
						AsBooleanTerm(d)(n.SubtractLeftChar())(q)
						&& i == Meta(1, Meta(-1, p) && Meta(-1, q))
						, (n == String("") && i == p))
				)))))
			).PartialAssert().Do();
		}
		string sAsBooleanTerm = AsBooleanTerm.DisplayValueSN();

		// Boolean factor
		// Some trickery here. A boolean factor could be
		// a boolean value, comparison, name path or function call.
		// But that would lead to an unfactorized first element.
		// Instead, allow a comparison to be an arithmetic expression.
		// Long(5) & Long(6) will parse. Let the execution sort it out. 
		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			(Define(AsBooleanFactor) == AsComparisonExpression).PartialAssert().Do();
		}
		string sAsBooleanFactor = AsBooleanFactor.DisplayValueSN();
	}

	void SNL_Parse::DefineComparisonExpression()
	{
		// Comparison expression.
		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			SN_LOCAL(m);
			SN_LOCAL(n);

			SN_LOCAL(p);
			SN_LOCAL(q);

			(Define(AsComparisonExpression)(d)(s)(i) ==
				Local(m, Local(n, Local(p, Local(q, Let(s == m + n,
					AsArithmeticExpression(d)(m)(p)
					&&	(	n.LookaheadLeft() == Char('<') && AsArithmeticExpression(d)(n.SubtractLeftChar())(q) && i == Meta(1, Meta(-1, p) < Meta(-1, q))
						||	n.LookaheadLeft() == Char('>') && AsArithmeticExpression(d)(n.SubtractLeftChar())(q) && i == Meta(1, Meta(-1, p) > Meta(-1, q))
						||	n.LookaheadLeft() == Char('=') && AsArithmeticExpression(d)(n.SubtractLeftChar())(q) && i == Meta(1, Meta(-1, p) = Meta(-1, q))
						||	n.LookStringLeft(String("<=")) && AsArithmeticExpression(d)(n.SubtractLeft(String("<=")))(q) && i == Meta(1, Meta(-1, p) <= Meta(-1, q))
						||	n.LookStringLeft(String(">=")) && AsArithmeticExpression(d)(n.SubtractLeft(String(">=")))(q) && i == Meta(1, Meta(-1, p) > Meta(-1, q))
						||	(n == String("") && i == p)) // Allow a comparison to be an arithmetic expression.
				)))))
			).PartialAssert().Do();
		}
		string sAsComparisonExpression = AsComparisonExpression.DisplayValueSN();
	}

	void SNL_Parse::DefineArithmeticExpression()
	{
		// Arithmetic expression + -
		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			SN_LOCAL(m);
			SN_LOCAL(n);

			SN_LOCAL(p);
			SN_LOCAL(q);

			(Define(AsArithmeticExpression)(d)(s)(i) ==
				Local(m, Local(n, Local(p, Local(q, Let(s == m + n,
					AsArithmeticTerm(d)(m)(p)
					&&	(n.LookaheadLeft() == (Char('+') || Char('-'))).Collapse().If(
							AsArithmeticExpression(d)(n.SubtractLeftChar())(q)
							&&	i == (n.LookaheadLeft() == Char('+')).If(
									Meta(1, Meta(-1, p) + Meta(-1, q))
								,	Meta(1, Meta(-1, p) - Meta(-1, q)))
						,	(n == String("") && i == p))
				)))))
			).PartialAssert().Do();
		}
		string sAsArithmeticExpression = AsArithmeticExpression.DisplayValueSN();

		// Arithmetic term * /
		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			SN_LOCAL(m);
			SN_LOCAL(n);

			SN_LOCAL(p);
			SN_LOCAL(q);

			(Define(AsArithmeticTerm)(d)(s)(i) ==
				Local(m, Local(n, Local(p, Local(q, Let(s == m + n,
					AsArithmeticFactor(d)(m)(p)
					&& (n.LookaheadLeft() == (Char('*') || Char('/'))).Collapse().If(
						AsArithmeticTerm(d)(n.SubtractLeftChar())(q)
						&& i == (n.LookaheadLeft() == Char('*')).If(
							Meta(1, Meta(-1, p) * Meta(-1, q))
							, Meta(1, Meta(-1, p) / Meta(-1, q)))
						, (n == String("") && i == p))
				)))))
			).PartialAssert().Do();
		}
		string sAsArithmeticTerm = AsArithmeticTerm.DisplayValueSN();

		// Arithmetic factor
		{
			(Define(AsArithmeticFactor) == AsFunctionCall).PartialAssert().Do();
		}
		string sAsArithmeticFactor = AsArithmeticFactor.DisplayValueSN();
	}

	void SNL_Parse::DefineFunctionCall()
	{
		{
			SN_LOCAL(AsFunctionCallExt);
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			SN_LOCAL(m);
			SN_LOCAL(n);

			SN_LOCAL(w);

			SN_LOCAL(p);
			SN_LOCAL(q);

			(Define(AsFunctionCall)(d)(s)(i) == (
				Local(m, Local(n, Local(p, Let(s == m + n,
					AsSubscript(d)(m)(p)
				&&	AsFunctionCallExt(d)(n)(i)(p)
				))))
			)).PartialAssert().Do();

			(Define(AsFunctionCallExt)(d)(s)(i)(p) == (
				Local(m, Local(w, Local(n, Local(q, Let(s == w + m + n,
					(m_Validate.IsWhiteSpaceContinuation(w) && !m_CharacterSet.OperatorChar(m.LookaheadLeft())).If(
						AsSubscript(d)(m)(i)(q)
					&&  AsFunctionCallExt(d)(n)(i)(Meta(1, Meta(-1, p)[Meta(-1, q)]))
						, s == w && i == p
					)
				)))))
			)).PartialAssert().Do();
			string sAsFunctionCallExt = AsFunctionCallExt.DisplayValueSN();
		}
		string sAsFunctionCall = AsFunctionCall.DisplayValueSN();
	}

	void SNL_Parse::DefineSubscript()
	{
		{
			SN_LOCAL(AsSubscriptExt);
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			SN_LOCAL(m);
			SN_LOCAL(n);

			SN_LOCAL(w);

			SN_LOCAL(p);
			SN_LOCAL(q);

			(Define(AsSubscript)(d)(s)(i) == (
				Local(m, Local(n, Local(p, Let(s == m + n,
					AsPath(d)(m)(p)
					&& AsSubscriptExt(d)(n)(i)(p)
				))))
				)).PartialAssert().Do();

			(Define(AsSubscriptExt)(d)(s)(i)(p) == (
				(s.LookaheadLeft() == Char('[')).If(
					Local(m, Local(n, Local(q, Let(s == Char('[') + m + n,
						AsExpression(d)(m)(q)
					&&	n.LookaheadLeft() == Char(']')
					&&	AsSubscriptExt(d)(n.SubtractLeftChar())(i)(Meta(1, Function(Meta(-1, p), Meta(-1, q))))
					))))
				,	s == String("") && i == p
				)
			)).PartialAssert().Do();
			string sAsSubscriptExt = AsSubscriptExt.DisplayValueSN();
		}
		string sAsSubscript = AsSubscript.DisplayValueSN();
	}

	void SNL_Parse::DefinePath()
	{
		{
			SN_LOCAL(AsPathExt);
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			SN_LOCAL(m);
			SN_LOCAL(n);

			SN_LOCAL(p);
			SN_LOCAL(q);

			(Define(AsPath)(d)(s)(i) == (
				Local(m, Local(n, Local(p, Let(s == m + n,
					AsValueRef(d)(m)(p)
				&&	AsPathExt(d)(n)(i)(p)
				))))
				)).PartialAssert().Do();

			(Define(AsPathExt)(d)(s)(i)(p) == (
				(s.LookaheadLeft() == Char('.')).If(
					Local(m, Local(n, Local(q, Let(s == Char('.') + m + n,
						AsValueRef(d)(m)(i)(p)
					&&	AsPathExt)(d)(n)(i)(Meta(1, Meta(-1, p)[Meta(-1, q)]))
					)))
				, s == String("") && i == p
				)
				)).PartialAssert().Do();
			string sAsPathExt = AsPathExt.DisplayValueSN();
		}
		string sAsPath = AsPath.DisplayValueSN();
	}

	void SNL_Parse::DefineFactor()
	{
		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			(Define(AsFactor)(d)(s)(i) == (AsBracket(d)(s)(i) || AsName(d)(s)(i) || AsValue(s)(i))).PartialAssert().Do();
		}

		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			SN_LOCAL(m);

			(Define(AsBracket)(d)(s)(i) == (
				s.LookaheadLeft() == Char('(')
				&& Local(m, Let(s == Char('(') + m + Char(')'),
					AsExpression(d)(m)(i)
				))
				)).PartialAssert().Do();
		}

		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			(Define(AsName)(d)(s)(i) == (m_Validate.IsName(s) && (i == d.CreateMetaVariable(s)))).PartialAssert().Do();
		}
	}

	void SNL_Parse::DefineValue()
	{
		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			(Define(AsValue)(d)(s)(i) == (AsBoolean(s)(i) || AsNumber(s)(i) || AsString(s)(i))).PartialAssert().Do();
		}

		{
			SN_LOCAL(s);
			SN_LOCAL(i);

			(Define(AsBoolean)(s)(i) == (
				(s == String("true") && i == True)
			||	(s == String("false") && i == False)
			)).PartialAssert().Do();
		}

		{
			SN_LOCAL(s);
			SN_LOCAL(i);

			(Define(AsInteger)(s)(i) == (m_Validate.IsInteger(s) && (s.StringToInt() == i))).PartialAssert().Do();
		}

		{
			SN_LOCAL(s);
			SN_LOCAL(i);

			(Define(AsScientific)(s)(i) == (m_Validate.IsExponential(s) && (s.StringToInt() == i))).PartialAssert().Do();
		}

		{
			SN_LOCAL(s);
			SN_LOCAL(i);

			SN_LOCAL(p);
			SN_LOCAL(q);
			SN_LOCAL(m);
			SN_LOCAL(n);

			SN_LOCAL(Float_Extension);
			SN_LOCAL(Exp_Extension);

			(Define(AsNumber)(s)(i) == (
				Local(p, Local(q, Let(s == p + q
					, m_Validate.IsInteger(p)
					&& (Float_Extension(s)(i)(q)
						|| (q == String("") && s.StringToInt() == i))
				))))).PartialAssert().Do();

			(Define(Float_Extension)(s)(i)(q) ==
				Local(m, Local(n, Let(q == m + n
					, m_Validate.IsFloatingPointExtension(m)
					&& (Exp_Extension(s)(i)(n)
						|| (n == String("") && (s.StringToDouble() == i)))
				)))).PartialAssert().Do();

			(Define(Exp_Extension)(s)(i)(n) ==
				(m_Validate.IsExponentialExtension(n)
					&& (s.StringToDouble() == i)
					)).PartialAssert().Do();
		}

		{
			SN_LOCAL(s);
			SN_LOCAL(t);
			SN_LOCAL(i);

			(Define(AsString)(s)(i) == (m_Validate.IsString(s) && Local(t, Let(s == String("\"") + t + String("\""), t.Unescape(CPP) == i)))).PartialAssert().Do();
		}

		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);


			(Define(AsValueRef)(d)(s)(i) == (AsName(d)(s)(i) || AsNumber(s)(i) || AsString(s)(i))).PartialAssert().Do();
		}
	}
}
