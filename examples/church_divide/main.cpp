#include <iostream>
#include <string>
#include <conio.h>

#include "sn.h"
using namespace PGCX;
using namespace skynet; // Interface namespace for SN.

string doc_root = "C:/Users/peter_driscoll/Documents/Source/Repos/skynet2/html";

void TestIsInteger()
{
	SN_DECLARE(Digit);
	SN_DECLARE(d);
	SN_DECLARE(i);

	(Define(Digit)(d) == (d == (String("0") || String("1") || String("2") || String("3") || String("4")
		|| String("5") || String("6") || String("7") || String("8") || String("9")))).PartialAssert().Do();
	Digit.LogDisplaySN();

	SN_DECLARE(IsInteger);
	(Define(IsInteger)(i) == (Digit(i) && !Digit(i.LookaheadRight())).Collapse().If(Bool(true), Digit(i.SelectLeftChar()) && IsInteger(i.SubtractLeftChar()))).PartialAssert().Do();
	IsInteger.LogDisplaySN();

	//(IsInteger(String("1")).Assert().Do());
	(IsInteger(String("12")).Assert().Do());
	(IsInteger(String("123456789")).Assert().Do());

	SN_DECLARE(a);
	SN_DECLARE(b);
	(a + b == "1abc").Assert().Do();
	IsInteger(a).Assert().Do();
	string test_a = a.GetString();
	string test_b = b.GetString();

	SN_DECLARE(a1);
	SN_DECLARE(b1);
	(a1 + b1 == "12abc").Assert().Do();
	IsInteger(a1).Assert().Do();
	string test_a1 = a1.GetString();
	string test_b1 = b1.GetString();

	SN_DECLARE(a2);
	SN_DECLARE(b2);
	(a2 + b2 == "123456789abc").Assert().Do();
	//IsInteger(a2).Assert().Do();
	string test_a2 = a2.GetString();
	string test_b2 = b2.GetString();

	SN_DECLARE(PmGenInteger);
	SN_DECLARE(s);
	//(Define(PmGenInteger)(d) == IsInteger(d) && .If(IsInteger(d), d.IntToString())).PartialAssert().Do();
	//(Define(PmGenInteger)(s.StringToInt())) == IsInteger(s).If(IsInteger(d), d)).PartialAssert().Do();
	//(Define(PmGenInteger)(d) == ((d = Long(s)) && IsInteger(s)).If(d)).PartialAssert().Do();
	//(Define(PmGenInteger)(d) == ((s == d.IntToString()) && IsInteger(s)).If(s)).PartialAssert().Do();
}

void TestCharInValueSet()
{
	Transaction transaction;

	SN_DECLARE(B_Digit);
	SN_DECLARE(d);

	(Define(B_Digit)(d) == (d == (String("0") || String("1") || String("2") || String("3") || String("4")
		|| String("5") || String("6") || String("7") || String("8") || String("9")))).PartialAssert().Do();

/*
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
*/
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


void TestValueSetOfStandardFunctions()
{
	Transaction transaction;

	SN_DECLARE(f);
	(f == (skynet::Add || skynet::Multiply)).Assert().Do();
	string f_string = f.DisplaySN();

	SN_DECLARE(x);
	(x == f(long(8))(long(13))).Assert().Do();
	string x_string = x.BuildSet().DoEvaluate().DisplaySN();
	(x < long(30)).Assert().Do();
	string x_string2 = x.DisplaySN();
	long x_num = Long(x).GetNumber();
	(x == Long(21)).Assert().Do();
}


void TestValueSetOfLambdaFunctions()
{
	Transaction transaction;

	SN_DECLARE(plus);
	SN_DECLARE(times);

	{
		SN_DECLARE(a);
		SN_DECLARE(b);

		(Define(plus)(a)(b) == a + b).PartialAssert().Do();

		SN_DECLARE(c);
		SN_DECLARE(d);

		(Define(times)(c)(d) == c * d).PartialAssert().Do();
	}
	string s_plus = plus.DisplayValueSN();
	string s_times = times.DisplayValueSN();
	string s_plus_times = (plus || times).DoEvaluate().DisplaySN();
	string s_plus_times2 = (plus || times).DoEvaluate().DisplaySN();

	SN_DECLARE(f);
	(f == (plus || times)).Assert().Do();
	string f_string = f.DisplaySN();

	SN_DECLARE(x);
	(x == f(long(5))(long(8))).Assert().Do();
	string x_string = x.DisplaySN();
	(x < long(20)).Assert().Do();
	(x == long(13)).Assert().Do();
}

void TestValueSetOfStringFunctions()
{
	Transaction transaction;

	SN_DECLARE(f);

	SN_DECLARE(wrap);
	SN_DECLARE(map);
	{
		SN_DECLARE(a);

		(Define(wrap)(a) == String("{") + (f(a) + String("}"))).PartialAssert().Do();

		SN_DECLARE(c);
		SN_DECLARE(d);

		(Define(map)(a) == a).PartialAssert().Do();
	}
	string f_before_string = f.DisplaySN();

	(Define(f) == (wrap || map)).PartialAssert().Do();
	string f_string = f.DisplaySN();

	SN_DECLARE(x);
	(String("{{expression}}") == f(x)).Assert().Do();
	string x_display = x.DisplaySN();
	string x_string = x.BuildSet().DoEvaluate().DisplaySN();
	string x_compare_string = "{String(\"expression\"), String(\"{expression}\"), String(\"{{expression}}\")}";

	ASSERTM(x_string == x_compare_string, "compare strings");
	// This is not quite right. Logically, 'x' should be a value set ["{{expression}}", "{expression}", "expression"]
	// Not set up to implement this yet. ????
}

void TestSimple()
{
	SN_DECLARE(result);
	(Long(5) + Long(6) == result).Assert().Do();
	(result > Long(11)).Debug().Evaluate().Do();
}

void TestPythagoras()
{
	SN_DECLARE(X);
	(Double(245.67).Square() + X.Square() == Double(357.56).Square()).Assert().Do();
	string X_string = X.BuildSet().DoEvaluate().DisplaySN();
	ASSERTM(X_string == "{Double(259.798777), Double(-259.798777)}", "");
}

void TestChurchDivide()
{
	SN_DECLARE(divide);

	SN_DECLARE(div);
	SN_DECLARE(divide1);
	SN_DECLARE(succ);
	SN_DECLARE(Y);
	SN_DECLARE(zero);
	SN_DECLARE(IsZero);
	SN_DECLARE(falseL);
	SN_DECLARE(trueL);
	SN_DECLARE(minus);
	SN_DECLARE(pred);

	SN_DECLARE(inc);
	SN_DECLARE(f);
	SN_DECLARE(x);
	SN_DECLARE(n);
	SN_DECLARE(m);
	SN_DECLARE(a);
	SN_DECLARE(b);
	SN_DECLARE(c);
	SN_DECLARE(d);
	SN_DECLARE(u);
	SN_DECLARE(v);
	SN_DECLARE(g);
	SN_DECLARE(h);

	SN_DECLARE(r1);
	SN_DECLARE(r2);
	SN_DECLARE(r3);

	(inc(v) == v + Long(1)).PartialAssert().Do();
	(inc(inc(inc(Long(0)))) == Long(3)).Assert().Do();

	// Y = \f.(\x.x x) (\x.f (x x))
	(Define(Y) == Lambda(f, Lambda(x, f(x(x)))(Lambda(x, f(x(x)))))).PartialAssert().Do();

	// \n.\f.\x.n (\g.\h.h (g f)) (\u.x) (\u.u)
	(Define(pred) == Lambda(n, Lambda(f, Lambda(x, n(Lambda(g, Lambda(h, h(g(f))))) (Lambda(u, x))(Lambda(u, u)))))).PartialAssert().Do();

	// minus m n = (n pred) m	
	(Define(minus)(m)(n) == n(pred)(m)).PartialAssert().Do();

	// div = \c.\n.\m.\f.\x.(\d.IsZero d (0 f x) (f (c d m f x))) (minus n m)
	(Define(div) == Lambda(c, Lambda(n, Lambda(m, Lambda(f, Lambda(x, (Lambda(d, IsZero(d)(zero(f)(x))(f(c(d)(m)(f)(x))))(minus(n)(m))))))))).PartialAssert().Do();

	// divide1 == Y div
	(Define(divide1) == Y(div)).PartialAssert().Do();

	// divide = \n.divide1 (succ n)
	(Define(divide) == Lambda(n, divide1(succ(n)))).PartialAssert().Do();

	// succ n f x = f (n f x)
	(Define(succ)(n)(f)(x) == f(n(f)(x))).PartialAssert().Do();

	// zero = \f.\x.x
	(Define(zero) == Lambda(f, Lambda(x, x))).PartialAssert().Do();

	// trueL = \a.\b.a
	(Define(trueL) == Lambda(a, Lambda(b, a))).PartialAssert().Do();

	// falseL = \a.\b.a
	(Define(falseL) == Lambda(a, Lambda(b, b))).PartialAssert().Do();

	// IsZero = \n.n (\x.false) true
	(Define(IsZero) == Lambda(n, n(Lambda(x, falseL))(trueL))).PartialAssert().Do();

	//(divide(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == Long(1)).Evaluate().Do();
	(divide(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == r1).Assert().Do();
	(r1 == Long(1)).Evaluate().Do();

	(divide(Lambda(f, Lambda(x, f(f(f(f(x)))))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == Long(2)).Evaluate().Do();
	(divide(Lambda(f, Lambda(x, f(f(f(f(x)))))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == r2).Assert().Do();
	(r2 == Long(2)).Evaluate().Do();

	(divide(Lambda(f, Lambda(x, f(f(f(f(f(f(f(f(f(x))))))))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == Long(3)).Evaluate().Do();
	(divide(Lambda(f, Lambda(x, f(f(f(f(f(f(f(f(f(x))))))))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == r3).Assert().Do();
	(r3 == Long(3)).Evaluate().Do();

}


void main(int argc, char *argv[])
{
	Manager manager("Test Church Divide");
	manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root);
	//manager.StartDebugCommandLineServer(skynet::StepInto, _kbhit, _getch);
	try
	{
		// TestIsInteger();
		// TestCharInValueSet();
		// TestValueSetOfStandardFunctions();
		// TestValueSetOfLambdaFunctions();
		// TestValueSetOfStringFunctions();
		// TestPythagoras();
		// TestSimple();
		
		TestChurchDivide();
	}
	catch (Error e)
	{
		string description = e.GetDescription();
		cout << "Error: " + description;
	}
}
