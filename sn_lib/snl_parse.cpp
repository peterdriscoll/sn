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
		SN_LINK(Integer);
		SN_LINK(Scientific);
		SN_LINK(Number);

		SN_LINK(Name);
		SN_LINK(String);
		SN_LINK(Value);
	}

	void SNL_Parse::Init()
	{
		{
			SN_LOCAL(s);
			SN_LOCAL(i);

			(Define(Integer)(s)(i) == (m_Validate.IsInteger(s) && (s.StringToInt() == i))).PartialAssert().Do();
		}

		{
			SN_LOCAL(s);
			SN_LOCAL(i);

			(Define(Scientific)(s)(i) == (m_Validate.IsExponential(s) && (s.StringToInt() == i))).PartialAssert().Do();
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

			(Define(Number)(s)(i) == (
				Let(s = p + q
					,	m_Validate.IsInteger(p)
					&&	(	Float_Extension(s)(i)(q)
						||	(q == String("") && (s.StringToDouble() == i)))
				)));

			(Define(Float_Extension)(s)(i)(q) ==
				Let(q == m + n
					,	m_Validate.IsFloatingPointExtension(m)
					&&  (	Exp_Extension(s)(i)(n)
						||	(n == String("") && (s.StringToDouble() == i))
				)));

			(Define(Exp_Extension)(s)(i)(n) ==
					(m_Validate.IsExponentialExtension(n)
				&&	(s.StringToDouble() == i)
					));
		}

		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			(Define(Name)(d)(s)(i) == (m_Validate.IsName(s) && (i == d.CreateMetaVariable(s)))).PartialAssert().Do();
		}

		{
			SN_LOCAL(s);
			SN_LOCAL(t);
			SN_LOCAL(i);

			(Define(String)(s)(i) == (m_Validate.IsString(s) && Local(t, Let(s == String("\"") + t + String("\""), t.Unescape(CPP) == i)))).PartialAssert().Do();
		}

		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);


			(Define(Value)(d)(s)(i) == (Name(d)(s)(i) || Number(s)(i) || String(s)(i))).PartialAssert().Do();
		}
	}
}