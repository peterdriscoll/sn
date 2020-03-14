#include "snl_parse.h"

#include "snl_pch.h"

using namespace std;
using namespace skynet;
using namespace PGCX;

namespace SNL
{
	SNL_Parse::SNL_Parse(const SNL::SNL_Validate &p_Validate)
		: skynet::Domain("Parse")
		, m_Validate(p_Validate)
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
	}

	void SNL_Parse::Init()
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
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			(Define(AsName)(d)(s)(i) == (m_Validate.IsName(s) && (i == d.CreateMetaVariable(s)))).PartialAssert().Do();
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
}