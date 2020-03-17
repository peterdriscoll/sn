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
		SN_LINK(AsInteger);
		SN_LINK(AsScientific);
		SN_LINK(AsNumber);

		SN_LINK(AsName);
		SN_LINK(AsString);
		SN_LINK(AsValue);

		SN_LINK(AsPath);
		SN_LINK(AsCall);
		SN_LINK(AsLambda);
	}

	void SNL_Parse::Init()
	{
		DefineValue();
		DefineReference();
	}

	void SNL_Parse::DefineValue()
	{
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
					,	m_Validate.IsInteger(p)
					&&	(	Float_Extension(s)(i)(q)
						||	(q == String("") && s.StringToInt() == i))
				))))).PartialAssert().Do();

			(Define(Float_Extension)(s)(i)(q) ==
				Local(m, Local(n, Let(q == m + n
					,	m_Validate.IsFloatingPointExtension(m)
					&&  (	Exp_Extension(s)(i)(n)
						||	(n == String("") && (s.StringToDouble() == i)))
				)))).PartialAssert().Do();

			(Define(Exp_Extension)(s)(i)(n) ==
					(m_Validate.IsExponentialExtension(n)
				&&	(s.StringToDouble() == i)
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


			(Define(AsValue)(d)(s)(i) == (AsName(d)(s)(i) || AsNumber(s)(i) || AsString(s)(i))).PartialAssert().Do();
		}
	}

	void SNL_Parse::DefineReference()
	{
		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			(Define(AsName)(d)(s)(i) == (m_Validate.IsName(s) && (i == d.CreateMetaVariable(s)))).PartialAssert().Do();
		}

		{
			SN_LOCAL(PathExtension);

			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			SN_LOCAL(m);
			SN_LOCAL(n);

			SN_LOCAL(p);

			(Define(AsPath)(d)(s)(i) == (
				Local(m, Local(n, Let(s == m + n,
					AsName(d)(m)(p)
				&&	((n.LookaheadLeft() == String(".") && AsPath(p)(n.SubtractLeftChar())(i))
					|| (n == String("") && p == i))
				)))	
			)).PartialAssert().Do();
		}
	}

	void SNL_Parse::DefineLambdaCalculus()
	{
		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			SN_LOCAL(m);
			SN_LOCAL(n);

			SN_LOCAL(p);
			SN_LOCAL(q);

			SN_LOCAL(w1);
			SN_LOCAL(w2);

			(Define(AsLambda)(d)(s)(i) == (
				Local(m, Local(n, Local(p, Local(q, Let(s == w1 + String("\\") + m + String(".") + w2 + n,
					m_Validate.IsWhiteSpaceContinuation(w1)
				&&	AsName(d)(m)(p)
				&&	m_Validate.IsWhiteSpaceContinuation(w2)
				&&	AsExpression(d)(n)(q)
				&&	i == Meta(1, Lambda(Meta(-1, p), Meta(-1, q)).Notes(w1, w2)
				))))))
			)).PartialAssert().Do();
		}

		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);


			(Define(AsLambdaPart)(d)(s)(i) ==
				AsPath(d)(s)(i)
			||	AsLambda(d)(s)(i)
			).PartialAssert().Do();
		}

		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			SN_LOCAL(m);
			SN_LOCAL(n);

			SN_LOCAL(w);

			SN_LOCAL(p);
			SN_LOCAL(q);

			(Define(AsCall)(d)(s)(i) == (
				Local(m, Local(n, Local(p, Local(q, Let(s == m + w + n,
					AsLambdaPart(d)(m)(p)
				&&	m_Validate.IsWhiteSpace(w)
				&&	AsLambdaTerm(d)(n)(q)
				&&	i == skynet::Meta(1, Function(skynet::Meta(-1, p), skynet::Meta(-1, q)))
				)))))
			)).PartialAssert().Do();
		}

		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);


			(Define(AsLambdaTerm)(d)(s)(i) ==
				AsPath(d)(s)(i)
			||	AsLambdaTerm(d)(s)(i)
			).PartialAssert().Do();
		}
	}

	void SNL_Parse::DefineExpression()
	{
		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			SN_LOCAL(m);
			SN_LOCAL(n);

			SN_LOCAL(p);
			SN_LOCAL(q);

			(Define(AsExpression)(d)(s)(i) ==
				Local(m, Local(n, Local(p, Local(q, Let(s == m + n,
					AsTerm(d)(s)(p)
				&&	(	n.LookaheadLeft() == m_CharacterSet.Sign && AsExpression(d)(s)(i)
					||	(n == String("") && i == p))
				)))))
			).PartialAssert().Do();
		}

		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			SN_LOCAL(m);
			SN_LOCAL(n);

			SN_LOCAL(p);
			SN_LOCAL(q);

			(Define(AsTerm)(d)(s)(i) ==
				Local(m, Local(n, Local(p, Local(q, Let(s == m + n,
					AsFactor(d)(s)(p)
					&& (n.LookaheadLeft() == m_CharacterSet.TimesDivide && AsTerm(d)(s)(i)
						|| (n == String("") && i == p))
				)))))
			).PartialAssert().Do();
		}
	}
}

