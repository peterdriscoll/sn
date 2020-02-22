#include "snl_parser.h"

#include "snl_pch.h"

using namespace std;
using namespace skynet;
using namespace PGCX;

namespace SNL
{
	SNL_Parser::SNL_Parser(const SNL::SNL_Validate &p_Validate)
		: skynet::Domain("Parser")
		, m_Validate(p_Validate)
	{
		Setup();
	}

	void SNL_Parser::Setup()
	{
		Link();
		Init();
	}

	void SNL_Parser::Link()
	{
		SN_LINK(ParseInteger);
		SN_LINK(ParseName);
		SN_LINK(ParseString);
	}

	void SNL_Parser::Init()
	{
		{
			SN_LOCAL(s);
			SN_LOCAL(i);

			(Define(ParseInteger)(s)(i) == (m_Validate.IsInteger(s) && (s.StringToInt() == i))).PartialAssert().Do();
		}

		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);

			(Define(ParseName)(d)(s)(i) == (m_Validate.IsName(s) && (i == d.CreateMetaVariable(s)))).PartialAssert().Do();
		}

		{
			SN_LOCAL(s);
			SN_LOCAL(t);
			SN_LOCAL(i);

			(Define(ParseString)(s)(i) == (m_Validate.IsString(s) && Local(t, Let(s == String("\"") + t + String("\""), t.Unescape(CPP) == i)))).PartialAssert().Do();
		}

		{
			SN_LOCAL(d);
			SN_LOCAL(s);
			SN_LOCAL(i);


			(Define(ParseValue)(d)(s)(i) == (ParseName(d)(s)(i) || ParseInteger(s)(i) || ParseString(s)(i))).PartialAssert().Do();
		}
	}
}