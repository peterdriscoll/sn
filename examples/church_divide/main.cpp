#include <iostream>
#include <string>
#include <conio.h>

#include "sn.h"
using namespace PGCX;
using namespace skynet; // Interface namespace for SN.

void TestValueSetOfStringFunctions()
{
	Transaction transaction;

	SN_DECLARE(f);

	SN_DECLARE(wrap);
	SN_DECLARE(map);
	{
		SN_DECLARE(a);

		(Define(wrap)(a) == String("{") + (f(a) + String("}"))).PartialAssertAction();

		SN_DECLARE(c);
		SN_DECLARE(d);

		(Define(map)(a) == a).PartialAssertAction();
	}
	string f_before_string = f.DisplaySN();

	(Define(f) == (wrap || map)).PartialAssertAction();
	string f_string = f.DisplaySN();

	SN_DECLARE(x);
	(String("{{expression}}") == f(x)).AssertAction();
	string x_display = x.DisplaySN();
	string x_string = x.BuildSet().Evaluate().DisplaySN();
	string x_compare_string = "{String(\"expression\"), String(\"{expression}\"), String(\"{{expression}}\")}";

	ASSERTM(x_string == x_compare_string, "compare strings");
	// This is not quite right. Logically, 'x' should be a value set ["{{expression}}", "{expression}", "expression"]
	// Not set up to implement this yet. ????
}

void TestSimple()
{
	SN_DECLARE(result);
	(Long(5) + Long(6) == result).AssertAction();
	(result > Long(11)).Debug().EvaluateAction();
}

void TestPythagoras()
{
	SN_DECLARE(X);
	(Double(245.67).Square() + X.Square() == Double(357.56).Square()).AssertAction();
	string X_string = X.BuildSet().Evaluate().DisplaySN();
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

	(inc(v) == v + Long(1)).PartialAssertAction();
	(inc(inc(inc(Long(0)))) == Long(3)).AssertAction();

	// divide = \n.divide1 (succ n)
	(Define(divide) == Lambda(n, divide1(succ(n)))).PartialAssertAction();

	// divide1 == Y div
	(Define(divide1) == Y(div)).PartialAssertAction();

	// div = \c.\n.\m.\f.\x.(\d.IsZero d (0 f x) (f (c d m f x))) (minus n m)
	(Define(div) == Lambda(c, Lambda(n, Lambda(m, Lambda(f, Lambda(x, (Lambda(d, IsZero(d)(zero(f)(x))(f(c(d)(m)(f)(x))))(minus(n)(m))))))))).PartialAssertAction();

	// succ n f x = f (n f x)
	(Define(succ)(n)(f)(x) == f(n(f)(x))).PartialAssertAction();

	// Y = \f.(\x.x x) (\x.f (x x))
	(Define(Y) == Lambda(f, Lambda(x, f(x(x)))(Lambda(x, f(x(x)))))).PartialAssertAction();

	// zero = \f.\x.x
	(Define(zero) == Lambda(f, Lambda(x, x))).PartialAssertAction();

	// IsZero = \n.n (\x.false) true
	(Define(IsZero) == Lambda(n, n(Lambda(x, falseL))(trueL))).PartialAssertAction();

	// trueL = \a.\b.a
	(Define(trueL) == Lambda(a, Lambda(b, a))).PartialAssertAction();

	// falseL = \a.\b.a
	(Define(falseL) == Lambda(a, Lambda(b, b))).PartialAssertAction();

	// minus m n = (n pred) m	
	(Define(minus)(m)(n) == n(pred)(m)).PartialAssertAction();

	// \n.\f.\x.n (\g.\h.h (g f)) (\u.x) (\u.u)
	(Define(pred) == Lambda(n, Lambda(f, Lambda(x, n(Lambda(g, Lambda(h, h(g(f))))) (Lambda(u, x))(Lambda(u, u)))))).PartialAssertAction();

	(divide(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == Long(1)).EvaluateAction();
	(divide(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == r1).AssertAction();
	(r1 == Long(1)).EvaluateAction();

	(divide(Lambda(f, Lambda(x, f(f(f(f(x)))))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == Long(2)).EvaluateAction();
	(divide(Lambda(f, Lambda(x, f(f(f(f(x)))))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == r2).AssertAction();
	(r2 == Long(2)).EvaluateAction();

	(divide(Lambda(f, Lambda(x, f(f(f(f(f(f(f(f(f(x))))))))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == Long(3)).EvaluateAction();
	(divide(Lambda(f, Lambda(x, f(f(f(f(f(f(f(f(f(x))))))))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == r3).AssertAction();
	(r3 == Long(3)).EvaluateAction();
}


void main(int argc, char *argv[])
{
	Manager manager; // Sets up any configuration parameters for SN
	manager.StartDebug(SN::StepInto, _kbhit, _getch);

	Transaction transaction;
	try
	{
		TestValueSetOfStringFunctions();
		// TestPythagoras();
		// TestSimple();
		// TestChurchDivide();
	}
	catch (Error e)
	{
		string description = e.GetDescription();
		cout << "Error: " + description;
	}
}
