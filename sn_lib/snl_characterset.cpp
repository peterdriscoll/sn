#include "SNL_CharacterSet.h"

#include "snl_pch.h"

using namespace std;
using namespace skynet;
using namespace PGCX;

namespace SNL
{
	SNL_CharacterSet::SNL_CharacterSet()
		: skynet::Domain("CharacterSets")
	{
		Setup();
	}

	void SNL_CharacterSet::Setup()
	{
		Link();
		Init();
	}
	void SNL_CharacterSet::Link()
	{
		SN_LINK(Sign);
		SN_LINK(Digit);
		SN_LINK(AlphaLower);
		SN_LINK(AlphaUpper);
		SN_LINK(Alpha);
		SN_LINK(AlphaUnder);
		SN_LINK(AlphaNumeric);
		SN_LINK(AlphaUnderNumeric);
		SN_LINK(White);
	}

	void SNL_CharacterSet::Init()
	{
		// Character sets.

		{
			SN_LOCAL(c);

			(Define(Sign)(c) == (c == (String("+") || String("-")))).PartialAssert().Do();
		}

		{
			SN_LOCAL(d);

			(Define(Digit)(d) == (d == (String("0") || String("1") || String("2") || String("3") || String("4")
				|| String("5") || String("6") || String("7") || String("8") || String("9")))).PartialAssert().Do();
		}

		{
			SN_LOCAL(l);
			(Define(AlphaLower)(l) == (l == (String("a") || String("b") || String("c") || String("d") || String("e")
				|| String("f") || String("g") || String("h") || String("i") || String("j") || String("k") || String("l")
				|| String("m") || String("n") || String("o") || String("p") || String("q") || String("r") || String("s")
				|| String("t") || String("u") || String("v") || String("w") || String("x") || String("y") || String("z")))).PartialAssert().Do();
		}

		{
			SN_LOCAL(u);
			(Define(AlphaUpper)(u) == (u == (String("A") || String("B") || String("C") || String("D") || String("E")
				|| String("F") || String("G") || String("H") || String("I") || String("J") || String("K") || String("L")
				|| String("M") || String("N") || String("O") || String("P") || String("Q") || String("R") || String("S")
				|| String("T") || String("U") || String("V") || String("W") || String("X") || String("Y") || String("Z")))).PartialAssert().Do();
		}

		{
			SN_LOCAL(a);
			(Define(Alpha)(a) == (AlphaLower(a) || AlphaUpper(a))).PartialAssert().Do();
		}

		{
			SN_LOCAL(a);
			(Define(AlphaUnder)(a) == (AlphaLower(a) || AlphaUpper(a) || a == string("_"))).PartialAssert().Do();
		}

		{
			SN_LOCAL(k);
			(Define(AlphaNumeric)(k) == (Alpha(k) || Digit(k))).PartialAssert().Do();
		}

		{
			SN_LOCAL(k);
			(Define(AlphaUnderNumeric)(k) == (AlphaUnder(k) || Digit(k))).PartialAssert().Do();
		}

		{
			SN_LOCAL(w);
			(Define(White)(w) == (w == (String(" ") || String("\t") || String("\n") || String("\r")))).PartialAssert().Do();
		}
	}
}