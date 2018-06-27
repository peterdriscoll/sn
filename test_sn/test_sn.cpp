#include "sn.h"

#include <cstdio>

#include "test_sn_pch.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace skynet;
using namespace PGCX;

bool runWebServer = false;
bool runWebServer2 = false;
bool runWebServer3 = false;

class TestEnd
{
public:
	TestEnd() {};
	~TestEnd()
	{
	};

};

//TestEnd TE;

namespace test_sn
{
	void AssertErrorHandler(SN::SN_Error p_Result)
	{
		string description = p_Result.GetDescription();
		Assert::IsTrue(p_Result.GetBool(), wstring(description.begin(), description.end()).c_str());
	}

	TEST_CLASS(test_sn)
	{
	private:
		void Initialize()
		{
			Transaction::ResetNetMemoryUsed();
			Transaction::ResetGrossMemoryUsed();
		}

		void Cleanup()
		{
			Assert::IsTrue(Transaction::TotalNetMemoryUsed() == 0);
			Assert::IsTrue(Promotion::PromotionUsedMemory() == 0);
			Assert::IsTrue(Promotion::PromotionFreeMemory() == Transaction::TotalGrossMemoryUsed());
		}

	public:

		//	TestStringValue
		//		Test operator equals on program model string value gives the program model function call.
		//
		//		In the program model, operations dont return values.  Instead they return program model objects that represent the calculation of the values.
		//		The function Equivalent determines if the project model produced by == is the application of Equals as a function call on the two parameters.
		//
		//		Equals is a FunctionDef object that defines the function.
		TEST_METHOD(TestStringEquivalent)
		{
			Initialize();
			Assert::IsTrue(Transaction::TotalNetMemoryUsed() == 0);
			{
				Manager manager("Test String Equivalent", AssertErrorHandler);
				// manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				Assert::IsTrue((String("dog") == String("dog")).Equivalent(Function(Function(Equals, String("dog")), String("dog"))));
				Assert::IsTrue(!(String("dog") == String("dog")).Equivalent(Function(Function(Equals, String("dog")), String("cat"))));
				Assert::IsTrue((String("dog") + String("cat")).Equivalent(Function(Function(Add, String("dog")), String("cat"))));
				Assert::IsTrue((String("dog") == String("dog")).Equivalent(Equals(String("dog"))(String("dog"))));
				Assert::IsTrue(!(String("dog") == String("dog")).Equivalent(Equals(String("dog"))(String("cat"))));
				Assert::IsTrue((String("dog") + String("cat")).Equivalent(Add(String("dog"))(String("cat"))));
			}
			Cleanup();
		}

		TEST_METHOD(TestStringEvaluate)
		{
			Initialize();
			{
				Manager manager("Test String Evaluate", AssertErrorHandler);

				(String("dog") == String("dog")).EvaluateAction();
				(!(String("dog") == String("cat"))).EvaluateAction();

				SN_DECLARE_VALUE(z, String("dog"));
				(String("dog") == z).EvaluateAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestStringPartialEvaluate)
		{
			Initialize();
			{
				Manager manager("Test String Partial Evaluate", AssertErrorHandler);

				SN_DECLARE(y);
				SN_DECLARE(x);
				SN_DECLARE_VALUE(z, String("dog"));

				Assert::IsTrue((String("dog") == y).PartialEvaluate().Equivalent(String("dog") == y));
				Assert::IsTrue(!(String("dog") == y).PartialEvaluate().Equivalent(String("dog") == x));
				Assert::IsTrue((String("dog") == String("dog")).PartialEvaluate().Equivalent(Bool(true)));
				(String("dog") == z).PartialEvaluateAction();

				Assert::IsTrue((String("dog") + y == String("dogcat")).PartialEvaluate().Equivalent(Function(Function(Equals, Function(Function(Add, String("dog")), y)), String("dogcat"))));
				(String("dog") + z == String("dogdog")).PartialEvaluateAction();

				(String("dog") + z == "dogdog").PartialEvaluateAction();
				(String("dog") + "dog" == "dogdog").PartialEvaluateAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestStringAssert)
		{
			Initialize();
			{
				Manager manager("Test String Assert", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);
				{
					Transaction transaction;

					SN_DECLARE(y);

					(String("dog") == y).AssertAction();
					(String("dog") == y).EvaluateAction();

					SN_DECLARE(z);

					(String("dog") + z == "dogcat").AssertAction();
					Value cat_value = z.GetValue();
					string s_cat_value = cat_value.GetString();
					Assert::IsTrue(s_cat_value == "cat");
					(String("cat") == z).EvaluateAction();
				}
				{
					Transaction transaction;

					SN_DECLARE(a);
					(String("dog") + a == String("dogcat")).AssertAction();
					(a == String("cat")).AssertAction();
					SN_DECLARE(b);
					(b + String("cat") == String("dogcat")).AssertAction();
					(b == String("dog")).AssertAction();
					SN_DECLARE(c);
					(String("dog") + String("cat") == c).AssertAction();
					(c == String("dogcat")).AssertAction();
				}
				{
					Transaction transaction;

					SN_DECLARE(a);
					(String("dogcat").SubtractRight(String("cat")) == a).AssertAction();
					(a == String("dog")).AssertAction();
					SN_DECLARE(b);
					(String("dogcat").SubtractRight(b) == String("dog")).AssertAction();
					(b == String("cat")).AssertAction();
					SN_DECLARE(c);
					(c.SubtractRight(String("cat")) == String("dog")).AssertAction();
					(c == String("dogcat")).AssertAction();
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestBoolEquivalent)
		{
			Initialize();
			{
				Manager manager("Test Bool Equivalent", AssertErrorHandler);

				Assert::IsTrue((Bool(true) == Bool(true)).Equivalent(Function(Function(Equals, Bool(true)), Bool(true))));
				Assert::IsTrue(!(Bool(true) == Bool(true)).Equivalent(Function(Function(Equals, Bool(true)), Bool(false))));
				Assert::IsTrue((Bool(true) && Bool(false)).Equivalent(Function(Function(And, Bool(true)), Bool(false))));
				Assert::IsTrue((Bool(false) || Bool(false)).Equivalent(Function(Function(Or, Bool(false)), Bool(false))));
				Assert::IsTrue((!False).Equivalent(Function(Not, False)));
				Assert::IsTrue(False.If(False, True).Equivalent(Function(Function(Function(If, False), False), True)));
			}
			Cleanup();
		}

		TEST_METHOD(TestBoolEvaluate)
		{
			Initialize();
			{
				Manager manager("Test Bool Evaluate", AssertErrorHandler);

				{
					Transaction transaction;

					(Bool(true) == Bool(true)).EvaluateAction();
					(!(Bool(true) == Bool(false))).EvaluateAction();
				}
				{
					Transaction transaction;

					SN_DECLARE_VALUE(z, Bool(true));
					(Bool(true) == z).EvaluateAction();

					SN_DECLARE_VALUE(k, True);
					SN_DECLARE_VALUE(l, False);
					SN_DECLARE_VALUE(m, True);
					(k.If(l, m) == False).EvaluateAction();
					(True.If("dog", "cat") == "dog").EvaluateAction();
					(False.If("dog", "cat") == "cat").EvaluateAction();
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestBoolEvaluate2)
		{
			Initialize();
			{
				Manager manager("Test Bool Evaluate2", AssertErrorHandler);

				{
					Transaction transaction;
					//Truth table AND
					(!(Bool(false) && Bool(false))).EvaluateAction();
					(!(Bool(false) && Bool(true))).EvaluateAction();
					(!(Bool(true) && Bool(false))).EvaluateAction();
					(Bool(true) && Bool(true)).EvaluateAction();
				}
				{
					Transaction transaction;

					// Truth table OR
					(!(Bool(false) || Bool(false))).EvaluateAction();
					(Bool(false) || Bool(true)).EvaluateAction();
					(Bool(true) || Bool(false)).EvaluateAction();
					(Bool(true) || Bool(true)).EvaluateAction();
				}
				{
					Transaction transaction;

					// Truth table AND
					((Bool(false) && Bool(false)) == Bool(false)).EvaluateAction();
					((Bool(false) && Bool(true)) == Bool(false)).EvaluateAction();
					((Bool(true) && Bool(false)) == Bool(false)).EvaluateAction();
					((Bool(true) && Bool(true)) == Bool(true)).EvaluateAction();
				}
				{
					Transaction transaction;

					// Truth table OR
					((Bool(false) || Bool(false)) == Bool(false)).EvaluateAction();
					((Bool(false) || Bool(true)) == Bool(true)).EvaluateAction();
					((Bool(true) || Bool(false)) == Bool(true)).EvaluateAction();
					((Bool(true) || Bool(true)) == Bool(true)).EvaluateAction();
				}
				{
					Transaction transaction;

					// Truth table if A then B else C
					(False.If(False, False) == False).EvaluateAction();
					(False.If(False, True) == True).EvaluateAction();
					(False.If(True, False) == False).EvaluateAction();
					(False.If(True, True) == True).EvaluateAction();
					(True.If(False, False) == False).EvaluateAction();
					(True.If(False, True) == False).EvaluateAction();
					(True.If(True, False) == True).EvaluateAction();
					(True.If(True, True) == True).EvaluateAction();
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestBoolPartialEvaluate)
		{
			Initialize();
			{
				Manager manager("Test Bool Partial Evaluate", AssertErrorHandler);

				SN_DECLARE(y);
				SN_DECLARE(x);
				SN_DECLARE_VALUE(z, Bool(true));


				Assert::IsTrue((Bool(true) == y).PartialEvaluate().Equivalent(Bool(true) == y));
				Assert::IsTrue(!(Bool(true) == y).PartialEvaluate().Equivalent(Bool(true) == x));
				Assert::IsTrue((Bool(true) == Bool(true)).PartialEvaluate().Equivalent(Bool(true)));
				(Bool(true) == z).PartialEvaluateAction();

				Assert::IsTrue((Bool(false) || y).PartialEvaluate().Equivalent(y));
				Assert::IsTrue((y || Bool(false)).PartialEvaluate().Equivalent(y));
				Assert::IsTrue((Bool(true) || y).PartialEvaluate().Equivalent(Bool(true)));
				Assert::IsTrue((y || Bool(true)).PartialEvaluate().Equivalent(Bool(true)));

				Assert::IsTrue((Bool(true) && y).PartialEvaluate().Equivalent(y));
				Assert::IsTrue((y && Bool(true)).PartialEvaluate().Equivalent(y));
				Assert::IsTrue((Bool(false) && y).PartialEvaluate().Equivalent(Bool(false)));
				Assert::IsTrue((y && Bool(false)).PartialEvaluate().Equivalent(Bool(false)));

				(Bool(false) || z).PartialEvaluateAction();
				(Bool(true) && true).PartialEvaluateAction();

				(Bool(true) && true).PartialEvaluateAction();

				SN_DECLARE(i);
				(i == i).PartialEvaluateAction();

				// If

				SN_DECLARE(k);
				SN_DECLARE(l);

				Assert::IsTrue((k.If(False, True) == l).PartialEvaluate().Equivalent((!k == l).PartialEvaluate()));
				Assert::IsTrue((k.If(True, False) == l).PartialEvaluate().Equivalent((k == l).PartialEvaluate()));
				Assert::IsTrue(k.If(True, True).PartialEvaluate().Equivalent(True));
				Assert::IsTrue(k.If(False, False).PartialEvaluate().Equivalent(False));
				Assert::IsTrue(True.If(k, l).PartialEvaluate().Equivalent(k));
				Assert::IsTrue(False.If(k, l).PartialEvaluate().Equivalent(l));

				Assert::IsTrue(True.If(True, False).PartialEvaluate().Equivalent(True));

				// Not
				Assert::IsTrue((!!k).PartialEvaluate().Equivalent(k));
				Assert::IsTrue((!(k.If(False, True))).PartialEvaluate().Equivalent(k));
			}
			Cleanup();
		}

		TEST_METHOD(TestBoolAssert)
		{
			Initialize();
			{
				Manager manager("Test Bool Assert", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(z);

				(z || Bool(false) == Bool(true)).AssertAction();

				(Bool(true) == z).EvaluateAction();
				(!(Bool(false) && z)).PartialEvaluateAction();
				(Bool(true) && Bool(true)).PartialEvaluateAction();


				SN_DECLARE(y);

				(Bool(true) == y.Debug()).AssertAction();
				(Bool(true) == y).EvaluateAction();

				SN_DECLARE(k1);
				SN_DECLARE_VALUE(l1, False);

				(k1.Debug().If(False, True) == l1).AssertAction();
				(k1 == !l1).EvaluateAction();

				SN_DECLARE(k2);
				SN_DECLARE_VALUE(l2, True);

				(k2.If(False, True) == l2).AssertAction();
				(k2 == !l2).EvaluateAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestBoolAssertIf)
		{
			Initialize();
			{
				Manager manager("Test Bool Assert If", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(m);
				SN_DECLARE(n);
				SN_DECLARE(o);

				(o.If(False, True) == False).AssertAction();
				o.EvaluateAction();
				(o.If(m, n) == True).AssertAction();
				m.EvaluateAction();
				((!o).If(m, n) == m).AssertAction();
				n.EvaluateAction();
			}
			Cleanup();
		}
		TEST_METHOD(TestBoolNotAndNot)
		{
			Initialize();
			{
				Manager manager("Test Bool Not And Not", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				(!((Bool(true) && Bool(false)))).AssertAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestDoubleEquivalent)
		{
			Initialize();
			{
				Manager manager("Test Double Equivalent", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				((Long(1) + Long(2)) == Long(3)).EvaluateAction();

				Assert::IsTrue((Double(5.5) == Double(7.8)).Equivalent(Equals(Double(5.5))(Double(7.8))));
				Assert::IsTrue(!(Double(9.0) == Double(9.5)).Equivalent(Equals(Double(9.0))(Double(9.7))));
				Assert::IsTrue((Double(8.5) + Double(4.5)).Equivalent(Add(Double(8.5))(Double(4.5))));
				Assert::IsTrue((Double(8.5) - Double(4.5)).Equivalent(Subtract(Double(8.5))(Double(4.5))));
				Assert::IsTrue((Double(8.5) * Double(4.5)).Equivalent(Multiply(Double(8.5))(Double(4.5))));
				Assert::IsTrue((Double(8.5) / Double(4.5)).Equivalent(Divide(Double(8.5))(Double(4.5))));
			}
			Cleanup();
		}

		TEST_METHOD(TestDoubleEvaluate)
		{
			Initialize();
			{
				Manager manager("Test Double Evaluate", AssertErrorHandler);

				(Double(3.3) == Double(3.3)).EvaluateAction();
				(!(Double(5.43) == Double(6))).EvaluateAction();

				SN_DECLARE_VALUE(z, Double(2.2));
				(Double(3.5) + Double(5.6) == Double(9.1)).EvaluateAction();
				(Double(2.2) == z).EvaluateAction();

				SN_DECLARE_VALUE(x, Double(3));
				(z * x == Double(6.6)).EvaluateAction();

				SN_DECLARE_VALUE(y, Double(2));
				((z * x) / y == Double(3.3)).EvaluateAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestDoublePartialEvaluate)
		{
			Initialize();
			{
				Manager manager("Test Double Partial Evaluate", AssertErrorHandler);

				SN_DECLARE(y);
				SN_DECLARE(x);
				SN_DECLARE_VALUE(z, Double(3.1415));


				Assert::IsTrue((Double(3.1415) == y).PartialEvaluate().Equivalent(Double(3.1415) == y));
				Assert::IsTrue(!(Double(9.7) == y).PartialEvaluate().Equivalent(Double(9.7) == x));
				Assert::IsTrue((Double(9.7) == Double(9.7)).PartialEvaluate().Equivalent(Bool(true)));
				(Double(3.1415) == z).PartialEvaluateAction();

				Assert::IsTrue((Double(5.5) + y == Double(10)).PartialEvaluate().Equivalent(Equals(Add(Double(5.5))(y))(Double(10))));
				(Double(8.0) + z == Double(11.1415)).PartialEvaluateAction();

				(Double(3.1415) + z == Double(3.1415)*Double(2.0)).PartialEvaluateAction();
				(Double(4.5) + 4.5 == 9.0).PartialEvaluateAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestDoubleAssert)
		{
			Initialize();
			{
				Manager manager("Test Double Assert", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(y);

				(Double(45.6) == y).AssertAction();
				(Double(45.6) == y).EvaluateAction();

				SN_DECLARE(z);

				(Double(45.6) + z == 100.0).AssertAction();

				(Double(54.4) == z).EvaluateAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestLongEvaluate)
		{
			Initialize();
			{
				Manager manager("Test Long Evaluate", AssertErrorHandler);

				(Long(4) * Double(1.25) == Double(5)).EvaluateAction();
				(Double(5) / Long(4) == Double(1.25)).EvaluateAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestLongAssertSimplified)
		{
			Initialize();
			{
				Manager manager("Test Long Assert Simplified", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(c);

				(Long(2) + Long(3) == c).AssertAction();

				Assert::IsTrue(c.Equivalent(Long(5)));
			}
			Cleanup();
		}
		
		TEST_METHOD(TestLongAssert)
		{
			Initialize();
			{
				Manager manager("Test Long Assert", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);
				{
					Transaction transaction;

					SN_DECLARE(a);
					SN_DECLARE(b);
					SN_DECLARE(c);

					(a + Long(3) == Long(5)).AssertAction();
					(Long(2) + b == Long(5)).AssertAction();
					(Long(2) + Long(3) == c).AssertAction();

					Assert::IsTrue(a.Equivalent(Long(2)));
					Assert::IsTrue(b.Equivalent(Long(3)));
					Assert::IsTrue(c.Equivalent(Long(5)));
				}

				{
					Transaction transaction;

					SN_DECLARE(x);
					SN_DECLARE(y);
					SN_DECLARE(z);

					(x - Long(3) == Long(2)).AssertAction();
					(Long(5) - y == Long(2)).AssertAction();
					(Long(5) - Long(3) == z).AssertAction();

					Assert::IsTrue(x.Equivalent(Long(5)));
					Assert::IsTrue(y.Equivalent(Long(3)));
					Assert::IsTrue(z.Equivalent(Long(2)));
				}

				{
					Transaction transaction;

					SN_DECLARE(a);
					SN_DECLARE(b);
					SN_DECLARE(c);

					(a * Double(1.25) == Long(5)).AssertAction();
					(Long(4) * b == Double(5)).AssertAction();
					(Long(4) * Double(1.25) == c).AssertAction();

					Assert::IsTrue(a.Equivalent(Double(4)));
					Assert::IsTrue(b.Equivalent(Double(1.25)));
					Assert::IsTrue(c.Equivalent(Double(5)));
				}

				{
					Transaction transaction;

					SN_DECLARE(x);
					SN_DECLARE(y);
					SN_DECLARE(z);

					(x / Long(4) == Double(1.25)).AssertAction();
					(Long(5) / y == Double(1.25)).AssertAction();
					(Double(5) / Long(4) == z).AssertAction();

					Assert::IsTrue(x.Equivalent(Double(5)));
					Assert::IsTrue(y.Equivalent(Double(4)));
					Assert::IsTrue(z.Equivalent(Double(1.25)));
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestFunctionDefinition)
		{
			Initialize();
			{
				Manager manager("Test Function Definition", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				{
					SN_DECLARE(RemovePrefix);
					SN_DECLARE(p);
					SN_DECLARE(x);

					(Define(RemovePrefix) == Lambda(p, Lambda(x, x.SubtractLeft(p)))).PartialAssertAction();
					std::cout << std::endl << "Variable " << RemovePrefix.DisplaySN() << std::endl;
					string s_RemovePrefix = RemovePrefix.GetVariableValue().DisplaySN();
					Assert::IsTrue(s_RemovePrefix == "@p.@x.SubtractLeft x p");
					string v_RemovePrefix = Lambda(p, Lambda(x, x.SubtractLeft(p))).PartialEvaluate().DisplaySN();
					Assert::IsTrue(v_RemovePrefix == "@p.@x.SubtractLeft x p");
					Assert::IsTrue(RemovePrefix.PartialEvaluate().Equivalent(Lambda(p, Lambda(x, x.SubtractLeft(p))).PartialEvaluate()));
					(RemovePrefix(String("Atl"))(String("AtlDog")) == String("Dog")).EvaluateAction();
				}
				{
					SN_DECLARE(RemovePostfix);
					SN_DECLARE(p);
					SN_DECLARE(x);

					(Define(RemovePostfix) == Lambda(p, Lambda(x, x.SubtractRight(p)))).PartialAssertAction();
					Assert::IsTrue(RemovePostfix.GetVariableValue().DisplaySN() == "@p.@x.SubtractRight x p");
					std::cout << std::endl << "Variable " << RemovePostfix.DisplaySN() << std::endl;
					Assert::IsTrue(RemovePostfix.PartialEvaluate().Equivalent(Lambda(p, Lambda(x, x.SubtractRight(p))).PartialEvaluate()));
					(RemovePostfix(String("Atl"))(String("DogAtl")) == String("Dog")).EvaluateAction();
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestRecursiveLambdaFunctionDefinition)
		{
			Initialize();
			{
				Manager manager("Test Function Definition", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(Factorial);
				SN_DECLARE(n);

				(Define(Factorial) == Lambda(n, (n == Long(0)).If(Long(1), n * Factorial(n - Long(1))))).PartialAssertAction();
				std::cout << std::endl << "Function " << Factorial.DisplaySN() << std::endl;
				(Factorial(Long(3)) == Long(6)).EvaluateAction();
				(Factorial(Long(10)) == Long(3628800)).EvaluateAction();
				(Factorial(Long(12)) == Long(479001600)).EvaluateAction();

				SN_DECLARE(Fact);
				SN_DECLARE(m);

				(Define(Fact)(m) == (m == Long(0)).If(Long(1), m * Fact(m - Long(1)))).PartialAssertAction();
				std::cout << std::endl << "Function " << Fact.DisplaySN() << std::endl;
				(Fact(Long(3)) == Long(6)).EvaluateAction();
				(Fact(Long(10)) == Long(3628800)).EvaluateAction();
				(Fact(Long(12)) == Long(479001600)).EvaluateAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestRecursiveFunctionDefinition)
		{
			Initialize();
			{
				Manager manager("Test Recursive Function Definition", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);
				{
					Transaction transaction;

					SN_DECLARE(Fact);
					SN_DECLARE(m);

					(Define(Fact)(m) == (m == Long(0)).If(Long(1), m * Fact(m - Long(1)))).PartialAssertAction();

					(Fact(Long(0)) == Long(1)).AssertAction();
					(Fact(Long(1)) == Long(1)).AssertAction();
					(Fact(Long(3)) == Long(6)).AssertAction();
					(Fact(Long(10)) == Long(3628800)).AssertAction();
					(Fact(Long(12)) == Long(479001600)).AssertAction();
				}
				{
					Transaction transaction;

					SN_DECLARE(Fact);
					SN_DECLARE(k);
					SN_DECLARE(n);
					// Fact 0 == 1 && Fact k : > 0 == k * Fact k-1 : < 0
					((Fact(Long(0)) == Long(1)) && (Fact(k, Lambda(n, n > Long(0))) == k * Fact(k - Long(1), Lambda(n, n < Long(0))))).PartialAssertAction();
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestPartialCall)
		{
			Initialize();
			{
				Manager manager("Test Partial Call", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);
				SN_DECLARE(f);
				SN_DECLARE(g);
				SN_DECLARE(a);
				SN_DECLARE(b);
				SN_DECLARE(c);

				(f(a)(b) == a * b).PartialAssertAction();
				(Define(g) == f(Long(5))).PartialAssertAction();

				(g(Long(4)) == c).AssertAction();
				string c_string = c.GetVariableValue().DisplayValueSN();
				Assert::IsTrue(c_string == "Long(20)");

				(c == Long(20)).AssertAction();
			}
			Cleanup();
		}


		TEST_METHOD(TestYCombinator)
		{
			Initialize();
			{
				Manager manager("Test Y Combinator", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);
				SN_DECLARE(Y);
				SN_DECLARE(f);
				SN_DECLARE(x);

				// Y = \f.(\x.x x) (\x.f (x x))
				(Define(Y) == Lambda(f, Lambda(x, f(x(x)))(Lambda(x, f(x(x)))))).PartialAssertAction();

				SN_DECLARE(Fact);
				SN_DECLARE(g);
				SN_DECLARE(n);

				(Define(Fact)(g)(n) == (n == Long(0)).If(Long(1), n * g(n - Long(1)))).PartialAssertAction();

				(Y(Fact)(Long(0)) == Long(1)).EvaluateAction();
				(Y(Fact)(Long(1)) == Long(1)).EvaluateAction();
				(Y(Fact)(Long(3)) == Long(6)).EvaluateAction();
				(Y(Fact)(Long(10)) == Long(3628800)).EvaluateAction();
				(Y(Fact)(Long(12)) == Long(479001600)).EvaluateAction();

				(Y(Fact)(Long(0)) == Long(1)).AssertAction();
				(Y(Fact)(Long(1)) == Long(1)).AssertAction();
				(Y(Fact)(Long(3)) == Long(6)).AssertAction();
				(Y(Fact)(Long(10)) == Long(3628800)).AssertAction();
				(Y(Fact)(Long(12)) == Long(479001600)).AssertAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestChurchSucc)
		{
			Initialize();
			{
				Manager manager("Test Church Succ", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(inc);
				SN_DECLARE(succ);
				SN_DECLARE(n);
				SN_DECLARE(f);
				SN_DECLARE(x);
				SN_DECLARE(v);
				SN_DECLARE(r0);
				SN_DECLARE(r1);
				SN_DECLARE(r2);
				SN_DECLARE(r3);
				(inc(v) == v + Long(1)).PartialAssertAction();
				(inc(inc(inc(Long(0)))) == Long(3)).AssertAction();

				// succ n f x = f (n f x)
				(Define(succ)(n)(f)(x) == f(n(f)(x))).PartialAssertAction();

				(succ(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == Long(1)).EvaluateAction();
				(succ(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == r0).AssertAction();
				(r0 == Long(1)).EvaluateAction();


				(succ(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == Long(2)).EvaluateAction();
				(succ(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == r1).AssertAction();
				(r1 == Long(2)).EvaluateAction();

				(succ(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == Long(3)).EvaluateAction();
				(succ(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == r2).AssertAction();
				(r2 == Long(3)).EvaluateAction();

				(succ(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == Long(4)).EvaluateAction();
				(succ(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == r3).AssertAction();
				(r3 == Long(4)).EvaluateAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestChurchPlus)
		{
			Initialize();
			{
				Manager manager("Test Church Plus", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(plus);
				SN_DECLARE(n);
				SN_DECLARE(m);
				SN_DECLARE(f);
				SN_DECLARE(inc);
				SN_DECLARE(x);
				SN_DECLARE(v);
				SN_DECLARE(r0);
				SN_DECLARE(r1);
				SN_DECLARE(r2);
				SN_DECLARE(r3);
				(inc(v) == v + Long(1)).PartialAssertAction();
				(inc(inc(inc(Long(0)))) == Long(3)).AssertAction();

				// plus m n f x = m f (n f x) 
				(Define(plus) == Lambda(m, Lambda(n, Lambda(f, Lambda(x, m(f)((n(f)(x)))))))).PartialAssertAction();

				(plus(Lambda(f, Lambda(x, x)))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == Long(0)).EvaluateAction();
				(plus(Lambda(f, Lambda(x, x)))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == r0).AssertAction();
				(r0 == Long(0)).EvaluateAction();

				(plus(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == Long(2)).EvaluateAction();
				(plus(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == r1).AssertAction();
				(r1 == Long(2)).EvaluateAction();

				(plus(Lambda(f, Lambda(x, f(f(x)))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == Long(4)).EvaluateAction();
				(plus(Lambda(f, Lambda(x, f(f(x)))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == r2).AssertAction();
				(r2 == Long(4)).EvaluateAction();

				(plus(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == Long(6)).EvaluateAction();
				(plus(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == r3).AssertAction();
				(r3 == Long(6)).EvaluateAction();
			}
			Cleanup();
		}
		
		TEST_METHOD(TestChurchMultiply)
		{
			Initialize();
			{
				Manager manager("Test Church Multiply", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(mult);
				SN_DECLARE(n);
				SN_DECLARE(m);
				SN_DECLARE(f);
				SN_DECLARE(inc);
				SN_DECLARE(x);
				SN_DECLARE(v);
				SN_DECLARE(r0);
				SN_DECLARE(r1);
				SN_DECLARE(r2);
				SN_DECLARE(r3);
				(inc(v) == v + Long(1)).PartialAssertAction();
				(inc(inc(inc(Long(0)))) == Long(3)).AssertAction();

				// multiply m n f x = m (n f) x
				(Define(mult)(m)(n)(f)(x) == m(n(f))(x)).PartialAssertAction();

				(mult(Lambda(f, Lambda(x, x)))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == Long(0)).EvaluateAction();
				(mult(Lambda(f, Lambda(x, x)))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == r0).AssertAction();
				(r0 == Long(0)).EvaluateAction();

				(mult(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == Long(1)).EvaluateAction();
				(mult(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == r1).AssertAction();
				(r1 == Long(1)).EvaluateAction();

				(mult(Lambda(f, Lambda(x, f(f(x)))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == Long(4)).EvaluateAction();
				(mult(Lambda(f, Lambda(x, f(f(x)))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == r2).AssertAction();
				(r2 == Long(4)).EvaluateAction();

				(mult(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == Long(9)).EvaluateAction();
				(mult(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == r3).AssertAction();
				(r3 == Long(9)).EvaluateAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestChurchExp)
		{
			Initialize();
			{
				Manager manager("Test Church Exp", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(exp);
				SN_DECLARE(n);
				SN_DECLARE(m);
				SN_DECLARE(f);
				SN_DECLARE(inc);
				SN_DECLARE(x);
				SN_DECLARE(v);
				SN_DECLARE(r0);
				SN_DECLARE(r1);
				SN_DECLARE(r2);
				SN_DECLARE(r3);
				(inc(v) == v + Long(1)).PartialAssertAction();
				(inc(inc(inc(Long(0)))) == Long(3)).AssertAction();

				// exp m n f x = (n m) f x 
				(Define(exp)(m)(n)(f)(x) == m(n)(f)(x)).PartialAssertAction();

				(exp(Lambda(f, Lambda(x, x)))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == Long(1)).EvaluateAction();
				(exp(Lambda(f, Lambda(x, x)))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == r0).AssertAction();
				(r0 == Long(1)).EvaluateAction();


				(exp(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == Long(1)).EvaluateAction();
				(exp(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == r1).AssertAction();
				(r1 == Long(1)).EvaluateAction();

				(exp(Lambda(f, Lambda(x, f(f(x)))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == Long(4)).EvaluateAction();
				(exp(Lambda(f, Lambda(x, f(f(x)))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == r2).AssertAction();
				(r2 == Long(4)).EvaluateAction();

				(exp(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == Long(27)).EvaluateAction();
				(exp(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == r3).AssertAction();
				(r3 == Long(27)).EvaluateAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestChurchPred)
		{
			Initialize();
			{
				Manager manager("Test Church Pred", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(pred);
				SN_DECLARE(n);
				SN_DECLARE(m);
				SN_DECLARE(h);
				SN_DECLARE(g);
				SN_DECLARE(u);
				SN_DECLARE(f);
				SN_DECLARE(inc);
				SN_DECLARE(x);
				SN_DECLARE(v);
				SN_DECLARE(r0);
				SN_DECLARE(r1);
				SN_DECLARE(r2);
				SN_DECLARE(r3);
				(inc(v) == v + Long(1)).PartialAssertAction();
				(inc(inc(inc(Long(0)))) == Long(3)).AssertAction();

				// \n.\f.\x.n (\g.\h.h (g f)) (\u.x) (\u.u)
				(Define(pred) == Lambda(n, Lambda(f, Lambda(x, n(Lambda(g, Lambda(h, h(g(f))))) (Lambda(u, x))(Lambda(u, u)))))).PartialAssertAction();

				(pred(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == Long(0)).EvaluateAction();
				(pred(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == r0).AssertAction();
				(r0 == Long(0)).EvaluateAction();

				(pred(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == Long(0)).EvaluateAction();
				(pred(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == r1).AssertAction();
				(r1 == Long(0)).EvaluateAction();

				(pred(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == Long(1)).EvaluateAction();
				(pred(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == r2).AssertAction();
				(r2 == Long(1)).EvaluateAction();

				(pred(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == Long(2)).EvaluateAction();
				(pred(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == r3).AssertAction();
				(r3 == Long(2)).EvaluateAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestChurchMinus)
		{
			Initialize();
			{
				Manager manager("Test Church Minus", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(pred);
				SN_DECLARE(h);
				SN_DECLARE(g);
				SN_DECLARE(u);
				SN_DECLARE(minus);
				SN_DECLARE(n);
				SN_DECLARE(m);
				SN_DECLARE(f);
				SN_DECLARE(inc);
				SN_DECLARE(x);
				SN_DECLARE(v);
				SN_DECLARE(r0);
				SN_DECLARE(r1);
				SN_DECLARE(r2);
				SN_DECLARE(r3);
				(inc(v) == v + Long(1)).PartialAssertAction();
				(inc(inc(inc(Long(0)))) == Long(3)).AssertAction();

				// \n.\f.\x.n (\g.\h.h (g f)) (\u.x) (\u.u)
				(Define(pred) == Lambda(n, Lambda(f, Lambda(x, n(Lambda(g, Lambda(h, h(g(f))))) (Lambda(u, x))(Lambda(u, u)))))).PartialAssertAction();

				// minus m n = (n pred) m	
				(Define(minus)(m)(n) == n(pred)(m)).PartialAssertAction();

				// This works too.
				// (Define(minus)(m)(n)(f)(x) == n(pred)(m)(f)(x)).PartialAssertAction();

				(minus(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == Long(1)).EvaluateAction();
				(minus(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == r0).AssertAction();
				(r0 == Long(1)).EvaluateAction();

				(minus(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == Long(2)).EvaluateAction();
				(minus(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == r1).AssertAction();
				(r1 == Long(2)).EvaluateAction();

				(minus(Lambda(f, Lambda(x, f(f(f(f(f(x))))))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == Long(3)).EvaluateAction();
				(minus(Lambda(f, Lambda(x, f(f(f(f(f(x))))))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == r2).AssertAction();
				(r2 == Long(3)).EvaluateAction();

				(minus(Lambda(f, Lambda(x, f(f(f(f(f(f(f(f(x)))))))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == Long(5)).EvaluateAction();
				(minus(Lambda(f, Lambda(x, f(f(f(f(f(f(f(f(x)))))))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == r3).AssertAction();
				(r3 == Long(5)).EvaluateAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestChurchDivide)
		{
			return;
			Initialize();
			{
				Manager manager("Test Church Divide", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

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
				(Define(div) == Lambda(c, Lambda(n, Lambda(m, Lambda(f, Lambda(x, (Lambda(d,IsZero(d)(zero(f)(x))(f(c(d)(m)(f)(x))))(minus(n)(m))))))))).PartialAssertAction();

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
			Cleanup();
		}

		TEST_METHOD(TestStringRefDefinition)
		{
			Initialize();
			{
				Manager manager("Test String Ref Definition", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);
				/*
				SN_DECLARE(x);
				SN_DECLARE(y);
				(String("dogcat") == x + y).AssertAction();
				(x == "dog").AssertAction();
				y.Evaluate().Equivalent(String("cat")));
				*/
				SN_DECLARE(a);
				SN_DECLARE(b);
				SN_DECLARE(c);
				SN_DECLARE(d);
				(String("ratdogcat") == a + b).AssertAction();
				(a == d + c).AssertAction();

				(d == String("rat")).AssertAction();
				string d_string = d.GetString();
				Assert::IsTrue(d_string == "rat");

				(c == String("dog")).AssertAction();
				string c_string = c.GetString();
				Assert::IsTrue(c_string == "dog");

				Assert::IsTrue(b.Evaluate().Equivalent(String("cat")));
				string b_string = b.GetString();
				Assert::IsTrue(b_string == "cat");
			}
			Cleanup();
		}

		TEST_METHOD(TestOr1)
		{
			Initialize();
			{
				Manager manager("Test Or 1", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(x);
				(x == Long(3)).AssertAction();
				string x_string = x.GetVariableValue().DisplayValueSN();

				Assert::IsTrue(x_string == "Long(3)");
				(x == Long(3)).EvaluateAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestOr2)
		{
			Initialize();
			{
				Manager manager("Test Or 2", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(x);
				(x == Long(3) || x == Long(4)).AssertAction();
				string x_string = x.DisplaySN();
				string x_valueset = x.Evaluate().DisplaySN();
				string x_buildset = x.BuildSet().Evaluate().DisplaySN();
				string t_valueset = (Long(3) || Long(4)).Evaluate().DisplaySN();
				string t_buildset = (Long(3) || Long(4)).BuildSet().Evaluate().DisplaySN();
				SN_DECLARE(y);
				(y == (Long(3) || Long(4))).AssertAction();
				string y_string = y.DisplaySN();
				string y_valueset = y.Evaluate().DisplaySN();
				string y_buildset = y.BuildSet().Evaluate().DisplaySN();

				(x.BuildSet() == y.BuildSet()).EvaluateAction();
				(x.BuildSet() == (Long(3) || Long(4)).BuildSet()).EvaluateAction();
				cout << x.DisplaySN();
			}
			Cleanup();
		}

		TEST_METHOD(TestOr3)
		{
			Initialize();
			{
				Manager manager("Test Or 3", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(x);
				(x == Long(3) || x == Long(4) || x == Long(5)).AssertAction();
				string x_string = x.DisplaySN();
				string x_valueset = x.Evaluate().DisplaySN();
				string x_buildset = x.BuildSet().Evaluate().DisplaySN();
				x.LogDisplaySN();
				x.Evaluate().LogDisplaySN();
				x.BuildSet().Evaluate().LogDisplaySN();

				(x.BuildSet() == (Long(3) || Long(4) || Long(5)).BuildSet()).EvaluateAction();
				cout << x.DisplaySN();
			}
			Cleanup();
		}

		TEST_METHOD(TestOr4)
		{
			Initialize();
			{
				Manager manager("Test Or 4", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(x);
				(x == Long(3) || x == Long(4) || x == Long(5) || x == Long(6)).AssertAction();
				string x_string = x.DisplaySN();
				string x_valueset = x.Evaluate().DisplaySN();
				string x_buildset = x.BuildSet().Evaluate().DisplaySN();

				(x.BuildSet() == (Long(3) || Long(4) || Long(5) || Long(6)).BuildSet()).EvaluateAction();
				cout << x.DisplaySN();
			}

			Cleanup();
		}

		TEST_METHOD(TestOr4WithCalc)
		{
			Initialize();
			{
				Manager manager("Test Or 4 With Calc", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(x);
				(x == Long(3)+Long(3) || x == Long(4)+Long(4) || x == Long(5)+Long(5) || x == Long(6)+Long(6)).AssertAction();
				string x_string = x.DisplaySN();
				string x_valueset = x.Evaluate().DisplaySN();
				string x_buildset = x.BuildSet().Evaluate().DisplaySN();

				(x.BuildSet() == (Long(3)+Long(3) || Long(4)+Long(4) || Long(5)+Long(5) || Long(6)+Long(6)).BuildSet()).EvaluateAction();
				cout << x.DisplaySN();
			}
			Cleanup();
		}

		TEST_METHOD(TestOrReversed4WithCalc)
		{
			Initialize();
			{
				Manager manager("Test Or Reversed 4 With Calc", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(x);
				(Long(3) + Long(3) == x || Long(4) + Long(4) == x || Long(5) + Long(5) == x || Long(6) + Long(6) == x).AssertAction();
				string x_string = x.DisplaySN();
				string x_valueset = x.Evaluate().DisplaySN();
				string x_buildset = x.BuildSet().Evaluate().DisplaySN();

				(x.BuildSet() == (Long(3) + Long(3) || Long(4) + Long(4) || Long(5) + Long(5) || Long(6) + Long(6)).BuildSet()).EvaluateAction();
				cout << x.DisplaySN();
			}
			Cleanup();
		}

		TEST_METHOD(TestOrReversed4WithSolve)
		{
			Initialize();
			{
				Manager manager("Test Or Reversed 4 With Solve", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(x);
				(Long(6) == x + Long(3) || Long(8) == x + Long(4) || Long(10) == x + Long(5) || Long(12) == x + Long(6)).AssertAction();
				string x_string = x.DisplaySN();
				string x_valueset = x.Evaluate().DisplaySN();
				string x_buildset = x.BuildSet().Evaluate().DisplaySN();

				(x.BuildSet() == (Long(3) || Long(4) || Long(5) || Long(6)).BuildSet()).EvaluateAction();
				cout << x.DisplaySN();
			}
			Cleanup();
		}

		TEST_METHOD(TestCharInValueSet)
		{
			Initialize();
			{
				Manager manager("Test Char In Value Set", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(B_Digit);
				SN_DECLARE(d);

				(Define(B_Digit)(d) == (d == (String("0") || String("1") || String("2") || String("3") || String("4")
					|| String("5") || String("6") || String("7") || String("8") || String("9")))).PartialAssertAction();

				std::cout << std::endl << "Function " << B_Digit.DisplaySN() << std::endl;
				(B_Digit(String("0"))).AssertAction();
				(B_Digit(String("1"))).AssertAction();
				(B_Digit(String("2"))).AssertAction();
				(B_Digit(String("3"))).AssertAction();
				(B_Digit(String("4"))).AssertAction();
				(B_Digit(String("5"))).AssertAction();
				(B_Digit(String("6"))).AssertAction();
				(B_Digit(String("7"))).AssertAction();
				(B_Digit(String("8"))).AssertAction();
				(B_Digit(String("9"))).AssertAction();

				(!B_Digit(String("X"))).AssertAction();

				// These return a value set of bools.  Should this reduce to a boolean value???

				(B_Digit(String("0"))).EvaluateAction();
				(B_Digit(String("1"))).EvaluateAction();
				(B_Digit(String("2"))).EvaluateAction();
				(B_Digit(String("3"))).EvaluateAction();
				(B_Digit(String("4"))).EvaluateAction();
				(B_Digit(String("5"))).EvaluateAction();
				(B_Digit(String("6"))).EvaluateAction();
				(B_Digit(String("7"))).EvaluateAction();
				(B_Digit(String("8"))).EvaluateAction();
				(B_Digit(String("9"))).EvaluateAction();

				(!B_Digit(String("X"))).EvaluateAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestCharInSet)
		{
			Initialize();
			{
				Manager manager("Test Char In Set", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(B_Digit2);
				SN_DECLARE(d);

				(Define(B_Digit2)(d) == ((String("0") || String("1") || String("2") || String("3") || String("4")
					|| String("5") || String("6") || String("7") || String("8") || String("9")).BuildSet().HasMember(d))).PartialAssertAction();

				std::cout << std::endl << "Function " << B_Digit2.DisplaySN() << std::endl;
				B_Digit2(String("0")).AssertAction();
				B_Digit2(String("1")).AssertAction();
				B_Digit2(String("2")).AssertAction();
				B_Digit2(String("3")).AssertAction();
				B_Digit2(String("4")).AssertAction();
				B_Digit2(String("5")).AssertAction();
				B_Digit2(String("6")).AssertAction();
				B_Digit2(String("7")).AssertAction();
				B_Digit2(String("8")).AssertAction();
				B_Digit2(String("9")).AssertAction();

				(!(B_Digit2(String("X")))).AssertAction();

				(B_Digit2(String("0"))).EvaluateAction();
				(B_Digit2(String("1"))).EvaluateAction();
				(B_Digit2(String("2"))).EvaluateAction();
				(B_Digit2(String("3"))).EvaluateAction();
				(B_Digit2(String("4"))).EvaluateAction();
				(B_Digit2(String("5"))).EvaluateAction();
				(B_Digit2(String("6"))).EvaluateAction();
				(B_Digit2(String("7"))).EvaluateAction();
				(B_Digit2(String("8"))).EvaluateAction();
				(B_Digit2(String("9"))).EvaluateAction();

				(!B_Digit2(String("X"))).EvaluateAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestIsInteger)
		{
			Initialize();
			{
				Manager manager("Test Is Integer", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(Digit);
				SN_DECLARE(d);
				SN_DECLARE(i);

				(Define(Digit)(d) == (d == (String("0") || String("1") || String("2") || String("3") || String("4")
					|| String("5") || String("6") || String("7") || String("8") || String("9")))).PartialAssertAction();
				Digit.LogDisplaySN();

				SN_DECLARE(IsInteger);
				(Define(IsInteger)(i) == (Digit(i) && !Digit(i.LookaheadRight())).Collapse().If(Bool(true), Digit(i.SelectLeftChar()) && IsInteger(i.SubtractLeftChar()))).PartialAssertAction();
				IsInteger.LogDisplaySN();

				//(IsInteger(String("1")).AssertAction());
				(IsInteger(String("12")).AssertAction());
				(IsInteger(String("123456789")).AssertAction());

				SN_DECLARE(a);
				SN_DECLARE(b);
				(a + b == "1abc").AssertAction();
				IsInteger(a).AssertAction();
				string test_a = a.GetString();
				string test_b = b.GetString();

				SN_DECLARE(a1);
				SN_DECLARE(b1);
				(a1 + b1 == "12abc").AssertAction();
				IsInteger(a1).AssertAction();
				string test_a1 = a1.GetString();
				string test_b1 = b1.GetString();

				SN_DECLARE(a2);
				SN_DECLARE(b2);
				(a2 + b2 == "123456789abc").AssertAction();
				//IsInteger(a2).AssertAction();
				string test_a2 = a2.GetString();
				string test_b2 = b2.GetString();

				SN_DECLARE(PmGenInteger);
				SN_DECLARE(s);
				//(Define(PmGenInteger)(d) == IsInteger(d) && .If(IsInteger(d), d.IntToString())).PartialAssertAction();
				//(Define(PmGenInteger)(s.StringToInt())) == IsInteger(s).If(IsInteger(d), d)).PartialAssertAction();
				//(Define(PmGenInteger)(d) == ((d = Long(s)) && IsInteger(s)).If(d)).PartialAssertAction();
				//(Define(PmGenInteger)(d) == ((s == d.IntToString()) && IsInteger(s)).If(s)).PartialAssertAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestParser)
		{
			Initialize();
			{
				Manager manager("Test Parser", AssertErrorHandler);
			}
			Cleanup();
		}

		TEST_METHOD(TestMetaEvaluate)
		{
			Initialize();
			{
				Manager manager("Test Meta Evaluate", AssertErrorHandler);

				SN_DECLARE_VALUE(dog, String("Vincent "));
				SN_DECLARE_VALUE(cat, String("Emma "));

				Assert::IsTrue(Meta(1, String("Vincent ") + String("Emma ")).Evaluate().Equivalent(Meta(1, Function(Function(Add, String("Vincent ")), String("Emma ")))));
				Assert::IsTrue(Meta(1, dog + cat).Evaluate().Equivalent(Meta(1, Function(Function(Add, dog), cat))));

				std::cout << std::endl << Meta(1, Meta(-1, dog) + Meta(-1, cat)).Evaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(1, Meta(-1, dog) + Meta(-1, cat)).Evaluate().Equivalent(Meta(1, String("Vincent ") + String("Emma "))));

				std::cout << std::endl << Meta(-1, Meta(1, Meta(-1, dog) + Meta(-1, cat)).Evaluate()).Evaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(-1, Meta(1, Meta(-1, dog) + Meta(-1, cat)).Evaluate()).Evaluate().Equivalent(String("Vincent Emma ")));

				std::cout << std::endl << Meta(1, Meta(-1, dog)).Evaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(1, Meta(-1, dog)).Evaluate().Equivalent(Meta(1, dog.Evaluate())));

				SN_DECLARE_VALUE(e, Meta(2, Meta(-1, dog) + Meta(-1, cat)));

				std::cout << std::endl << e.Evaluate().DisplaySN() << std::endl;

				Assert::IsTrue(e.Evaluate().Equivalent(Meta(2, Meta(-1, dog) + Meta(-1, cat))));

				std::cout << std::endl << Meta(-1, e).Evaluate().DisplaySN() << std::endl;


				Assert::IsTrue(Meta(-1, e).Evaluate().Equivalent(Meta(1, String("Vincent ") + String("Emma "))));
				Assert::IsTrue(e.Evaluate(-1).Equivalent(Meta(1, String("Vincent ") + String("Emma "))));

				std::cout << std::endl << Meta(-1, Meta(-1, e).Evaluate()).Evaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(-1, Meta(-1, e).Evaluate()).Evaluate().Equivalent(String("Vincent Emma ")));
				Assert::IsTrue(e.Evaluate(-1).Evaluate(-1).Equivalent(String("Vincent Emma ")));

				Assert::IsTrue(Meta(1, Meta(-1, dog)).Evaluate().Equivalent(Meta(1, dog.Evaluate())));
			}
			Cleanup();
		}

		TEST_METHOD(TestMetaPartialEvaluateOnValues)
		{
			Initialize();
			{
				Manager manager("Test Meta Partial Evaluate On Values", AssertErrorHandler);

				SN_DECLARE_VALUE(dog, String("Vincent "));
				SN_DECLARE_VALUE(cat, String("Emma "));

				Assert::IsTrue(Meta(1, String("Vincent ") + String("Emma ")).PartialEvaluate().Equivalent(Meta(1, Function(Function(Add, String("Vincent ")), String("Emma ")))));

				std::cout << std::endl << Meta(1, dog + cat).PartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(1, dog + cat).PartialEvaluate().Equivalent(Meta(1, Function(Function(Add, dog), cat))));

				std::cout << std::endl << Meta(1, Meta(-1, dog) + Meta(-1, cat)).PartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(1, Meta(-1, dog) + Meta(-1, cat)).PartialEvaluate().Equivalent(Meta(1, String("Vincent ") + String("Emma "))));

				std::cout << std::endl << Meta(-1, Meta(1, Meta(-1, dog) + Meta(-1, cat)).PartialEvaluate()).PartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(-1, Meta(1, Meta(-1, dog) + Meta(-1, cat)).PartialEvaluate()).PartialEvaluate().Equivalent(String("Vincent Emma ")));

				std::cout << std::endl << Meta(1, Meta(-1, dog)).PartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(1, Meta(-1, dog)).PartialEvaluate().Equivalent(Meta(1, dog.PartialEvaluate())));

				SN_DECLARE_VALUE(e, Meta(2, Meta(-1, dog) + Meta(-1, cat)));

				std::cout << std::endl << e.PartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(e.PartialEvaluate().Equivalent(Meta(2, Meta(-1, dog) + Meta(-1, cat))));

				std::cout << std::endl << Meta(-1, e).PartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(-1, e).PartialEvaluate().Equivalent(Meta(1, String("Vincent ") + String("Emma "))));
				Assert::IsTrue(e.PartialEvaluate(-1).Equivalent(Meta(1, String("Vincent ") + String("Emma "))));

				std::cout << std::endl << Meta(-1, Meta(-1, e).PartialEvaluate()).PartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(-1, Meta(-1, e).PartialEvaluate()).PartialEvaluate().Equivalent(String("Vincent Emma ")));
				Assert::IsTrue(e.PartialEvaluate(-1).PartialEvaluate(-1).Equivalent(String("Vincent Emma ")));
			}
			Cleanup();
		}

		TEST_METHOD(TestMetaPartialEvaluate)
		{
			Initialize();
			{
				Manager manager("Test Meta Partial Evaluate", AssertErrorHandler);

				SN_DECLARE(dog);
				SN_DECLARE(cat);

				Assert::IsTrue(Meta(1, String("Vincent ") + String("Emma ")).PartialEvaluate().Equivalent(Meta(1, Function(Function(Add, String("Vincent ")), String("Emma ")))));

				std::cout << std::endl << Meta(1, dog + cat).PartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(1, dog + cat).PartialEvaluate().Equivalent(Meta(1, Function(Function(Add, dog), cat))));

				std::cout << std::endl << Meta(1, Meta(-1, dog) + Meta(-1, cat)).PartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(1, Meta(-1, dog) + Meta(-1, cat)).PartialEvaluate().Equivalent(Meta(1, dog + cat)));

				std::cout << std::endl << Meta(-1, Meta(1, Meta(-1, dog) + Meta(-1, cat)).PartialEvaluate()).PartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(-1, Meta(1, Meta(-1, dog) + Meta(-1, cat)).PartialEvaluate()).PartialEvaluate().Equivalent(dog + cat));

				std::cout << std::endl << Meta(1, Meta(-1, dog)).PartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(1, Meta(-1, dog)).PartialEvaluate().Equivalent(Meta(1, dog.PartialEvaluate())));

				SN_DECLARE_VALUE(e, Meta(2, Meta(-1, dog) + Meta(-1, cat)));

				std::cout << std::endl << e.PartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(e.PartialEvaluate().Equivalent(Meta(2, Meta(-1, dog) + Meta(-1, cat))));

				std::cout << std::endl << Meta(-1, e).PartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(-1, e).PartialEvaluate().Equivalent(Meta(1, dog + cat)));
				Assert::IsTrue(e.PartialEvaluate(-1).Equivalent(Meta(1, dog + cat)));

				std::cout << std::endl << Meta(-1, Meta(-1, e).PartialEvaluate()).PartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(-1, Meta(-1, e).PartialEvaluate()).PartialEvaluate().Equivalent(dog + cat));
				Assert::IsTrue(e.PartialEvaluate(-1).PartialEvaluate(-1).Equivalent(dog + cat));
			}
			Cleanup();
		}

		TEST_METHOD(TestValueSetEvaluate)
		{
			Initialize();
			{
				Manager manager("Test Value Set Evaluate", AssertErrorHandler);

				// || operator applied to values other than bool, creates a value set.
				// ValueSet has Constructors for up to 5 parameters.  This will be mainly for testing. 
				Assert::IsTrue((Long(5) || Long(7)).Evaluate().DoRemove(Long(5)).DoRemove(Long(7)).DoIsEmpty());

				Assert::IsTrue((String("dog") || String("cat")).Evaluate().DoRemove(String("dog")).DoRemove(String("cat")).DoIsEmpty());

				Assert::IsTrue((Char('d') || Char('c')).Evaluate().DoRemove(Char('d')).DoRemove(Char('c')).DoIsEmpty());

				Assert::IsTrue((((String("catdograt") || String("pigdograt")) - (String("dograt") || String("rat")))).Evaluate().DoRemove(String("cat")).DoRemove(String("pig")).DoRemove(String("catdog")).DoRemove(String("pigdog")).DoIsEmpty());


				Assert::IsTrue((((String("cat") || String("dog")) + (String("food") || String("awesomefood")))).Evaluate().DoRemove(String("catfood")).DoRemove(String("catawesomefood")).DoRemove(String("dogfood")).DoRemove(String("dogawesomefood")).DoIsEmpty());

				Assert::IsTrue((Double(5) || Double(7)).Evaluate().DoRemove(Double(5)).DoRemove(Double(7)).DoIsEmpty());

				Assert::IsTrue((((Double(5) || Double(7)) + (Double(8) || Double(9)))).Evaluate().DoRemove(Double(13)).DoRemove(Double(14)).DoRemove(Double(15)).DoRemove(Double(16)).DoIsEmpty());
				// The order does not matter
				Assert::IsTrue((Long(-4) || Long(4)).Evaluate().DoRemove(Long(4)).DoRemove(Long(-4)).DoIsEmpty());

				// Value sets behave like values.  an operator or function applied to value set, generates the cardesian product of the values, with the function applied.
				Assert::IsTrue((((Long(5) || Long(7)) + (Long(8) || Long(9)))).Evaluate().DoRemove(Long(13)).DoRemove(Long(14)).DoRemove(Long(15)).DoRemove(Long(16)).DoIsEmpty());

				//added for subtraction
				Assert::IsTrue((((Long(9) || Long(11)) - (Long(3) || Long(4)))).Evaluate().DoRemove(Long(6)).DoRemove(Long(5)).DoRemove(Long(8)).DoRemove(Long(7)).DoIsEmpty());

				Assert::IsTrue((((Long(1) || Long(2)) * (Long(3) || Long(4)))).Evaluate().DoRemove(Long(3)).DoRemove(Long(4)).DoRemove(Long(6)).DoRemove(Long(8)).DoIsEmpty());

				Assert::IsTrue((((Long(5) || Long(10)) / (Long(1) || Long(5)))).Evaluate().DoRemove(Double(5)).DoRemove(Double(1)).DoRemove(Double(10)).DoRemove(Double(2)).DoIsEmpty());

				// If the values are tagged with worlds from different world sets then all the values from the cardesian product are created. 
				// first    2 || -2 = [2:A, -2:B]
				// second   2 || -2 = [2:C, -2:D]
				// first+second = [2+2:AC, 0:AD, 0:BC, -2:BD]
				Assert::IsTrue((((Long(2) || Long(-2)) + (Long(2) || Long(-2)))).Evaluate().DoRemove(Long(4)).DoRemove(Long(0)).DoRemove(Long(-4)).DoIsEmpty());

				// If using the same value set twice then must use the same value each time.
				// This is because the intersection of different worlds from the same value set is always empty.
				// More explictly, each value in a vale set is tagged with a world from a world set.
				//     2 || -2 = [2:A, -2:B]
				// where A and B are worlds. Worlds are sets.
				// A intersection B is empty.
				SN_DECLARE_VALUE(two_or_minus_two, (Long(2) || Long(-2)).Evaluate());

				Assert::IsTrue((two_or_minus_two + two_or_minus_two).Evaluate().DoRemove(Long(4)).DoRemove(Long(-4)).DoIsEmpty());

				Assert::IsTrue(((Long(2) || Long(-2)) + (Long(2) || Long(-2))).Evaluate().DoRemove(Long(4)).DoRemove(Long(0)).DoRemove(Long(-4)).DoIsEmpty());

				(two_or_minus_two + two_or_minus_two).BuildSet().HasMember(Long(-4)).EvaluateAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestValueSetEquivalencyEvaluate)
		{
			Initialize();
			{
				Manager manager("Test Value Set Equivalency Evaluate", AssertErrorHandler);

				(((String("Dog") || String("Cat")) + String("Food")).BuildSet() == ((String("DogFood") || String("CatFood")).BuildSet())).EvaluateAction();

				(((String("Dog") || String("Cat")) + String("Food")).BuildSet() == ((String("CatFood") || String("DogFood")).BuildSet())).EvaluateAction();
				(((String("Dog") || String("Pig")) + String("Food")).BuildSet() == ((String("DogFood") || String("PigFood")).BuildSet())).EvaluateAction();

				(((Long(10) || Long(9)) + Long(1)).BuildSet() == (Long(11) || Long(10)).BuildSet()).EvaluateAction();

				(((Double(10) || Double(9)) + Double(1)).BuildSet() == (Double(11) || Double(10)).BuildSet()).EvaluateAction();


				// mixed value sets
				Assert::IsTrue((String("dog") || Long(9)).Evaluate().DoRemove(String("dog")).DoRemove(Long(9)).DoIsEmpty());

				//NESTED OR CASE
				Assert::IsTrue(((String("dog") || Long(9)) || String("Cat")).Evaluate().DoRemove(String("dog")).DoRemove(Long(9)).DoRemove(String("Cat")).DoIsEmpty());
			}
			Cleanup();
		}

		TEST_METHOD(TestSetEvaluate)
		{
			Initialize();
			{
				Manager manager("Test Set Evaluate", AssertErrorHandler);

				Expression a;
				(((Long(2) || Long(-2))*(Long(3) || Long(-3))).BuildSet() == ((Long(6) || Long(-6)).BuildSet())).EvaluateAction();

				((Long(4).SquareRoot()).BuildSet() == (Long(2) || Long(-2)).BuildSet()).EvaluateAction();
				string my_exp4 = (Long(4)).BuildSet().Evaluate().DisplaySN();
				std::cout << std::endl << ((Long(4)).BuildSet()).Evaluate().DisplaySN() << std::endl;
				Assert::IsTrue(((Long(4)).BuildSet()).Evaluate().DisplaySN() == "{Long(4)}");

				string my_exp5 = ((Long(2) || Long(-2)) + (Long(2) || Long(-2))).Evaluate().DisplaySN();
				Assert::IsTrue(((Long(2) || Long(-2)) + (Long(2) || Long(-2))).Evaluate().DoRemove(Long(4)).DoRemove(Long(0)).DoRemove(Long(-4)).DoIsEmpty());

				SN_DECLARE_VALUE(x, Long(2) || Long(-2));

				(((x < Long(0)).If(x || Long(5), Long(0))).BuildSet() == (Long(0) || Long(5) || Long(-2)).BuildSet()).EvaluateAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestSetAssert)
		{
			Initialize();
			{
				Manager manager("Test Set Assert", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(x);
				SN_DECLARE(z);
				(x.Square() == Long(4)).AssertAction();
				((x.BuildSet()) == (Long(2) || Long(-2)).BuildSet()).EvaluateAction();
				(z == (Long(2) || Long(-2))).AssertAction();

				// This is not true. The expression is a value set with true and false in it.
				//(x == (Long(2) || Long(-2))).EvaluateAction();

				SN_DECLARE(y);
				(y*y == Long(4)).AssertAction();   // recognise that y*y = y.Square() in the * operator in SNI_Expression.
				((y.BuildSet()) == (Long(2) || Long(-2)).BuildSet()).EvaluateAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestSubStrings)
		{
			Initialize();
			{
				Manager manager("Test Sub Strings", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				(String("catdog").SubtractRight(String("catdog").SubtractLeft(String("cat"))) == String("cat")).AssertAction();
				(String("catdog").SubtractLeft(String("catdog").SubtractRight(String("dog"))) == String("dog")).AssertAction();

				SN_DECLARE_VALUE(x, String("catdog"));
				SN_DECLARE_VALUE(y, String("cat"));
				SN_DECLARE_VALUE(z, String("dog"));

				(x.SubtractRight(x.SubtractLeft(y)) == y).AssertAction();
				(x.SubtractLeft(x.SubtractRight(z)) == z).AssertAction();

				(String("GamesX").SubtractRightChar() + String("GamesX").SelectRightChar() == String("GamesX")).AssertAction();
				SN_DECLARE_VALUE(m, String("GamesX"));
				(m.SubtractRightChar() + m.SelectRightChar() == m).AssertAction();

				(String("XGames").SelectLeftChar() + String("XGames").SubtractLeftChar() == String("XGames")).AssertAction();
				SN_DECLARE_VALUE(n, String("XGames"));
				(n.SelectLeftChar() + n.SubtractLeftChar() == n).AssertAction();
			}
			Cleanup();
		}

/*		TEST_METHOD(TestAssertNothing)
		{
			Initialize();
			{
				Manager manager("Test Assert Nothing", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer2);
			}
			Cleanup();
		}
*/

		TEST_METHOD(TestAssertThrow)
		{
			Initialize();
			{
				Manager manager("Test Assert Throw", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer2);
				{
					try
					{
						(Long(5) == Long(6)).AssertThrow();
						Assert::IsTrue(false, L"Expected a contradiction");
					}
					catch (Error e)
					{
						string description = e.GetDescription();
						Assert::IsTrue(e.IsError(), wstring(description.begin(), description.end()).c_str());
					}
				};
			}
			Cleanup();
		}

		TEST_METHOD(TestFileAccess)
		{
			Initialize();
			{
				string s_name_1 = "c:/temp/sn_test_testfileaccess_1.txt";
				string s_name_2 = "c:/temp/sn_test_testfileaccess_2.txt";
				string s_contents_1 = "Tora! Tora! Tora!";
				string s_contents_2 = "Two dogs barking!";

				remove(s_name_1.data());
				remove(s_name_2.data());
				{
					std::ofstream out(s_name_1);
					out << s_contents_1;
					out.close();
				}

				Manager manager("Test File Access", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer2);
				{
					Transaction transaction;

					SN_DECLARE_VALUE(name_1, String(s_name_1));
					SN_DECLARE_VALUE(contents_1, String(s_contents_1));

					(name_1.File() == contents_1).AssertAction();

					{
						std::ofstream out(s_name_1);
						out << s_contents_1;
						out.close();
					}

					(name_1.File() == contents_1).AssertAction();

					SN_DECLARE_VALUE(name_2, String(s_name_2));
					SN_DECLARE_VALUE(contents_2, String(s_contents_2));

					(name_2.File() == contents_2).AssertAction();

					ifstream inFile;
					inFile.open(s_name_2); // open the input file
					Assert::IsTrue(inFile.is_open());

					stringstream strStream;
					strStream << inFile.rdbuf(); // read the file
					Assert::IsTrue(strStream.str() == s_contents_2);

					{
						std::ofstream out(s_name_2);
						out << s_contents_1;
						out.close();
					}

					(name_2.File() == contents_2).AssertAction(); // The assert still hold. For consistency, SN caches the value for the file name.
				}

				// Outside the transaction, the cache is cleared and the value is re-read.
				{
					Transaction transaction;

					SN_DECLARE_VALUE(name_1, String(s_name_1));
					SN_DECLARE_VALUE(name_2, String(s_name_2));
					SN_DECLARE_VALUE(contents_1, String(s_contents_1));
					(name_1.File() == name_2.File()).AssertAction();
					(name_2.File() == contents_1).AssertAction();
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestValueSetOfLambdaFunctions)
		{
			Initialize();
			{
				Manager manager("Test Value Set Of Lambda Functions", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(plus);
				SN_DECLARE(times);

				{
					SN_DECLARE(a);
					SN_DECLARE(b);

					(Define(plus)(a)(b) == a + b).PartialAssertAction();

					SN_DECLARE(c);
					SN_DECLARE(d);

					(Define(times)(c)(d) == c * d).PartialAssertAction();
				}
				string s_plus = plus.DisplayValueSN();
				string s_times = times.DisplayValueSN();
				string s_plus_times = (plus || times).Evaluate().DisplaySN();
				string s_plus_times2 = (plus || times).Evaluate().DisplaySN();

				SN_DECLARE(f);
				(f == (plus || times)).AssertAction();
				string f_string = f.DisplaySN();

				SN_DECLARE(x);
				(x == f(long(5))(long(8))).AssertAction();
				string x_string = x.DisplaySN();
				(x < long(20)).AssertAction();
				(x == long(13)).AssertAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestValueSetOfStandardFunctions)
		{
			Initialize();
			{
				Manager manager("Test Value Set Of Standard Functions", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(f);
				(f == (skynet::Add || skynet::Multiply)).AssertAction();
				string f_string = f.DisplaySN();

				SN_DECLARE(x);
				(x == f(long(8))(long(13))).AssertAction();
				string x_string = x.BuildSet().Evaluate().DisplaySN();
				Assert::IsTrue(x_string == "{Long(21), Long(104)}");
				(x < long(30)).AssertAction();
				string x_string2 = x.DisplaySN();
				long x_num = Long(x).GetNumber();
				Assert::IsTrue(x_num == 21);
				(x == Long(21)).AssertAction();
			}
			Cleanup();
		}

		TEST_METHOD(TestValueSetOfStringFunctions)
		{
			Initialize();
			{
				Manager manager("Test Value Set Of String Functions", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer3);

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

				Assert::IsTrue(x_string == x_compare_string);
			}
			Cleanup();
		}

		TEST_METHOD(TestStringSearch)
		{
			Initialize();
			{
				Manager manager("Test String Search", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);
				{
					Transaction transaction;

					SN_DECLARE(money);
					SN_DECLARE(evil);
					(money + (String(" is the root of all ") + evil) == String("Money is the root of all evil")).AssertAction();
					string money_string = money.GetString();
					string evil_string = evil.GetString();
					Assert::IsTrue(money_string == "Money");
					Assert::IsTrue(evil_string == "evil");
				}
				{
					Transaction transaction;

					SN_DECLARE(money);
					SN_DECLARE(evil);
					(money + String(" is the root of all ") + evil == String("Money is the root of all evil")).AssertAction();
					string money_string = money.GetString();
					string evil_string = evil.GetString();
					Assert::IsTrue(money_string == "Money");
					Assert::IsTrue(evil_string == "evil");
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestStringSearchExample1)
		{
			Initialize();
			{
				Manager manager("Test String Search Example 1", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(firstname1);
				SN_DECLARE(surname1);
				(String("My first name is ") + (firstname1 + String(" and my surname is Fischer.")) == String("My first name is Bobby and my surname is Fischer.")).AssertAction();
				string firstname1_string = firstname1.GetString();
				Assert::IsTrue(firstname1_string == "Bobby");

				SN_DECLARE(firstname2);
				SN_DECLARE(surname2);
				(String("My first name is ") + (firstname2 + (String(" and my surname is ") + surname2)) == String("My first name is Bobby and my surname is Fischer")).AssertAction();
				string firstname2_string = firstname2.GetString();
				string surname2_string = surname2.GetString();
				Assert::IsTrue(firstname2_string == "Bobby");
				Assert::IsTrue(surname2_string == "Fischer");
			}
			Cleanup();
		}

		TEST_METHOD(TestStringSearchExample2)
		{
			Initialize();
			{
				Manager manager("Test String Search Example 2", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);
				{
					Transaction transaction;

					SN_DECLARE(firstname);
					SN_DECLARE(surname);

					(String("My first name is ") + (firstname + (String(" and my surname is ") + (surname + String(".")))) == String("My first name is Bobby and my surname is Fischer.")).AssertAction();

					string firstname_string = firstname.GetString();
					string surname_string = surname.GetString();
					Assert::IsTrue(firstname_string == "Bobby");
					Assert::IsTrue(surname_string == "Fischer");

					(firstname == String("Bobby")).AssertAction();
					(surname == String("Fischer")).AssertAction();
				}
				{
					Transaction transaction;

					SN_DECLARE(firstname);
					SN_DECLARE(surname);

					(String("My first name is ") + firstname + String(" and my surname is ") + (surname + String(".")) == String("My first name is Bobby and my surname is Fischer.")).AssertAction();
					(firstname == String("Bobby")).AssertAction();
					(surname == String("Fischer")).AssertAction();
					string firstname_string = firstname.GetString();
					string surname_string = surname.GetString();
					Assert::IsTrue(firstname_string == "Bobby");
					Assert::IsTrue(surname_string == "Fischer");
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestStringAmbiguity)
		{
			Initialize();
			{
				Manager manager("Test String Ambiguity", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(firstClause);
				SN_DECLARE(secondClause);
				SN_DECLARE(sentence);
				(firstClause + String(" and ") + secondClause == String("I love dogs and cats and dogs love me.")).AssertAction();
				string firstClause_string_vs = firstClause.DisplaySN();
				string secondClause_string_vs = secondClause.DisplaySN();
				
				string firstClause_string = firstClause.BuildSet().Evaluate().DisplaySN();
				string secondClause_string = secondClause.BuildSet().Evaluate().DisplaySN();
				Assert::IsTrue(firstClause_string == "{String(\"I love dogs\"), String(\"I love dogs and cats\")}");
				Assert::IsTrue(secondClause_string == "{String(\"cats and dogs love me.\"), String(\"dogs love me.\")}");
				
				(sentence == firstClause + (String(" and ") + secondClause)).AssertAction();
				string sentence_string_vs = sentence.DisplaySN();
				string sentence_string = sentence.GetString();
				Assert::IsTrue(sentence_string == "I love dogs and cats and dogs love me.");
			}
			Cleanup();
		}
		
		TEST_METHOD(TestPythagoras)
		{
			Initialize();
			{
				Manager manager("Test Pythagoras", AssertErrorHandler);
				manager.StartWebServer(SN::StepInto, "0.0.0.0", "80", "C:/sn/html", runWebServer);

				SN_DECLARE(X);
				(Double(245.67).Square() + X.Square() == Double(357.56).Square()).AssertAction();
				string X_string = X.BuildSet().Evaluate().DisplaySN();
				Assert::IsTrue(X_string == "{Double(259.798777), Double(-259.798777)}");
			}
			Cleanup();
		}
	};
}