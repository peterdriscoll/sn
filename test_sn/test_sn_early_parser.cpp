#include "snl.h"

#include <cstdio>
#include <fstream>

#include "test_sn_pch.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace std;
using namespace skynet;
using namespace PGCX;

namespace test_sn
{
	TEST_CLASS(test_sn_early_parser)
	{
	private:
		bool runWebServer = false;

		static void AssertErrorHandler(bool p_Err, const string& p_Description)
		{
			Assert::IsTrue(!p_Err, wstring(p_Description.begin(), p_Description.end()).c_str());
		}

		void Initialize()
		{
			Manager::LogicSetup();
			Transaction::ResetNetMemoryUsed();
			Transaction::ResetGrossMemoryUsed();

			Thread thread;
			if (thread.HasTopManager())
			{
				Assert::IsTrue(!thread.HasTopManager());
			}
		}

		void Cleanup()
		{
			Thread thread;
			if (thread.HasTopManager())
			{
				Assert::IsTrue(!thread.HasTopManager());
			}
			if (Transaction::TotalNetMemoryUsed() != 0)
			{
				Assert::IsTrue(Transaction::TotalNetMemoryUsed() == 0);
			}
			if (Promotion::PromotionUsedMemory() != 0)
			{
				Assert::IsTrue(Promotion::PromotionUsedMemory() == 0);
			}
			if (Promotion::PromotionFreeMemory() != Transaction::TotalGrossMemoryUsed())
			{
				Assert::IsTrue(Promotion::PromotionFreeMemory() == Transaction::TotalGrossMemoryUsed());
			}
		}

	public:
		TEST_METHOD(TestCharInValueSet)
		{
			Initialize();
			{
				Manager manager("Test Char In Value Set", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(B_Digit);
				SN_DECLARE(d);

				(Define(B_Digit)(d) == (d == (String("0") || String("1") || String("2") || String("3") || String("4")
					|| String("5") || String("6") || String("7") || String("8") || String("9")))).PartialAssert().Do();

				string B_Digit_text = B_Digit.DisplayValueSN();

				(B_Digit(String("0"))).Assert().Do();
				(B_Digit(String("1"))).Assert().Do();
				(B_Digit(String("2"))).Assert().Do();
				(B_Digit(String("3"))).Assert().Do();
				(B_Digit(String("4"))).Assert().Do();
				(B_Digit(String("5"))).Assert().Do();
				(B_Digit(String("6"))).Assert().Do();
				(B_Digit(String("7"))).Assert().Do();
				(B_Digit(String("8"))).Assert().Do();
				(B_Digit(String("9"))).Assert().Do();

				(!B_Digit(String("X"))).Assert().Do();

				// These return a value set of bools.  Should this reduce to a boolean value???

				(B_Digit(String("0"))).Evaluate().Do();
				(B_Digit(String("1"))).Evaluate().Do();
				(B_Digit(String("2"))).Evaluate().Do();
				(B_Digit(String("3"))).Evaluate().Do();
				(B_Digit(String("4"))).Evaluate().Do();
				(B_Digit(String("5"))).Evaluate().Do();
				(B_Digit(String("6"))).Evaluate().Do();
				(B_Digit(String("7"))).Evaluate().Do();
				(B_Digit(String("8"))).Evaluate().Do();
				(B_Digit(String("9"))).Evaluate().Do();

				(!B_Digit(String("X"))).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestCharInSet)
		{
			Initialize();
			{
				Manager manager("Test Char In Set", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(B_Digit2);
				SN_DECLARE(d);

				(Define(B_Digit2)(d) == ((String("0") || String("1") || String("2") || String("3") || String("4")
					|| String("5") || String("6") || String("7") || String("8") || String("9")).BuildSet().HasMember(d))).PartialAssert().Do();

				std::cout << std::endl << "Function " << B_Digit2.DisplaySN() << std::endl;
				B_Digit2(String("0")).Assert().Do();
				B_Digit2(String("1")).Assert().Do();
				B_Digit2(String("2")).Assert().Do();
				B_Digit2(String("3")).Assert().Do();
				B_Digit2(String("4")).Assert().Do();
				B_Digit2(String("5")).Assert().Do();
				B_Digit2(String("6")).Assert().Do();
				B_Digit2(String("7")).Assert().Do();
				B_Digit2(String("8")).Assert().Do();
				B_Digit2(String("9")).Assert().Do();

				(!(B_Digit2(String("X")))).Assert().Do();

				(B_Digit2(String("0"))).Evaluate().Do();
				(B_Digit2(String("1"))).Evaluate().Do();
				(B_Digit2(String("2"))).Evaluate().Do();
				(B_Digit2(String("3"))).Evaluate().Do();
				(B_Digit2(String("4"))).Evaluate().Do();
				(B_Digit2(String("5"))).Evaluate().Do();
				(B_Digit2(String("6"))).Evaluate().Do();
				(B_Digit2(String("7"))).Evaluate().Do();
				(B_Digit2(String("8"))).Evaluate().Do();
				(B_Digit2(String("9"))).Evaluate().Do();

				(!B_Digit2(String("X"))).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestIsInteger)
		{
			Initialize();
			{
				Manager manager("Test Is Integer", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(Digit);
				SN_DECLARE(d);
				SN_DECLARE(i);
				SN_DECLARE(j);

				(Define(Digit)(d) == (d == (String("0") || String("1") || String("2") || String("3") || String("4")
					|| String("5") || String("6") || String("7") || String("8") || String("9")))).PartialAssert().Do();

				SN_DECLARE(IsInteger);
				(Define(IsInteger)(i) == (Digit(i.SelectLeftChar()) && (!Digit(i.SubtractLeftChar().LookaheadLeft())).If(i == i.SelectLeftChar(), IsInteger(i.SubtractLeftChar())))).PartialAssert().Do();

				SN_DECLARE(ConvertInteger);
				(Define(ConvertInteger)(j) == Let(IsInteger(j), j.StringToInt())).PartialAssert().Do();

				(IsInteger(String("1")).Assert().Do());
				(IsInteger(String("12")).Assert().Do());
				(IsInteger(String("123456789")).Assert().Do());

				SN_DECLARE(x1);
				(ConvertInteger(String("1")) == x1).Assert().Do();
				(x1 == Long(1)).Evaluate().Do();

				SN_DECLARE(x2);
				(ConvertInteger(String("12")) == x2).Assert().Do();
				(x2 == Long(12)).Evaluate().Do();

				SN_DECLARE(x3);
				(ConvertInteger(String("123456789")) == x3).Assert().Do();
				(x3 == Long(123456789)).Evaluate().Do();

				SN_DECLARE(a);
				SN_DECLARE(b);
				(a + b == "1abc").Assert().Do();
				IsInteger(a).Assert().Do();
				string test_a = a.GetString();
				string test_b = b.GetString();
				Assert::IsTrue(test_a == "1");
				Assert::IsTrue(test_b == "abc");

				SN_DECLARE(c4);
				SN_DECLARE(d4);
				(c4 + d4 == "1abc").Assert().Do();
				SN_DECLARE(x4);
				(ConvertInteger(c4) == x4).Assert().Do();
				(x4 == Long(1)).Evaluate().Do();

				SN_DECLARE(a1);
				SN_DECLARE(b1);
				(a1 + b1 == "12abc").Assert().Do();
				IsInteger(a1).Assert().Do();
				string test_a1 = a1.GetString();
				string test_b1 = b1.GetString();
				Assert::IsTrue(test_a1 == "12");
				Assert::IsTrue(test_b1 == "abc");

				SN_DECLARE(c5);
				SN_DECLARE(d5);
				(c5 + d5 == "12abc").Assert().Do();
				SN_DECLARE(x5);
				(ConvertInteger(c5) == x5).Assert().Do();
				(x5 == Long(12)).Evaluate().Do();

				SN_DECLARE(a2);
				SN_DECLARE(b2);
				(a2 + b2 == "123456789abc").Assert().Do();
				IsInteger(a2).Assert().Do();
				string test_a2 = a2.GetString();
				string test_b2 = b2.GetString();
				Assert::IsTrue(test_a2 == "123456789");
				Assert::IsTrue(test_b2 == "abc");

				SN_DECLARE(c6);
				SN_DECLARE(d6);
				(c6 + d6 == "123456789abc").Assert().Do();
				SN_DECLARE(x6);
				(ConvertInteger(c6) == x6).Assert().Do();
				(x6 == Long(123456789)).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestParseInteger)
		{
			Initialize();
			{
				Manager manager("Test Parse Integer", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(Digit);
				SN_DECLARE(d);
				(Define(Digit)(d) == (d == (String("0") || String("1") || String("2") || String("3") || String("4")
					|| String("5") || String("6") || String("7") || String("8") || String("9")))).PartialAssert().Do();

				SN_DECLARE(IsInteger);
				SN_DECLARE(i);
				(Define(IsInteger)(i) == (Digit(i.SelectLeftChar()) && !(Digit(i.SubtractLeftChar().LookaheadLeft()))).If(i == i.SelectLeftChar(), IsInteger(i.SubtractLeftChar()))).PartialAssert().Do();

				SN_DECLARE(ParseInteger);
				SN_DECLARE(s);
				(Define(ParseInteger)(s)(i) == Let(IsInteger(s), s.StringToInt() == i)).PartialAssert().Do();

				SN_DECLARE(ParseInteger2);
				(Define(ParseInteger2)(s)(i) == (IsInteger(s) && (s.StringToInt() == i))).PartialAssert().Do();

				SN_DECLARE(x2);
				(ParseInteger("13")(x2)).Assert().Do();
				(x2 == Long(13)).Evaluate().Do();

				SN_DECLARE(x3);
				(ParseInteger("21")(x3)).Assert().Do();
				(x3 == Long(21)).Evaluate().Do();

				long x2_long = Long(x2.GetSafeValue()).GetNumber();
				long x3_long = Long(x3.GetSafeValue()).GetNumber();
				Assert::IsTrue(x2_long == 13);
				Assert::IsTrue(x3_long == 21);
			}
		}

		TEST_METHOD(TestParseName)
		{
			Initialize();
			{
				Manager manager("Test Parse Name", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(Digit);
				SN_DECLARE(d);
				(Define(Digit)(d) == (d == (String("0") || String("1") || String("2") || String("3") || String("4")
					|| String("5") || String("6") || String("7") || String("8") || String("9")))).PartialAssert().Do();

				SN_DECLARE(AlphaLower);
				SN_DECLARE(l);
				(Define(AlphaLower)(l) == (l == (String("a") || String("b") || String("c") || String("d") || String("e")
					|| String("f") || String("g") || String("h") || String("i") || String("j") || String("k") || String("l")
					|| String("m") || String("n") || String("o") || String("p") || String("q") || String("r") || String("s")
					|| String("t") || String("u") || String("v") || String("w") || String("x") || String("y") || String("z")))).PartialAssert().Do();

				SN_DECLARE(AlphaUpper);
				SN_DECLARE(u);
				(Define(AlphaUpper)(u) == (u == (String("A") || String("B") || String("C") || String("D") || String("E")
					|| String("F") || String("G") || String("H") || String("I") || String("J") || String("K") || String("L")
					|| String("M") || String("N") || String("O") || String("P") || String("Q") || String("R") || String("S")
					|| String("T") || String("U") || String("V") || String("W") || String("X") || String("Y") || String("Z")))).PartialAssert().Do();

				SN_DECLARE(Alpha);
				SN_DECLARE(a);
				(Define(Alpha)(a) == (AlphaLower(a) || AlphaUpper(a))).PartialAssert().Do();

				SN_DECLARE(AlphaNumeric);
				SN_DECLARE(k);
				(Define(AlphaNumeric)(k) == (Alpha(k) || Digit(k))).PartialAssert().Do();

				SN_DECLARE(a1);
				(Alpha(String("g")) == a1).Assert().Do();
				a1.Evaluate().Do();

				SN_DECLARE(a2);
				(Alpha(String("6")) == a2).Assert().Do();
				(!a2).Evaluate().Do();

				SN_DECLARE(a3);
				(AlphaNumeric(String("6")) == a3).Assert().Do();
				a3.Evaluate().Do();

				SN_DECLARE(IsInteger);
				SN_DECLARE(i);
				(Define(IsInteger)(i) == (Digit(i.SelectLeftChar()) && (!Digit(i.SubtractLeftChar().LookaheadLeft())).If(i == i.SelectLeftChar(), IsInteger(i.SubtractLeftChar())))).PartialAssert().Do();

				SN_DECLARE(z1);
				(IsInteger(String("456666")) == z1).Assert().Do();
				(z1).Evaluate().Do();

				SN_DECLARE(IsName);
				SN_DECLARE(IsNameContinuation);
				SN_DECLARE(n);

				(Define(IsName)(i) == (Alpha(i.SelectLeftChar()) && (!Alpha(i.SubtractLeftChar().LookaheadLeft())).If(i == i.SelectLeftChar(), IsNameContinuation(i.SubtractLeftChar())))).PartialAssert().Do();
				(Define(IsNameContinuation)(i) == (AlphaNumeric(i.SelectLeftChar()) && (!AlphaNumeric(i.SubtractLeftChar().LookaheadLeft())).If(i == i.SelectLeftChar(), IsNameContinuation(i.SubtractLeftChar())))).PartialAssert().Do();

				SN_DECLARE(z2);
				(IsName(String("Peter1")) == z2).Assert().Do();
				(z2).Evaluate().Do();

				SN_DECLARE(ParseInteger);
				SN_DECLARE(s);
				(Define(ParseInteger)(s)(i) == (IsInteger(s) && (s.StringToInt() == i))).PartialAssert().Do();

				SN_DECLARE(ParseName);
				(Define(ParseName)(d)(s)(i) == (IsName(s) && (i == d.CreateMetaVariable(s)))).PartialAssert().Do();

				SN_DECLARE(x3);
				(ParseInteger("21")(x3)).Assert().Do();
				(x3 == Long(21)).Evaluate().Do();

				SN_DOMAIN(MyDomain);
				SN_DECLARE(i3);
				(ParseName(MyDomain)(String("Peter1"))(i3)).Assert().Do();

				SN_DECLARE(i4);
				(i4 == Meta(1, MyDomain["Peter1"])).Assert().Do();
				(i3 == i4).Evaluate().Do();

				string i3_string = i3.DisplayValueSN();
				string i4_string = i4.DisplayValueSN();
				Assert::IsTrue(i3_string == i4_string);
			}
		}

		TEST_METHOD(TestParseString)
		{
			Initialize();
			{
				Manager manager("Test Parse String", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(IsString);
				SN_DECLARE(IsStringContent);
				SN_DECLARE(s);
				SN_DECLARE(t);
				SN_DECLARE(u);
				SN_DECLARE(v);

				//	IsString.
				//	The Let expression is designed  to never fail, but the expression
				//	cannot quite deduce that the input is definitely a string. If
				//	it is asserted then it works but if you assert the result equal
				//  to a variable, it thinks it could be true or false. This is not an
				//	error, it is logically correct.
				//	Collapse converts [true] or [true, false], to true. It returns false
				//	otherwise.

				(Define(IsString)(s) == (((s.SelectLeftChar() == String("\"")) && Local(t, Local(u, Let(s.SubtractLeftChar() == t + u, IsStringContent(t) && (u == String("\""))))))).Collapse()).PartialAssert().Do();

				(Define(IsStringContent)(s) == ((s.LookaheadLeft() == String("\"")).If(s == String(""), (s.LookaheadLeft() == String("\\")).If(IsStringContent(s.SubtractLeftChar().SubtractLeftChar()), IsStringContent(s.SubtractLeftChar()))))).PartialAssert().Do();

				IsString(String("\"My test string\"")).Assert().Do();

				SN_DECLARE(z1);
				(IsString(String("\"M\"")) == z1).Assert().Do();
				(z1).Evaluate().Do();

				SN_DECLARE(z2);
				(IsString(String("\"Line 1\\nLine2\"")) == z2).Assert().Do();
				(z2).Evaluate().Do();

				SN_DECLARE(ParseString);
				SN_DECLARE(i);
				(Define(ParseString)(s)(i) == (IsString(s) && Local(t, Let(s == String("\"") + t + String("\""), t.Unescape(CPP) == i)))).PartialAssert().Do();

				SN_DECLARE(x2);
				(ParseString("\"Line 1\\nLine2\"")(x2)).Assert().Do();
				(x2 == String("Line 1\nLine2")).Evaluate().Do();

				SN_DECLARE(y2);
				(y2 == String("Line 1\nLine2")).Assert().Do();

				string x2_string = x2.DisplayValueSN();
				string y2_string = y2.DisplayValueSN();
				Assert::IsTrue(x2_string == y2_string);

				SN_DECLARE(k1);
				(IsString(String("Bad string")) == k1).Assert().Do();
				(!k1).Evaluate().Do();

				SN_DECLARE(k2);
				SN_DECLARE(k3);
				(ParseString(String("Bad string"))(k2) == k3).Assert().Do();
				(!k3).Evaluate().Do();
			}
		}

		TEST_METHOD(TestParsePart)
		{
			Initialize();
			{
				Manager manager("Test Parse Part", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(Digit);
				SN_DECLARE(d);
				(Define(Digit)(d) == (d == (String("0") || String("1") || String("2") || String("3") || String("4")
					|| String("5") || String("6") || String("7") || String("8") || String("9")))).PartialAssert().Do();

				SN_DECLARE(AlphaLower);
				SN_DECLARE(l);
				(Define(AlphaLower)(l) == (l == (String("a") || String("b") || String("c") || String("d") || String("e")
					|| String("f") || String("g") || String("h") || String("i") || String("j") || String("k") || String("l")
					|| String("m") || String("n") || String("o") || String("p") || String("q") || String("r") || String("s")
					|| String("t") || String("u") || String("v") || String("w") || String("x") || String("y") || String("z")))).PartialAssert().Do();

				SN_DECLARE(AlphaUpper);
				SN_DECLARE(u);
				(Define(AlphaUpper)(u) == (u == (String("A") || String("B") || String("C") || String("D") || String("E")
					|| String("F") || String("G") || String("H") || String("I") || String("J") || String("K") || String("L")
					|| String("M") || String("N") || String("O") || String("P") || String("Q") || String("R") || String("S")
					|| String("T") || String("U") || String("V") || String("W") || String("X") || String("Y") || String("Z")))).PartialAssert().Do();

				SN_DECLARE(Alpha);
				SN_DECLARE(a);
				(Define(Alpha)(a) == (AlphaLower(a) || AlphaUpper(a) || a == string("_"))).PartialAssert().Do();

				SN_DECLARE(AlphaNumeric);
				SN_DECLARE(k);
				(Define(AlphaNumeric)(k) == (Alpha(k) || Digit(k))).PartialAssert().Do();

				SN_DECLARE(IsInteger);
				SN_DECLARE(i);
				(Define(IsInteger)(i) == (Digit(i.SelectLeftChar()) && (!Digit(i.SubtractLeftChar().LookaheadLeft())).If(i == i.SelectLeftChar(), IsInteger(i.SubtractLeftChar())))).PartialAssert().Do();

				SN_DECLARE(IsName);
				SN_DECLARE(IsNameContinuation);
				SN_DECLARE(n);

				(Define(IsName)(i) == (Alpha(i.SelectLeftChar()) && (!AlphaNumeric(i.SubtractLeftChar().LookaheadLeft())).If(i == i.SelectLeftChar(), Alpha(i.SelectLeftChar()) && IsNameContinuation(i.SubtractLeftChar())))).PartialAssert().Do();
				(Define(IsNameContinuation)(i) == (AlphaNumeric(i.SelectLeftChar()) && (!AlphaNumeric(i.SubtractLeftChar().LookaheadLeft())).If(i == i.SelectLeftChar(), AlphaNumeric(i.SelectLeftChar()) && IsNameContinuation(i.SubtractLeftChar())))).PartialAssert().Do();

				SN_DECLARE(ParseInteger);
				SN_DECLARE(s);
				(Define(ParseInteger)(s)(i) == (IsInteger(s) && (s.StringToInt() == i))).PartialAssert().Do();

				SN_DECLARE(ParseName);
				(Define(ParseName)(d)(s)(i) == (IsName(s) && (i == d.CreateMetaVariable(s)))).PartialAssert().Do();

				SN_DECLARE(ParsePart);
				(Define(ParsePart)(d)(s)(i) == (ParseName(d)(s)(i) || ParseInteger(s)(i))).PartialAssert().Do();

				SN_DOMAIN(MyDomain);
				SN_DECLARE(i1);
				(ParsePart(MyDomain)(String("Peter_1"))(i1)).Assert().Do();

				SN_DECLARE(j1);
				(j1 == Meta(1, MyDomain["Peter_1"])).Assert().Do();
				(i1 == j1).Evaluate().Do();

				string i1_string = i1.DisplayValueSN();
				string j1_string = j1.DisplayValueSN();
				Assert::IsTrue(i1_string == j1_string);

				SN_DECLARE(i2);
				(ParsePart(MyDomain)(String("543"))(i2)).Assert().Do();

				SN_DECLARE(j2);
				(j2 == Long(543)).Assert().Do();
				(i2 == j2).Evaluate().Do();

				string i2_string = i2.DisplayValueSN();
				string j2_string = j2.DisplayValueSN();
				Assert::IsTrue(i2_string == j2_string);
			}
		}

		TEST_METHOD(TestParsePart2)
		{
			Initialize();
			{
				Manager manager("Test Parse Part2", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(Digit);
				SN_DECLARE(d);
				(Define(Digit)(d) == (d == (String("0") || String("1") || String("2") || String("3") || String("4")
					|| String("5") || String("6") || String("7") || String("8") || String("9")))).PartialAssert().Do();

				SN_DECLARE(AlphaLower);
				SN_DECLARE(l);
				(Define(AlphaLower)(l) == (l == (String("a") || String("b") || String("c") || String("d") || String("e")
					|| String("f") || String("g") || String("h") || String("i") || String("j") || String("k") || String("l")
					|| String("m") || String("n") || String("o") || String("p") || String("q") || String("r") || String("s")
					|| String("t") || String("u") || String("v") || String("w") || String("x") || String("y") || String("z")))).PartialAssert().Do();

				SN_DECLARE(AlphaUpper);
				SN_DECLARE(u);
				(Define(AlphaUpper)(u) == (u == (String("A") || String("B") || String("C") || String("D") || String("E")
					|| String("F") || String("G") || String("H") || String("I") || String("J") || String("K") || String("L")
					|| String("M") || String("N") || String("O") || String("P") || String("Q") || String("R") || String("S")
					|| String("T") || String("U") || String("V") || String("W") || String("X") || String("Y") || String("Z")))).PartialAssert().Do();

				SN_DECLARE(Alpha);
				SN_DECLARE(a);
				(Define(Alpha)(a) == (AlphaLower(a) || AlphaUpper(a) || a == string("_"))).PartialAssert().Do();

				SN_DECLARE(AlphaNumeric);
				SN_DECLARE(k);
				(Define(AlphaNumeric)(k) == (Alpha(k) || Digit(k))).PartialAssert().Do();

				SN_DECLARE(IsInteger);
				SN_DECLARE(i);
				(Define(IsInteger)(i) == (Digit(i.SelectLeftChar()) && (!Digit(i.SubtractLeftChar().LookaheadLeft())).If(i == i.SelectLeftChar(), IsInteger(i.SubtractLeftChar())))).PartialAssert().Do();

				SN_DECLARE(IsName);
				SN_DECLARE(IsNameContinuation);
				SN_DECLARE(n);

				(Define(IsName)(i) == (Alpha(i.SelectLeftChar()) && (!AlphaNumeric(i.SubtractLeftChar().LookaheadLeft())).If(i == i.SelectLeftChar(), Alpha(i.SelectLeftChar()) && IsNameContinuation(i.SubtractLeftChar())))).PartialAssert().Do();
				(Define(IsNameContinuation)(i) == (AlphaNumeric(i.SelectLeftChar()) && (!AlphaNumeric(i.SubtractLeftChar().LookaheadLeft())).If(i == i.SelectLeftChar(), AlphaNumeric(i.SelectLeftChar()) && IsNameContinuation(i.SubtractLeftChar())))).PartialAssert().Do();

				SN_DECLARE(IsString);
				SN_DECLARE(IsStringContent);
				SN_DECLARE(s);
				SN_DECLARE(t);
				SN_DECLARE(v);

				//	IsString.
				//	The Let expression is designed  to never fail, but the expression
				//	cannot quite deduce that the input is definitely a string. If
				//	it is asserted then it works but if you assert the result equal
				//  to a variable, it thinks it could be true or false. This is not an
				//	error, it is logically correct.
				//	Collapse converts [true] or [true, false], to true. It returns false
				//	otherwise.

				(Define(IsString)(s) == (((s.SelectLeftChar() == String("\"")) && Local(t, Local(u, Let(s.SubtractLeftChar() == t + u, IsStringContent(t) && (u == String("\""))))))).Collapse()).PartialAssert().Do();

				(Define(IsStringContent)(s) == ((s.LookaheadLeft() == String("\"")).If(s == String(""), (s.LookaheadLeft() == String("\\")).If(IsStringContent(s.SubtractLeftChar().SubtractLeftChar()), IsStringContent(s.SubtractLeftChar()))))).PartialAssert().Do();


				SN_DECLARE(ParseInteger);
				(Define(ParseInteger)(s)(i) == (IsInteger(s) && (s.StringToInt() == i))).PartialAssert().Do();

				SN_DECLARE(ParseName);
				(Define(ParseName)(d)(s)(i) == (IsName(s) && (i == d.CreateMetaVariable(s)))).PartialAssert().Do();

				SN_DECLARE(ParseString);
				(Define(ParseString)(s)(i) == (IsString(s) && Local(t, Let(s == String("\"") + t + String("\""), t.Unescape(CPP) == i)))).PartialAssert().Do();

				SN_DECLARE(ParsePart);
				(Define(ParsePart)(d)(s)(i) == (ParseName(d)(s)(i) || ParseInteger(s)(i) || ParseString(s)(i))).PartialAssert().Do();

				SN_DOMAIN(MyDomain);
				SN_DECLARE(i1);
				(ParsePart(MyDomain)(String("Peter_1"))(i1)).Assert().Do();

				SN_DECLARE(j1);
				(j1 == Meta(1, MyDomain["Peter_1"])).Assert().Do();
				(i1 == j1).Evaluate().Do();

				string i1_string = i1.DisplayValueSN();
				string j1_string = j1.DisplayValueSN();
				Assert::IsTrue(i1_string == j1_string);

				SN_DECLARE(i2);
				(ParsePart(MyDomain)(String("543"))(i2)).Assert().Do();

				SN_DECLARE(j2);
				(j2 == Long(543)).Assert().Do();
				(i2 == j2).Evaluate().Do();

				string i2_string = i2.DisplayValueSN();
				string j2_string = j2.DisplayValueSN();
				Assert::IsTrue(i2_string == j2_string);

				SN_DECLARE(i3);
				(ParsePart(MyDomain)(String("\"My test string\""))(i3)).Assert().Do();

				SN_DECLARE(j3);
				(j3 == String("My test string")).Assert().Do();
				(i3 == j3).Evaluate().Do();

				string i3_string = i3.DisplayValueSN();
				string j3_string = j3.DisplayValueSN();
				Assert::IsTrue(i3_string == j3_string);

				SN_DECLARE(s1);
				(ParsePart(MyDomain)(String("\"My test \\\"string\\\"\""))(s1)).Assert().Do();

				SN_DECLARE(s2);
				(s2 == String("My test \"string\"")).Assert().Do();
				(s1 == s2).Evaluate().Do();

				string s1_string = s1.DisplayValueSN();
				string s2_string = s2.DisplayValueSN();
				Assert::IsTrue(s1_string == s2_string);
			}
		}
	};
}