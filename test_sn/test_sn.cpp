#include "sn.h"

#include <cstdio>
#include <fstream>

#include "test_sn_pch.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace std;
using namespace skynet;
using namespace PGCX;

bool runWebServer = false;
bool runWebServer2 = false;
bool runWebServer3 = false;

string doc_root = "C:/Users/peter_driscoll/Documents/Source/Repos/skynet2/html";

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
			Manager::LogicSetup();
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
				// manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

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

				(String("dog") == String("dog")).Evaluate().Do();
				(!(String("dog") == String("cat"))).Evaluate().Do();

				SN_DECLARE_VALUE(z, String("dog"));
				(String("dog") == z).Evaluate().Do();
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

				Assert::IsTrue((String("dog") == y).DoPartialEvaluate().Equivalent(String("dog") == y));
				Assert::IsTrue(!(String("dog") == y).DoPartialEvaluate().Equivalent(String("dog") == x));
				Assert::IsTrue((String("dog") == String("dog")).DoPartialEvaluate().Equivalent(Bool(true)));
				(String("dog") == z).DoPartialEvaluate().Do();

				Assert::IsTrue((String("dog") + y == String("dogcat")).DoPartialEvaluate().Equivalent(Function(Function(Equals, Function(Function(Add, String("dog")), y)), String("dogcat"))));
				(String("dog") + z == String("dogdog")).DoPartialEvaluate().Do();

				(String("dog") + z == "dogdog").DoPartialEvaluate().Do();
				(String("dog") + "dog" == "dogdog").DoPartialEvaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestStringAssert)
		{
			Initialize();
			{
				Manager manager("Test String Assert", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				{
					Transaction transaction;

					SN_DECLARE(y);

					(String("dog") == y).Assert().Do();
					(String("dog") == y).Evaluate().Do();

					SN_DECLARE(z);

					(String("dog") + z == "dogcat").Assert().Do();
					Value cat_value = z.GetValue();
					string s_cat_value = cat_value.GetString();
					Assert::IsTrue(s_cat_value == "cat");
					(String("cat") == z).Evaluate().Do();
				}
				{
					Transaction transaction;

					SN_DECLARE(a);
					(String("dog") + a == String("dogcat")).Assert().Do();
					(a == String("cat")).Assert().Do();
					SN_DECLARE(b);
					(b + String("cat") == String("dogcat")).Assert().Do();
					(b == String("dog")).Assert().Do();
					SN_DECLARE(c);
					(String("dog") + String("cat") == c).Assert().Do();
					(c == String("dogcat")).Assert().Do();
				}
				{
					Transaction transaction;

					SN_DECLARE(a);
					(String("dogcat").SubtractRight(String("cat")) == a).Assert().Do();
					(a == String("dog")).Assert().Do();
					SN_DECLARE(b);
					(String("dogcat").SubtractRight(b) == String("dog")).Assert().Do();
					(b == String("cat")).Assert().Do();
					SN_DECLARE(c);
					(c.SubtractRight(String("cat")) == String("dog")).Assert().Do();
					(c == String("dogcat")).Assert().Do();
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

					(Bool(true) == Bool(true)).Evaluate().Do();
					(!(Bool(true) == Bool(false))).Evaluate().Do();
				}
				{
					Transaction transaction;

					SN_DECLARE_VALUE(z, Bool(true));
					(Bool(true) == z).Evaluate().Do();

					SN_DECLARE_VALUE(k, True);
					SN_DECLARE_VALUE(l, False);
					SN_DECLARE_VALUE(m, True);
					(k.If(l, m) == False).Evaluate().Do();
					(True.If("dog", "cat") == "dog").Evaluate().Do();
					(False.If("dog", "cat") == "cat").Evaluate().Do();
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
					(!(Bool(false) && Bool(false))).Evaluate().Do();
					(!(Bool(false) && Bool(true))).Evaluate().Do();
					(!(Bool(true) && Bool(false))).Evaluate().Do();
					(Bool(true) && Bool(true)).Evaluate().Do();
				}
				{
					Transaction transaction;

					// Truth table OR
					(!(Bool(false) || Bool(false))).Evaluate().Do();
					(Bool(false) || Bool(true)).Evaluate().Do();
					(Bool(true) || Bool(false)).Evaluate().Do();
					(Bool(true) || Bool(true)).Evaluate().Do();
				}
				{
					Transaction transaction;

					// Truth table AND
					((Bool(false) && Bool(false)) == Bool(false)).Evaluate().Do();
					((Bool(false) && Bool(true)) == Bool(false)).Evaluate().Do();
					((Bool(true) && Bool(false)) == Bool(false)).Evaluate().Do();
					((Bool(true) && Bool(true)) == Bool(true)).Evaluate().Do();
				}
				{
					Transaction transaction;

					// Truth table OR
					((Bool(false) || Bool(false)) == Bool(false)).Evaluate().Do();
					((Bool(false) || Bool(true)) == Bool(true)).Evaluate().Do();
					((Bool(true) || Bool(false)) == Bool(true)).Evaluate().Do();
					((Bool(true) || Bool(true)) == Bool(true)).Evaluate().Do();
				}
				{
					Transaction transaction;

					// Truth table if A then B else C
					(False.If(False, False) == False).Evaluate().Do();
					(False.If(False, True) == True).Evaluate().Do();
					(False.If(True, False) == False).Evaluate().Do();
					(False.If(True, True) == True).Evaluate().Do();
					(True.If(False, False) == False).Evaluate().Do();
					(True.If(False, True) == False).Evaluate().Do();
					(True.If(True, False) == True).Evaluate().Do();
					(True.If(True, True) == True).Evaluate().Do();
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


				Assert::IsTrue((Bool(true) == y).DoPartialEvaluate().Equivalent(Bool(true) == y));
				Assert::IsTrue(!(Bool(true) == y).DoPartialEvaluate().Equivalent(Bool(true) == x));
				Assert::IsTrue((Bool(true) == Bool(true)).DoPartialEvaluate().Equivalent(Bool(true)));
				(Bool(true) == z).DoPartialEvaluate().Do();

				Assert::IsTrue((Bool(false) || y).DoPartialEvaluate().Equivalent(y));
				Assert::IsTrue((y || Bool(false)).DoPartialEvaluate().Equivalent(y));
				Assert::IsTrue((Bool(true) || y).DoPartialEvaluate().Equivalent(Bool(true)));
				Assert::IsTrue((y || Bool(true)).DoPartialEvaluate().Equivalent(Bool(true)));

				Assert::IsTrue((Bool(true) && y).DoPartialEvaluate().Equivalent(y));
				Assert::IsTrue((y && Bool(true)).DoPartialEvaluate().Equivalent(y));
				Assert::IsTrue((Bool(false) && y).DoPartialEvaluate().Equivalent(Bool(false)));
				Assert::IsTrue((y && Bool(false)).DoPartialEvaluate().Equivalent(Bool(false)));

				(Bool(false) || z).DoPartialEvaluate().Do();
				(Bool(true) && true).DoPartialEvaluate().Do();

				(Bool(true) && true).DoPartialEvaluate().Do();

				SN_DECLARE(i);
				(i == i).DoPartialEvaluate().Do();

				// If

				SN_DECLARE(k);
				SN_DECLARE(l);

				Assert::IsTrue((k.If(False, True) == l).DoPartialEvaluate().Equivalent((!k == l).DoPartialEvaluate()));
				Assert::IsTrue((k.If(True, False) == l).DoPartialEvaluate().Equivalent((k == l).DoPartialEvaluate()));
				Assert::IsTrue(k.If(True, True).DoPartialEvaluate().Equivalent(True));
				Assert::IsTrue(k.If(False, False).DoPartialEvaluate().Equivalent(False));
				Assert::IsTrue(True.If(k, l).DoPartialEvaluate().Equivalent(k));
				Assert::IsTrue(False.If(k, l).DoPartialEvaluate().Equivalent(l));

				Assert::IsTrue(True.If(True, False).DoPartialEvaluate().Equivalent(True));

				// Not
				Assert::IsTrue((!!k).DoPartialEvaluate().Equivalent(k));
				Assert::IsTrue((!(k.If(False, True))).DoPartialEvaluate().Equivalent(k));
			}
			Cleanup();
		}

		TEST_METHOD(TestBoolAssert)
		{
			Initialize();
			{
				Manager manager("Test Bool Assert", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(z);

				(z || Bool(false) == Bool(true)).Assert().Do();

				(Bool(true) == z).Evaluate().Do();
				(!(Bool(false) && z)).DoPartialEvaluate().Do();
				(Bool(true) && Bool(true)).DoPartialEvaluate().Do();


				SN_DECLARE(y);

				(Bool(true) == y.Debug()).Assert().Do();
				(Bool(true) == y).Evaluate().Do();

				SN_DECLARE(k1);
				SN_DECLARE_VALUE(l1, False);

				(k1.Debug().If(False, True) == l1).Assert().Do();
				(k1 == !l1).Evaluate().Do();

				SN_DECLARE(k2);
				SN_DECLARE_VALUE(l2, True);

				(k2.If(False, True) == l2).Assert().Do();
				(k2 == !l2).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestBoolAssertIf)
		{
			Initialize();
			{
				Manager manager("Test Bool Assert If", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(m);
				SN_DECLARE(n);
				SN_DECLARE(o);

				(o.If(False, True) == False).Assert().Do();
				o.Evaluate().Do();
				(o.If(m, n) == True).Assert().Do();
				m.Evaluate().Do();
				((!o).If(m, n) == m).Assert().Do();
				n.Evaluate().Do();
			}
			Cleanup();
		}
		TEST_METHOD(TestBoolNotAndNot)
		{
			Initialize();
			{
				Manager manager("Test Bool Not And Not", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				(!((Bool(true) && Bool(false)))).Assert().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestDoubleEquivalent)
		{
			Initialize();
			{
				Manager manager("Test Double Equivalent", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				((Long(1) + Long(2)) == Long(3)).Evaluate().Do();

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

				(Double(3.3) == Double(3.3)).Evaluate().Do();
				(!(Double(5.43) == Double(6))).Evaluate().Do();

				SN_DECLARE_VALUE(z, Double(2.2));
				(Double(3.5) + Double(5.6) == Double(9.1)).Evaluate().Do();
				(Double(2.2) == z).Evaluate().Do();

				SN_DECLARE_VALUE(x, Double(3));
				(z * x == Double(6.6)).Evaluate().Do();

				SN_DECLARE_VALUE(y, Double(2));
				((z * x) / y == Double(3.3)).Evaluate().Do();
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


				Assert::IsTrue((Double(3.1415) == y).DoPartialEvaluate().Equivalent(Double(3.1415) == y));
				Assert::IsTrue(!(Double(9.7) == y).DoPartialEvaluate().Equivalent(Double(9.7) == x));
				Assert::IsTrue((Double(9.7) == Double(9.7)).DoPartialEvaluate().Equivalent(Bool(true)));
				(Double(3.1415) == z).DoPartialEvaluate().Do();

				Assert::IsTrue((Double(5.5) + y == Double(10)).DoPartialEvaluate().Equivalent(Equals(Add(Double(5.5))(y))(Double(10))));
				(Double(8.0) + z == Double(11.1415)).DoPartialEvaluate().Do();

				(Double(3.1415) + z == Double(3.1415)*Double(2.0)).DoPartialEvaluate().Do();
				(Double(4.5) + 4.5 == 9.0).DoPartialEvaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestDoubleAssert)
		{
			Initialize();
			{
				Manager manager("Test Double Assert", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(y);

				(Double(45.6) == y).Assert().Do();
				(Double(45.6) == y).Evaluate().Do();

				SN_DECLARE(z);

				(Double(45.6) + z == 100.0).Assert().Do();

				(Double(54.4) == z).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestLongEvaluate)
		{
			Initialize();
			{
				Manager manager("Test Long Evaluate", AssertErrorHandler);

				(Long(4) * Double(1.25) == Double(5)).Evaluate().Do();
				(Double(5) / Long(4) == Double(1.25)).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestLongAssertSimplified)
		{
			Initialize();
			{
				Manager manager("Test Long Assert Simplified", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(c);

				(Long(2) + Long(3) == c).Assert().Do();

				Assert::IsTrue(c.Equivalent(Long(5)));
			}
			Cleanup();
		}
		
		TEST_METHOD(TestLongAssert)
		{
			Initialize();
			{
				Manager manager("Test Long Assert", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				{
					Transaction transaction;

					SN_DECLARE(a);
					SN_DECLARE(b);
					SN_DECLARE(c);

					(a + Long(3) == Long(5)).Assert().Do();
					(Long(2) + b == Long(5)).Assert().Do();
					(Long(2) + Long(3) == c).Assert().Do();

					Assert::IsTrue(a.Equivalent(Long(2)));
					Assert::IsTrue(b.Equivalent(Long(3)));
					Assert::IsTrue(c.Equivalent(Long(5)));
				}

				{
					Transaction transaction;

					SN_DECLARE(x);
					SN_DECLARE(y);
					SN_DECLARE(z);

					(x - Long(3) == Long(2)).Assert().Do();
					(Long(5) - y == Long(2)).Assert().Do();
					(Long(5) - Long(3) == z).Assert().Do();

					Assert::IsTrue(x.Equivalent(Long(5)));
					Assert::IsTrue(y.Equivalent(Long(3)));
					Assert::IsTrue(z.Equivalent(Long(2)));
				}

				{
					Transaction transaction;

					SN_DECLARE(a);
					SN_DECLARE(b);
					SN_DECLARE(c);

					(a * Double(1.25) == Long(5)).Assert().Do();
					(Long(4) * b == Double(5)).Assert().Do();
					(Long(4) * Double(1.25) == c).Assert().Do();

					Assert::IsTrue(a.Equivalent(Double(4)));
					Assert::IsTrue(b.Equivalent(Double(1.25)));
					Assert::IsTrue(c.Equivalent(Double(5)));
				}

				{
					Transaction transaction;

					SN_DECLARE(x);
					SN_DECLARE(y);
					SN_DECLARE(z);

					(x / Long(4) == Double(1.25)).Assert().Do();
					(Long(5) / y == Double(1.25)).Assert().Do();
					(Double(5) / Long(4) == z).Assert().Do();

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
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				{
					SN_DECLARE(RemovePrefix);
					SN_DECLARE(p);
					SN_DECLARE(x);

					(Define(RemovePrefix) == Lambda(p, Lambda(x, x.SubtractLeft(p)))).PartialAssert().Do();
					std::cout << std::endl << "Variable " << RemovePrefix.DisplaySN() << std::endl;
					string s_RemovePrefix = RemovePrefix.GetVariableValue().DisplaySN();
					Assert::IsTrue(s_RemovePrefix == "@p.@x.SubtractLeft x p");
					string v_RemovePrefix = Lambda(p, Lambda(x, x.SubtractLeft(p))).DoPartialEvaluate().DisplaySN();
					Assert::IsTrue(v_RemovePrefix == "@p.@x.SubtractLeft x p");
					Assert::IsTrue(RemovePrefix.DoPartialEvaluate().Equivalent(Lambda(p, Lambda(x, x.SubtractLeft(p))).DoPartialEvaluate()));
					(RemovePrefix(String("Atl"))(String("AtlDog")) == String("Dog")).Evaluate().Do();
				}
				{
					SN_DECLARE(RemovePostfix);
					SN_DECLARE(p);
					SN_DECLARE(x);

					(Define(RemovePostfix) == Lambda(p, Lambda(x, x.SubtractRight(p)))).PartialAssert().Do();
					Assert::IsTrue(RemovePostfix.GetVariableValue().DisplaySN() == "@p.@x.SubtractRight x p");
					std::cout << std::endl << "Variable " << RemovePostfix.DisplaySN() << std::endl;
					Assert::IsTrue(RemovePostfix.DoPartialEvaluate().Equivalent(Lambda(p, Lambda(x, x.SubtractRight(p))).DoPartialEvaluate()));
					(RemovePostfix(String("Atl"))(String("DogAtl")) == String("Dog")).Evaluate().Do();
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestRecursiveLambdaFunctionDefinition)
		{
			Initialize();
			{
				Manager manager("Test Function Definition", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(Factorial);
				SN_DECLARE(n);

				(Define(Factorial) == Lambda(n, (n == Long(0)).If(Long(1), n * Factorial(n - Long(1))))).PartialAssert().Do();
				std::cout << std::endl << "Function " << Factorial.DisplaySN() << std::endl;
				(Factorial(Long(3)) == Long(6)).Evaluate().Do();
				(Factorial(Long(10)) == Long(3628800)).Evaluate().Do();
				(Factorial(Long(12)) == Long(479001600)).Evaluate().Do();

				SN_DECLARE(Fact);
				SN_DECLARE(m);

				(Define(Fact)(m) == (m == Long(0)).If(Long(1), m * Fact(m - Long(1)))).PartialAssert().Do();
				std::cout << std::endl << "Function " << Fact.DisplaySN() << std::endl;
				(Fact(Long(3)) == Long(6)).Evaluate().Do();
				(Fact(Long(10)) == Long(3628800)).Evaluate().Do();
				(Fact(Long(12)) == Long(479001600)).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestRecursiveFunctionDefinition)
		{
			Initialize();
			{
				Manager manager("Test Recursive Function Definition", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				{
					Transaction transaction;

					SN_DECLARE(Fact);
					SN_DECLARE(m);

					(Define(Fact)(m) == (m == Long(0)).If(Long(1), m * Fact(m - Long(1)))).PartialAssert().Do();

					(Fact(Long(0)) == Long(1)).Assert().Do();
					(Fact(Long(1)) == Long(1)).Assert().Do();
					(Fact(Long(3)) == Long(6)).Assert().Do();
					(Fact(Long(10)) == Long(3628800)).Assert().Do();
					(Fact(Long(12)) == Long(479001600)).Assert().Do();
				}
				{
					Transaction transaction;

					SN_DECLARE(Fact);
					SN_DECLARE(k);
					SN_DECLARE(n);
					// Fact 0 == 1 && Fact k : > 0 == k * Fact k-1 : < 0
					((Fact(Long(0)) == Long(1)) && (Fact(k, Lambda(n, n > Long(0))) == k * Fact(k - Long(1), Lambda(n, n < Long(0))))).PartialAssert().Do();
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestPartialCall)
		{
			Initialize();
			{
				Manager manager("Test Partial Call", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				SN_DECLARE(f);
				SN_DECLARE(g);
				SN_DECLARE(a);
				SN_DECLARE(b);
				SN_DECLARE(c);

				(f(a)(b) == a * b).PartialAssert().Do();
				(Define(g) == f(Long(5))).PartialAssert().Do();

				(g(Long(4)) == c).Assert().Do();
				string c_string = c.GetVariableValue().DisplayValueSN();
				Assert::IsTrue(c_string == "Long(20)");

				(c == Long(20)).Assert().Do();
			}
			Cleanup();
		}


		TEST_METHOD(TestYCombinator)
		{
			Initialize();
			{
				Manager manager("Test Y Combinator", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				SN_DECLARE(Y);
				SN_DECLARE(f);
				SN_DECLARE(x);

				// Y = \f.(\x.x x) (\x.f (x x))
				(Define(Y) == Lambda(f, Lambda(x, f(x(x)))(Lambda(x, f(x(x)))))).PartialAssert().Do();

				SN_DECLARE(Fact);
				SN_DECLARE(g);
				SN_DECLARE(n);

				(Define(Fact)(g)(n) == (n == Long(0)).If(Long(1), n * g(n - Long(1)))).PartialAssert().Do();

				(Y(Fact)(Long(0)) == Long(1)).Evaluate().Do();
				(Y(Fact)(Long(1)) == Long(1)).Evaluate().Do();
				(Y(Fact)(Long(3)) == Long(6)).Evaluate().Do();
				(Y(Fact)(Long(10)) == Long(3628800)).Evaluate().Do();
				(Y(Fact)(Long(12)) == Long(479001600)).Evaluate().Do();

				(Y(Fact)(Long(0)) == Long(1)).Assert().Do();
				(Y(Fact)(Long(1)) == Long(1)).Assert().Do();
				(Y(Fact)(Long(3)) == Long(6)).Assert().Do();
				(Y(Fact)(Long(10)) == Long(3628800)).Assert().Do();
				(Y(Fact)(Long(12)) == Long(479001600)).Assert().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestChurchSucc)
		{
			Initialize();
			{
				Manager manager("Test Church Succ", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

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
				(inc(v) == v + Long(1)).PartialAssert().Do();
				(inc(inc(inc(Long(0)))) == Long(3)).Assert().Do();

				// succ n f x = f (n f x)
				(Define(succ)(n)(f)(x) == f(n(f)(x))).PartialAssert().Do();

				(succ(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == Long(1)).Evaluate().Do();
				(succ(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == r0).Assert().Do();
				(r0 == Long(1)).Evaluate().Do();


				(succ(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == Long(2)).Evaluate().Do();
				(succ(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == r1).Assert().Do();
				(r1 == Long(2)).Evaluate().Do();

				(succ(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == Long(3)).Evaluate().Do();
				(succ(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == r2).Assert().Do();
				(r2 == Long(3)).Evaluate().Do();

				(succ(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == Long(4)).Evaluate().Do();
				(succ(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == r3).Assert().Do();
				(r3 == Long(4)).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestChurchPlus)
		{
			Initialize();
			{
				Manager manager("Test Church Plus", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

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
				(inc(v) == v + Long(1)).PartialAssert().Do();
				(inc(inc(inc(Long(0)))) == Long(3)).Assert().Do();

				// plus m n f x = m f (n f x) 
				(Define(plus) == Lambda(m, Lambda(n, Lambda(f, Lambda(x, m(f)((n(f)(x)))))))).PartialAssert().Do();

				(plus(Lambda(f, Lambda(x, x)))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == Long(0)).Evaluate().Do();
				(plus(Lambda(f, Lambda(x, x)))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == r0).Assert().Do();
				(r0 == Long(0)).Evaluate().Do();

				(plus(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == Long(2)).Evaluate().Do();
				(plus(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == r1).Assert().Do();
				(r1 == Long(2)).Evaluate().Do();

				(plus(Lambda(f, Lambda(x, f(f(x)))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == Long(4)).Evaluate().Do();
				(plus(Lambda(f, Lambda(x, f(f(x)))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == r2).Assert().Do();
				(r2 == Long(4)).Evaluate().Do();

				(plus(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == Long(6)).Evaluate().Do();
				(plus(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == r3).Assert().Do();
				(r3 == Long(6)).Evaluate().Do();
			}
			Cleanup();
		}
		
		TEST_METHOD(TestChurchMultiply)
		{
			Initialize();
			{
				Manager manager("Test Church Multiply", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

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
				(inc(v) == v + Long(1)).PartialAssert().Do();
				(inc(inc(inc(Long(0)))) == Long(3)).Assert().Do();

				// multiply m n f x = m (n f) x
				(Define(mult)(m)(n)(f)(x) == m(n(f))(x)).PartialAssert().Do();

				(mult(Lambda(f, Lambda(x, x)))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == Long(0)).Evaluate().Do();
				(mult(Lambda(f, Lambda(x, x)))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == r0).Assert().Do();
				(r0 == Long(0)).Evaluate().Do();

				(mult(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == Long(1)).Evaluate().Do();
				(mult(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == r1).Assert().Do();
				(r1 == Long(1)).Evaluate().Do();

				(mult(Lambda(f, Lambda(x, f(f(x)))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == Long(4)).Evaluate().Do();
				(mult(Lambda(f, Lambda(x, f(f(x)))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == r2).Assert().Do();
				(r2 == Long(4)).Evaluate().Do();

				(mult(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == Long(9)).Evaluate().Do();
				(mult(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == r3).Assert().Do();
				(r3 == Long(9)).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestChurchExp)
		{
			Initialize();
			{
				Manager manager("Test Church Exp", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

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
				(inc(v) == v + Long(1)).PartialAssert().Do();
				(inc(inc(inc(Long(0)))) == Long(3)).Assert().Do();

				// exp m n f x = (n m) f x 
				(Define(exp)(m)(n)(f)(x) == m(n)(f)(x)).PartialAssert().Do();

				(exp(Lambda(f, Lambda(x, x)))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == Long(1)).Evaluate().Do();
				(exp(Lambda(f, Lambda(x, x)))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == r0).Assert().Do();
				(r0 == Long(1)).Evaluate().Do();


				(exp(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == Long(1)).Evaluate().Do();
				(exp(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == r1).Assert().Do();
				(r1 == Long(1)).Evaluate().Do();

				(exp(Lambda(f, Lambda(x, f(f(x)))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == Long(4)).Evaluate().Do();
				(exp(Lambda(f, Lambda(x, f(f(x)))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == r2).Assert().Do();
				(r2 == Long(4)).Evaluate().Do();

				(exp(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == Long(27)).Evaluate().Do();
				(exp(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == r3).Assert().Do();
				(r3 == Long(27)).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestChurchPred)
		{
			Initialize();
			{
				Manager manager("Test Church Pred", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

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
				(inc(v) == v + Long(1)).PartialAssert().Do();
				(inc(inc(inc(Long(0)))) == Long(3)).Assert().Do();

				// \n.\f.\x.n (\g.\h.h (g f)) (\u.x) (\u.u)
				(Define(pred) == Lambda(n, Lambda(f, Lambda(x, n(Lambda(g, Lambda(h, h(g(f))))) (Lambda(u, x))(Lambda(u, u)))))).PartialAssert().Do();

				(pred(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == Long(0)).Evaluate().Do();
				(pred(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == r0).Assert().Do();
				(r0 == Long(0)).Evaluate().Do();

				(pred(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == Long(0)).Evaluate().Do();
				(pred(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == r1).Assert().Do();
				(r1 == Long(0)).Evaluate().Do();

				(pred(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == Long(1)).Evaluate().Do();
				(pred(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == r2).Assert().Do();
				(r2 == Long(1)).Evaluate().Do();

				(pred(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == Long(2)).Evaluate().Do();
				(pred(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == r3).Assert().Do();
				(r3 == Long(2)).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestChurchMinus)
		{
			Initialize();
			{
				Manager manager("Test Church Minus", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

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
				(inc(v) == v + Long(1)).PartialAssert().Do();
				(inc(inc(inc(Long(0)))) == Long(3)).Assert().Do();

				// \n.\f.\x.n (\g.\h.h (g f)) (\u.x) (\u.u)
				(Define(pred) == Lambda(n, Lambda(f, Lambda(x, n(Lambda(g, Lambda(h, h(g(f))))) (Lambda(u, x))(Lambda(u, u)))))).PartialAssert().Do();

				// minus m n = (n pred) m	
				(Define(minus)(m)(n) == n(pred)(m)).PartialAssert().Do();

				// This works too.
				// (Define(minus)(m)(n)(f)(x) == n(pred)(m)(f)(x)).PartialAssert().Do();

				(minus(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == Long(1)).Evaluate().Do();
				(minus(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == r0).Assert().Do();
				(r0 == Long(1)).Evaluate().Do();

				(minus(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == Long(2)).Evaluate().Do();
				(minus(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == r1).Assert().Do();
				(r1 == Long(2)).Evaluate().Do();

				(minus(Lambda(f, Lambda(x, f(f(f(f(f(x))))))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == Long(3)).Evaluate().Do();
				(minus(Lambda(f, Lambda(x, f(f(f(f(f(x))))))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == r2).Assert().Do();
				(r2 == Long(3)).Evaluate().Do();

				(minus(Lambda(f, Lambda(x, f(f(f(f(f(f(f(f(x)))))))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == Long(5)).Evaluate().Do();
				(minus(Lambda(f, Lambda(x, f(f(f(f(f(f(f(f(x)))))))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == r3).Assert().Do();
				(r3 == Long(5)).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestChurchDivide)
		{
			return;
			Initialize();
			{
				Manager manager("Test Church Divide", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

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

				(divide(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == Long(1)).Evaluate().Do();
				(divide(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == r1).Assert().Do();
				(r1 == Long(1)).Evaluate().Do();

				(divide(Lambda(f, Lambda(x, f(f(f(f(x)))))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == Long(2)).Evaluate().Do();
				(divide(Lambda(f, Lambda(x, f(f(f(f(x)))))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == r2).Assert().Do();
				(r2 == Long(2)).Evaluate().Do();

				(divide(Lambda(f, Lambda(x, f(f(f(f(f(f(f(f(f(x))))))))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == Long(3)).Evaluate().Do();
				(divide(Lambda(f, Lambda(x, f(f(f(f(f(f(f(f(f(x))))))))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == r3).Assert().Do();
				(r3 == Long(3)).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestStringRefDefinition)
		{
			Initialize();
			{
				Manager manager("Test String Ref Definition", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, false);
				/*
				SN_DECLARE(x);
				SN_DECLARE(y);
				(String("dogcat") == x + y).Assert().Do();
				(x == "dog").Assert().Do();
				y.DoEvaluate().Equivalent(String("cat")));
				*/
				SN_DECLARE(a);
				SN_DECLARE(b);
				SN_DECLARE(c);
				SN_DECLARE(d);
				(String("ratdogcat") == a + b).Assert().Do();
				(a == d + c).Assert().Do();

				(d == String("rat")).Assert().Do();
				string d_string = d.GetString();
				Assert::IsTrue(d_string == "rat");

				(c == String("dog")).Assert().Do();
				string c_string = c.GetString();
				Assert::IsTrue(c_string == "dog");

				Assert::IsTrue(b.DoEvaluate().Equivalent(String("cat")));
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
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(x);
				(x == Long(3)).Assert().Do();
				string x_string = x.GetVariableValue().DisplayValueSN();

				Assert::IsTrue(x_string == "Long(3)");
				(x == Long(3)).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestOr2)
		{
			Initialize();
			{
				Manager manager("Test Or 2", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(x);
				(x == Long(3) || x == Long(4)).Assert().Do();
				string x_string = x.DisplaySN();
				string x_valueset = x.DoEvaluate().DisplaySN();
				string x_buildset = x.BuildSet().DoEvaluate().DisplaySN();
				string t_valueset = (Long(3) || Long(4)).DoEvaluate().DisplaySN();
				string t_buildset = (Long(3) || Long(4)).BuildSet().DoEvaluate().DisplaySN();
				SN_DECLARE(y);
				(y == (Long(3) || Long(4))).Assert().Do();
				string y_string = y.DisplaySN();
				string y_valueset = y.DoEvaluate().DisplaySN();
				string y_buildset = y.BuildSet().DoEvaluate().DisplaySN();

				(x.BuildSet() == y.BuildSet()).Evaluate().Do();
				(x.BuildSet() == (Long(3) || Long(4)).BuildSet()).Evaluate().Do();
				cout << x.DisplaySN();
			}
			Cleanup();
		}

		TEST_METHOD(TestOr3)
		{
			Initialize();
			{
				Manager manager("Test Or 3", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(x);
				(x == Long(3) || x == Long(4) || x == Long(5)).Assert().Do();
				string x_string = x.DisplaySN();
				string x_valueset = x.DoEvaluate().DisplaySN();
				string x_buildset = x.BuildSet().DoEvaluate().DisplaySN();
				x.LogDisplaySN();
				x.DoEvaluate().LogDisplaySN();
				x.BuildSet().DoEvaluate().LogDisplaySN();

				(x.BuildSet() == (Long(3) || Long(4) || Long(5)).BuildSet()).Evaluate().Do();
				cout << x.DisplaySN();
			}
			Cleanup();
		}

		TEST_METHOD(TestOr4)
		{
			Initialize();
			{
				Manager manager("Test Or 4", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(x);
				(x == Long(3) || x == Long(4) || x == Long(5) || x == Long(6)).Assert().Do();
				string x_string = x.DisplaySN();
				string x_valueset = x.DoEvaluate().DisplaySN();
				string x_buildset = x.BuildSet().DoEvaluate().DisplaySN();

				(x.BuildSet() == (Long(3) || Long(4) || Long(5) || Long(6)).BuildSet()).Evaluate().Do();
				cout << x.DisplaySN();
			}

			Cleanup();
		}

		TEST_METHOD(TestOr4WithCalc)
		{
			Initialize();
			{
				Manager manager("Test Or 4 With Calc", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(x);
				(x == Long(3)+Long(3) || x == Long(4)+Long(4) || x == Long(5)+Long(5) || x == Long(6)+Long(6)).Assert().Do();
				string x_string = x.DisplaySN();
				string x_valueset = x.DoEvaluate().DisplaySN();
				string x_buildset = x.BuildSet().DoEvaluate().DisplaySN();

				(x.BuildSet() == (Long(3)+Long(3) || Long(4)+Long(4) || Long(5)+Long(5) || Long(6)+Long(6)).BuildSet()).Evaluate().Do();
				cout << x.DisplaySN();
			}
			Cleanup();
		}

		TEST_METHOD(TestOrReversed4WithCalc)
		{
			Initialize();
			{
				Manager manager("Test Or Reversed 4 With Calc", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(x);
				(Long(3) + Long(3) == x || Long(4) + Long(4) == x || Long(5) + Long(5) == x || Long(6) + Long(6) == x).Assert().Do();
				string x_string = x.DisplaySN();
				string x_valueset = x.DoEvaluate().DisplaySN();
				string x_buildset = x.BuildSet().DoEvaluate().DisplaySN();

				(x.BuildSet() == (Long(3) + Long(3) || Long(4) + Long(4) || Long(5) + Long(5) || Long(6) + Long(6)).BuildSet()).Evaluate().Do();
				cout << x.DisplaySN();
			}
			Cleanup();
		}

		TEST_METHOD(TestOrReversed4WithSolve)
		{
			Initialize();
			{
				Manager manager("Test Or Reversed 4 With Solve", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(x);
				(Long(6) == x + Long(3) || Long(8) == x + Long(4) || Long(10) == x + Long(5) || Long(12) == x + Long(6)).Assert().Do();
				string x_string = x.DisplaySN();
				string x_valueset = x.DoEvaluate().DisplaySN();
				string x_buildset = x.BuildSet().DoEvaluate().DisplaySN();

				(x.BuildSet() == (Long(3) || Long(4) || Long(5) || Long(6)).BuildSet()).Evaluate().Do();
				cout << x.DisplaySN();
			}
			Cleanup();
		}

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

				(Define(Digit)(d) == (d == (String("0") || String("1") || String("2") || String("3") || String("4")
					|| String("5") || String("6") || String("7") || String("8") || String("9")))).PartialAssert().Do();

				SN_DECLARE(IsInteger);
				(Define(IsInteger)(i) == (Digit(i.SelectLeftChar()) && (!Digit(i.SubtractLeftChar().LookaheadLeft())).If(i == i.SelectLeftChar(), IsInteger(i.SubtractLeftChar())))).PartialAssert().Do();

				SN_DECLARE(ConvertInteger);
				(Define(ConvertInteger)(i) == Let(IsInteger(i), i.StringToInt())).PartialAssert().Do();

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
				SN_DECLARE(PmGenInteger);
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

				long x2_long = Long(x2).GetNumber();
				long x3_long = Long(x3).GetNumber();
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

				manager.Breakpoint();
				manager.SetDebugAction(skynet::StepInto);

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

		TEST_METHOD(TestError)
		{
			Initialize();
			{
				Manager manager("Test Error", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				try
				{
					SN_DECLARE(a);
					SN_DECLARE(b);
					SN_DECLARE(c);
					(a || (b(String("dog")) == c)).PartialAssert().Throw();

					Assert::IsTrue(false, L"Expected a partial assert error.");
				}
				catch (Error e)
				{
					string description = e.GetDescription();
					Assert::IsTrue(e.IsError(), wstring(description.begin(), description.end()).c_str());
				}
				try
				{
					Function(Function(skynet::And, Long(5)), Long(6)).Assert().Throw();

					Assert::IsTrue(false, L"Expected an operator not implemented.");
				}
				catch (Error e)
				{
					string description = e.GetDescription();
					Assert::IsTrue(e.IsError(), wstring(description.begin(), description.end()).c_str());
				}
			}
		}

		TEST_METHOD(TestMetaEvaluate)
		{
			Initialize();
			{
				Manager manager("Test Meta Evaluate", AssertErrorHandler);

				SN_DECLARE_VALUE(dog, String("Vincent "));
				SN_DECLARE_VALUE(cat, String("Emma "));

				Assert::IsTrue(Meta(1, String("Vincent ") + String("Emma ")).DoEvaluate().Equivalent(Meta(1, Function(Function(Add, String("Vincent ")), String("Emma ")))));
				Assert::IsTrue(Meta(1, dog + cat).DoEvaluate().Equivalent(Meta(1, Function(Function(Add, dog), cat))));

				std::cout << std::endl << Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoEvaluate().Equivalent(Meta(1, String("Vincent ") + String("Emma "))));

				std::cout << std::endl << Meta(-1, Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoEvaluate()).DoEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(-1, Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoEvaluate()).DoEvaluate().Equivalent(String("Vincent Emma ")));

				std::cout << std::endl << Meta(1, Meta(-1, dog)).DoEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(1, Meta(-1, dog)).DoEvaluate().Equivalent(Meta(1, dog.DoEvaluate())));

				SN_DECLARE_VALUE(e, Meta(2, Meta(-1, dog) + Meta(-1, cat)));

				std::cout << std::endl << e.DoEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(e.DoEvaluate().Equivalent(Meta(2, Meta(-1, dog) + Meta(-1, cat))));

				std::cout << std::endl << Meta(-1, e).DoEvaluate().DisplaySN() << std::endl;


				Assert::IsTrue(Meta(-1, e).DoEvaluate().Equivalent(Meta(1, String("Vincent ") + String("Emma "))));
				Assert::IsTrue(e.DoEvaluate(-1).Equivalent(Meta(1, String("Vincent ") + String("Emma "))));

				std::cout << std::endl << Meta(-1, Meta(-1, e).DoEvaluate()).DoEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(-1, Meta(-1, e).DoEvaluate()).DoEvaluate().Equivalent(String("Vincent Emma ")));
				Assert::IsTrue(e.DoEvaluate(-1).DoEvaluate(-1).Equivalent(String("Vincent Emma ")));

				Assert::IsTrue(Meta(1, Meta(-1, dog)).DoEvaluate().Equivalent(Meta(1, dog.DoEvaluate())));
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

				Assert::IsTrue(Meta(1, String("Vincent ") + String("Emma ")).DoPartialEvaluate().Equivalent(Meta(1, Function(Function(Add, String("Vincent ")), String("Emma ")))));

				std::cout << std::endl << Meta(1, dog + cat).DoPartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(1, dog + cat).DoPartialEvaluate().Equivalent(Meta(1, Function(Function(Add, dog), cat))));

				std::cout << std::endl << Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoPartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoPartialEvaluate().Equivalent(Meta(1, String("Vincent ") + String("Emma "))));

				std::cout << std::endl << Meta(-1, Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoPartialEvaluate()).DoPartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(-1, Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoPartialEvaluate()).DoPartialEvaluate().Equivalent(String("Vincent Emma ")));

				std::cout << std::endl << Meta(1, Meta(-1, dog)).DoPartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(1, Meta(-1, dog)).DoPartialEvaluate().Equivalent(Meta(1, dog.DoPartialEvaluate())));

				SN_DECLARE_VALUE(e, Meta(2, Meta(-1, dog) + Meta(-1, cat)));

				std::cout << std::endl << e.DoPartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(e.DoPartialEvaluate().Equivalent(Meta(2, Meta(-1, dog) + Meta(-1, cat))));

				std::cout << std::endl << Meta(-1, e).DoPartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(-1, e).DoPartialEvaluate().Equivalent(Meta(1, String("Vincent ") + String("Emma "))));
				Assert::IsTrue(e.DoPartialEvaluate(-1).Equivalent(Meta(1, String("Vincent ") + String("Emma "))));

				std::cout << std::endl << Meta(-1, Meta(-1, e).DoPartialEvaluate()).DoPartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(-1, Meta(-1, e).DoPartialEvaluate()).DoPartialEvaluate().Equivalent(String("Vincent Emma ")));
				Assert::IsTrue(e.DoPartialEvaluate(-1).DoPartialEvaluate(-1).Equivalent(String("Vincent Emma ")));
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

				Assert::IsTrue(Meta(1, String("Vincent ") + String("Emma ")).DoPartialEvaluate().Equivalent(Meta(1, Function(Function(Add, String("Vincent ")), String("Emma ")))));

				std::cout << std::endl << Meta(1, dog + cat).DoPartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(1, dog + cat).DoPartialEvaluate().Equivalent(Meta(1, Function(Function(Add, dog), cat))));

				std::cout << std::endl << Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoPartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoPartialEvaluate().Equivalent(Meta(1, dog + cat)));

				std::cout << std::endl << Meta(-1, Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoPartialEvaluate()).DoPartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(-1, Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoPartialEvaluate()).DoPartialEvaluate().Equivalent(dog + cat));

				std::cout << std::endl << Meta(1, Meta(-1, dog)).DoPartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(1, Meta(-1, dog)).DoPartialEvaluate().Equivalent(Meta(1, dog.DoPartialEvaluate())));

				SN_DECLARE_VALUE(e, Meta(2, Meta(-1, dog) + Meta(-1, cat)));

				std::cout << std::endl << e.DoPartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(e.DoPartialEvaluate().Equivalent(Meta(2, Meta(-1, dog) + Meta(-1, cat))));

				std::cout << std::endl << Meta(-1, e).DoPartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(-1, e).DoPartialEvaluate().Equivalent(Meta(1, dog + cat)));
				Assert::IsTrue(e.DoPartialEvaluate(-1).Equivalent(Meta(1, dog + cat)));

				std::cout << std::endl << Meta(-1, Meta(-1, e).DoPartialEvaluate()).DoPartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(-1, Meta(-1, e).DoPartialEvaluate()).DoPartialEvaluate().Equivalent(dog + cat));
				Assert::IsTrue(e.DoPartialEvaluate(-1).DoPartialEvaluate(-1).Equivalent(dog + cat));
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
				Assert::IsTrue((Long(5) || Long(7)).DoEvaluate().DoRemove(Long(5)).DoRemove(Long(7)).DoIsEmpty());

				Assert::IsTrue((String("dog") || String("cat")).DoEvaluate().DoRemove(String("dog")).DoRemove(String("cat")).DoIsEmpty());

				Assert::IsTrue((Char('d') || Char('c')).DoEvaluate().DoRemove(Char('d')).DoRemove(Char('c')).DoIsEmpty());

				Assert::IsTrue((((String("catdograt") || String("pigdograt")) - (String("dograt") || String("rat")))).DoEvaluate().DoRemove(String("cat")).DoRemove(String("pig")).DoRemove(String("catdog")).DoRemove(String("pigdog")).DoIsEmpty());


				Assert::IsTrue((((String("cat") || String("dog")) + (String("food") || String("awesomefood")))).DoEvaluate().DoRemove(String("catfood")).DoRemove(String("catawesomefood")).DoRemove(String("dogfood")).DoRemove(String("dogawesomefood")).DoIsEmpty());

				Assert::IsTrue((Double(5) || Double(7)).DoEvaluate().DoRemove(Double(5)).DoRemove(Double(7)).DoIsEmpty());

				Assert::IsTrue((((Double(5) || Double(7)) + (Double(8) || Double(9)))).DoEvaluate().DoRemove(Double(13)).DoRemove(Double(14)).DoRemove(Double(15)).DoRemove(Double(16)).DoIsEmpty());
				// The order does not matter
				Assert::IsTrue((Long(-4) || Long(4)).DoEvaluate().DoRemove(Long(4)).DoRemove(Long(-4)).DoIsEmpty());

				// Value sets behave like values.  an operator or function applied to value set, generates the cardesian product of the values, with the function applied.
				Assert::IsTrue((((Long(5) || Long(7)) + (Long(8) || Long(9)))).DoEvaluate().DoRemove(Long(13)).DoRemove(Long(14)).DoRemove(Long(15)).DoRemove(Long(16)).DoIsEmpty());

				//added for subtraction
				Assert::IsTrue((((Long(9) || Long(11)) - (Long(3) || Long(4)))).DoEvaluate().DoRemove(Long(6)).DoRemove(Long(5)).DoRemove(Long(8)).DoRemove(Long(7)).DoIsEmpty());

				Assert::IsTrue((((Long(1) || Long(2)) * (Long(3) || Long(4)))).DoEvaluate().DoRemove(Long(3)).DoRemove(Long(4)).DoRemove(Long(6)).DoRemove(Long(8)).DoIsEmpty());

				Assert::IsTrue((((Long(5) || Long(10)) / (Long(1) || Long(5)))).DoEvaluate().DoRemove(Double(5)).DoRemove(Double(1)).DoRemove(Double(10)).DoRemove(Double(2)).DoIsEmpty());

				// If the values are tagged with worlds from different world sets then all the values from the cardesian product are created. 
				// first    2 || -2 = [2:A, -2:B]
				// second   2 || -2 = [2:C, -2:D]
				// first+second = [2+2:AC, 0:AD, 0:BC, -2:BD]
				Assert::IsTrue((((Long(2) || Long(-2)) + (Long(2) || Long(-2)))).DoEvaluate().DoRemove(Long(4)).DoRemove(Long(0)).DoRemove(Long(-4)).DoIsEmpty());

				// If using the same value set twice then must use the same value each time.
				// This is because the intersection of different worlds from the same value set is always empty.
				// More explictly, each value in a vale set is tagged with a world from a world set.
				//     2 || -2 = [2:A, -2:B]
				// where A and B are worlds. Worlds are sets.
				// A intersection B is empty.
				SN_DECLARE_VALUE(two_or_minus_two, (Long(2) || Long(-2)).DoEvaluate());

				Assert::IsTrue((two_or_minus_two + two_or_minus_two).DoEvaluate().DoRemove(Long(4)).DoRemove(Long(-4)).DoIsEmpty());

				Assert::IsTrue(((Long(2) || Long(-2)) + (Long(2) || Long(-2))).DoEvaluate().DoRemove(Long(4)).DoRemove(Long(0)).DoRemove(Long(-4)).DoIsEmpty());

				(two_or_minus_two + two_or_minus_two).BuildSet().HasMember(Long(-4)).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestValueSetEquivalencyEvaluate)
		{
			Initialize();
			{
				Manager manager("Test Value Set Equivalency Evaluate", AssertErrorHandler);

				(((String("Dog") || String("Cat")) + String("Food")).BuildSet() == ((String("DogFood") || String("CatFood")).BuildSet())).Evaluate().Do();

				(((String("Dog") || String("Cat")) + String("Food")).BuildSet() == ((String("CatFood") || String("DogFood")).BuildSet())).Evaluate().Do();
				(((String("Dog") || String("Pig")) + String("Food")).BuildSet() == ((String("DogFood") || String("PigFood")).BuildSet())).Evaluate().Do();

				(((Long(10) || Long(9)) + Long(1)).BuildSet() == (Long(11) || Long(10)).BuildSet()).Evaluate().Do();

				(((Double(10) || Double(9)) + Double(1)).BuildSet() == (Double(11) || Double(10)).BuildSet()).Evaluate().Do();


				// mixed value sets
				Assert::IsTrue((String("dog") || Long(9)).DoEvaluate().DoRemove(String("dog")).DoRemove(Long(9)).DoIsEmpty());

				//NESTED OR CASE
				Assert::IsTrue(((String("dog") || Long(9)) || String("Cat")).DoEvaluate().DoRemove(String("dog")).DoRemove(Long(9)).DoRemove(String("Cat")).DoIsEmpty());
			}
			Cleanup();
		}

		TEST_METHOD(TestSetEvaluate)
		{
			Initialize();
			{
				Manager manager("Test Set Evaluate", AssertErrorHandler);

				Expression a;
				(((Long(2) || Long(-2))*(Long(3) || Long(-3))).BuildSet() == ((Long(6) || Long(-6)).BuildSet())).Evaluate().Do();

				((Long(4).SquareRoot()).BuildSet() == (Long(2) || Long(-2)).BuildSet()).Evaluate().Do();
				string my_exp4 = (Long(4)).BuildSet().DoEvaluate().DisplaySN();
				std::cout << std::endl << ((Long(4)).BuildSet()).DoEvaluate().DisplaySN() << std::endl;
				Assert::IsTrue(((Long(4)).BuildSet()).DoEvaluate().DisplaySN() == "{Long(4)}");

				string my_exp5 = ((Long(2) || Long(-2)) + (Long(2) || Long(-2))).DoEvaluate().DisplaySN();
				Assert::IsTrue(((Long(2) || Long(-2)) + (Long(2) || Long(-2))).DoEvaluate().DoRemove(Long(4)).DoRemove(Long(0)).DoRemove(Long(-4)).DoIsEmpty());

				SN_DECLARE_VALUE(x, Long(2) || Long(-2));

				(((x < Long(0)).If(x || Long(5), Long(0))).BuildSet() == (Long(0) || Long(5) || Long(-2)).BuildSet()).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestSetAssert)
		{
			Initialize();
			{
				Manager manager("Test Set Assert", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(x);
				SN_DECLARE(z);
				(x.Square() == Long(4)).Assert().Do();
				((x.BuildSet()) == (Long(2) || Long(-2)).BuildSet()).Evaluate().Do();
				(z == (Long(2) || Long(-2))).Assert().Do();

				// This is not true. The expression is a value set with true and false in it.
				//(x == (Long(2) || Long(-2))).Evaluate().Do();

				SN_DECLARE(y);
				(y*y == Long(4)).Assert().Do();   // recognise that y*y = y.Square() in the * operator in SNI_Expression.
				((y.BuildSet()) == (Long(2) || Long(-2)).BuildSet()).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestSubStrings)
		{
			Initialize();
			{
				Manager manager("Test Sub Strings", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				(String("catdog").SubtractRight(String("catdog").SubtractLeft(String("cat"))) == String("cat")).Assert().Do();
				(String("catdog").SubtractLeft(String("catdog").SubtractRight(String("dog"))) == String("dog")).Assert().Do();

				SN_DECLARE_VALUE(x, String("catdog"));
				SN_DECLARE_VALUE(y, String("cat"));
				SN_DECLARE_VALUE(z, String("dog"));

				(x.SubtractRight(x.SubtractLeft(y)) == y).Assert().Do();
				(x.SubtractLeft(x.SubtractRight(z)) == z).Assert().Do();

				(String("GamesX").SubtractRightChar() + String("GamesX").SelectRightChar() == String("GamesX")).Assert().Do();
				SN_DECLARE_VALUE(m, String("GamesX"));
				(m.SubtractRightChar() + m.SelectRightChar() == m).Assert().Do();

				(String("XGames").SelectLeftChar() + String("XGames").SubtractLeftChar() == String("XGames")).Assert().Do();
				SN_DECLARE_VALUE(n, String("XGames"));
				(n.SelectLeftChar() + n.SubtractLeftChar() == n).Assert().Do();
			}
			Cleanup();
		}

/*		TEST_METHOD(TestAssertNothing)
		{
			Initialize();
			{
				Manager manager("Test Assert Nothing", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer2);
			}
			Cleanup();
		}
*/

		TEST_METHOD(TestAssertThrow)
		{
			Initialize();
			{
				Manager manager("Test Assert Throw", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer2);
				{
					try
					{
						(Long(5) == Long(6)).Assert().Throw();
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
					ofstream out(s_name_1);
					out << s_contents_1;
					out.close();
				}

				Manager manager("Test File Access", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer2);
				{
					Transaction transaction;

					SN_DECLARE_VALUE(name_1, String(s_name_1));
					SN_DECLARE_VALUE(contents_1, String(s_contents_1));

					(name_1.File() == contents_1).Assert().Do();

					{
						ofstream out(s_name_1);
						out << s_contents_1;
						out.close();
					}

					(name_1.File() == contents_1).Assert().Do();

					SN_DECLARE_VALUE(name_2, String(s_name_2));
					SN_DECLARE_VALUE(contents_2, String(s_contents_2));

					(name_2.File() == contents_2).Assert().Do();

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

					(name_2.File() == contents_2).Assert().Do(); // The assert still hold. For consistency, SN caches the value for the file name.
				}

				// Outside the transaction, the cache is cleared and the value is re-read.
				{
					Transaction transaction;

					SN_DECLARE_VALUE(name_1, String(s_name_1));
					SN_DECLARE_VALUE(name_2, String(s_name_2));
					SN_DECLARE_VALUE(contents_1, String(s_contents_1));
					(name_1.File() == name_2.File()).Assert().Do();
					(name_2.File() == contents_1).Assert().Do();
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestValueSetOfLambdaFunctions)
		{
			Initialize();
			{
				Manager manager("Test Value Set Of Lambda Functions", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

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
			Cleanup();
		}

		TEST_METHOD(TestValueSetOfStandardFunctions)
		{
			Initialize();
			{
				Manager manager("Test Value Set Of Standard Functions", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(f);
				(f == (skynet::Add || skynet::Multiply)).Assert().Do();
				string f_string = f.DisplaySN();

				SN_DECLARE(x);
				(x == f(long(8))(long(13))).Assert().Do();
				string x_string = x.BuildSet().DoEvaluate().DisplaySN();
				Assert::IsTrue(x_string == "{Long(21), Long(104)}");
				(x < long(30)).Assert().Do();
				string x_string2 = x.DisplaySN();
				long x_num = Long(x).GetNumber();
				Assert::IsTrue(x_num == 21);
				(x == Long(21)).Assert().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestValueSetOfStringFunctions)
		{
			Initialize();
			{
				Manager manager("Test Value Set Of String Functions", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer3);

				SN_DECLARE(f);

				SN_DECLARE(wrap);
				SN_DECLARE(map);
				{
					//Transaction transaction;
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

				Assert::IsTrue(x_string == x_compare_string);
			}
			Cleanup();
		}

		TEST_METHOD(TestStringSearch)
		{
			Initialize();
			{
				Manager manager("Test String Search", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				{
					Transaction transaction;

					SN_DECLARE(money);
					SN_DECLARE(evil);
					(money + (String(" is the root of all ") + evil) == String("Money is the root of all evil")).Assert().Do();
					string money_string = money.GetString();
					string evil_string = evil.GetString();
					Assert::IsTrue(money_string == "Money");
					Assert::IsTrue(evil_string == "evil");
				}
				{
					Transaction transaction;

					SN_DECLARE(money);
					SN_DECLARE(evil);
					(money + String(" is the root of all ") + evil == String("Money is the root of all evil")).Assert().Do();
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
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(firstname1);
				SN_DECLARE(surname1);
				(String("My first name is ") + (firstname1 + String(" and my surname is Fischer.")) == String("My first name is Bobby and my surname is Fischer.")).Assert().Do();
				string firstname1_string = firstname1.GetString();
				Assert::IsTrue(firstname1_string == "Bobby");

				SN_DECLARE(firstname2);
				SN_DECLARE(surname2);
				(String("My first name is ") + (firstname2 + (String(" and my surname is ") + surname2)) == String("My first name is Bobby and my surname is Fischer")).Assert().Do();
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
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				{
					Transaction transaction;

					SN_DECLARE(firstname);
					SN_DECLARE(surname);

					(String("My first name is ") + (firstname + (String(" and my surname is ") + (surname + String(".")))) == String("My first name is Bobby and my surname is Fischer.")).Assert().Do();

					string firstname_string = firstname.GetString();
					string surname_string = surname.GetString();
					Assert::IsTrue(firstname_string == "Bobby");
					Assert::IsTrue(surname_string == "Fischer");

					(firstname == String("Bobby")).Assert().Do();
					(surname == String("Fischer")).Assert().Do();
				}
				{
					Transaction transaction;

					SN_DECLARE(firstname);
					SN_DECLARE(surname);

					(String("My first name is ") + firstname + String(" and my surname is ") + (surname + String(".")) == String("My first name is Bobby and my surname is Fischer.")).Assert().Do();
					(firstname == String("Bobby")).Assert().Do();
					(surname == String("Fischer")).Assert().Do();
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
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(firstClause);
				SN_DECLARE(secondClause);
				SN_DECLARE(sentence);
				(firstClause + String(" and ") + secondClause == String("I love dogs and cats and dogs love me.")).Assert().Do();
				string firstClause_string_vs = firstClause.DisplaySN();
				string secondClause_string_vs = secondClause.DisplaySN();
				
				string firstClause_string = firstClause.BuildSet().DoEvaluate().DisplaySN();
				string secondClause_string = secondClause.BuildSet().DoEvaluate().DisplaySN();
				Assert::IsTrue(firstClause_string == "{String(\"I love dogs\"), String(\"I love dogs and cats\")}");
				Assert::IsTrue(secondClause_string == "{String(\"cats and dogs love me.\"), String(\"dogs love me.\")}");
				
				(sentence == firstClause + (String(" and ") + secondClause)).Assert().Do();
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
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(X);
				(Double(245.67).Square() + X.Square() == Double(357.56).Square()).Assert().Do();
				string X_string = X.BuildSet().DoEvaluate().DisplaySN();
				Assert::IsTrue(X_string == "{Double(259.798777), Double(-259.798777)}");
			}
			Cleanup();
		}

		TEST_METHOD(TestSimpleTransaction)
		{
			Initialize();
			{
				Manager manager("Test Simple Transaction", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(X);
				{
					Transaction transaction;
					(X + Long(3) == Long(5)).Assert().Do();
				}
				string X_string = X.DoEvaluate().DisplayValueSN();
				Assert::IsTrue(X_string == "Long(2)");
			}
			Cleanup();
		}

		TEST_METHOD(TestInheritInstance)
		{
			Initialize();
			{
				Manager manager("Test Simple Inherit1", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				skynet::True.IsA(Bool::Class()).Assert().Do();
				String("woof").IsA(String::Class()).Assert().Do();
				Char('X').IsA(Char::Class()).Assert().Do();
				(String("DogFood") || String("CatFood")).BuildSet().IsA(Set::Class()).Assert().Do();

				Meta(1, String("Vincent ") + String("Emma ")).IsA(Meta::Class()).Assert().Do();

				Short(5).IsA(Short::Class()).Assert().Do();
				Int(5).IsA(Int::Class()).Assert().Do();
				Long(5).IsA(Long::Class()).Assert().Do();
				LongLong(5).IsA(LongLong::Class()).Assert().Do();
				Float(5).IsA(Float::Class()).Assert().Do();
				Double(5).IsA(Double::Class()).Assert().Do();
				LongDouble(5).IsA(LongDouble::Class()).Assert().Do();
				Mapping().IsA(Mapping::Class()).Assert().Do();

				SN_DECLARE(a);
				SN_DECLARE(b);
				(String("dogcat") == a + b).Assert().Do();

				a.IsA(StringRef::Class()).Assert().Do();

				skynet::True.Assert().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestInheritShort)
		{
			Initialize();
			{
				Manager manager("Test Inherit Short", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				Short::Class().IsA(Long::Class()).Evaluate().Do();
				SN_DECLARE(x1);
				(Short::Class().IsA(Long::Class()) == x1).Assert().Do();
				x1.Evaluate().Do();
				bool b1 = x1.GetBool();
				Assert::IsTrue(b1);

				Short::Class().IsA(LongLong::Class()).Evaluate().Do();
				SN_DECLARE(x2);
				(Short::Class().IsA(LongLong::Class()) == x2).Assert().Do();
				x2.Evaluate().Do();
				bool b2 = x2.GetBool();
				Assert::IsTrue(b2);

				Short::Class().IsA(Float::Class()).Evaluate().Do();
				SN_DECLARE(x3);
				(Short::Class().IsA(Float::Class()) == x3).Assert().Do();
				x3.Evaluate().Do();
				bool b3 = x3.GetBool();
				Assert::IsTrue(b3);

				Short::Class().IsA(Double::Class()).Evaluate().Do();
				SN_DECLARE(x4);
				(Short::Class().IsA(Double::Class()) == x4).Assert().Do();
				x4.Evaluate().Do();
				bool b4 = x4.GetBool();
				Assert::IsTrue(b4);

				Short::Class().IsA(LongDouble::Class()).Evaluate().Do();
				SN_DECLARE(x5);
				(Short::Class().IsA(LongDouble::Class()) == x5).Assert().Do();
				x5.Evaluate().Do();
				bool b5 = x5.GetBool();
				Assert::IsTrue(b5);

			}
			Cleanup();
		}

		TEST_METHOD(TestInheritLong)
		{
			Initialize();
			{
				Manager manager("Test Inherit Long", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				Long::Class().IsA(LongLong::Class()).Evaluate().Do();
				SN_DECLARE(x1);
				(Long::Class().IsA(LongLong::Class()) == x1).Assert().Do();
				x1.Evaluate().Do();
				bool b1 = x1.GetBool();
				Assert::IsTrue(b1);

				Long::Class().IsA(Double::Class()).Evaluate().Do();
				SN_DECLARE(x2);
				(Long::Class().IsA(Double::Class()) == x2).Assert().Do();
				x2.Evaluate().Do();
				bool b2 = x2.GetBool();
				Assert::IsTrue(b2);

				Long::Class().IsA(LongDouble::Class()).Evaluate().Do();
				SN_DECLARE(x3);
				(Long::Class().IsA(LongDouble::Class()) == x3).Assert().Do();
				x3.Evaluate().Do();
				bool b3 = x3.GetBool();
				Assert::IsTrue(b3);

			}
			Cleanup();
		}

		TEST_METHOD(TestInheritLongLong)
		{
			Initialize();
			{
				Manager manager("Test Inherit LongLong", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				LongLong::Class().IsA(LongDouble::Class()).Evaluate().Do();
				SN_DECLARE(x1);
				(LongLong::Class().IsA(LongDouble::Class()) == x1).Assert().Do();
				x1.Evaluate().Do();
				bool b1 = x1.GetBool();
				Assert::IsTrue(b1);

			}
			Cleanup();
		}

		TEST_METHOD(TestInheritFloat)
		{
			Initialize();
			{
				Manager manager("Test Inherit Float", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				Float::Class().IsA(Double::Class()).Evaluate().Do();
				SN_DECLARE(x1);
				(Float::Class().IsA(Double::Class()) == x1).Assert().Do();
				x1.Evaluate().Do();
				bool b1 = x1.GetBool();
				Assert::IsTrue(b1);

				Float::Class().IsA(LongDouble::Class()).Evaluate().Do();
				SN_DECLARE(x2);
				(Float::Class().IsA(LongDouble::Class()) == x2).Assert().Do();
				x2.Evaluate().Do();
				bool b2 = x2.GetBool();
				Assert::IsTrue(b2);

			}
			Cleanup();
		}

		TEST_METHOD(TestInheritDouble)
		{
			Initialize();
			{
				Manager manager("Test Inherit Double", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				Double::Class().IsA(LongDouble::Class()).Evaluate().Do();
				SN_DECLARE(x1);
				(Double::Class().IsA(LongDouble::Class()) == x1).Assert().Do();
				x1.Evaluate().Do();
				bool b1 = x1.GetBool();
				Assert::IsTrue(b1);

			}
			Cleanup();
		}

		TEST_METHOD(TestInheritChar)
		{
			Initialize();
			{
				Manager manager("Test Inherit Char", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				Char::Class().IsA(String::Class()).Evaluate().Do();
				SN_DECLARE(x1);
				(Char::Class().IsA(String::Class()) == x1).Assert().Do();
				x1.Evaluate().Do();
				bool b1 = x1.GetBool();
				Assert::IsTrue(b1);

			}
			Cleanup();
		}

		TEST_METHOD(TestInheritStringRef)
		{
			Initialize();
			{
				Manager manager("Test Inherit Char", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				StringRef::Class().IsA(String::Class()).Evaluate().Do();
				SN_DECLARE(x1);
				(StringRef::Class().IsA(String::Class()) == x1).Assert().Do();
				x1.Evaluate().Do();
				bool b1 = x1.GetBool();
				Assert::IsTrue(b1);

			}
			Cleanup();
		}

		TEST_METHOD(TestInheritPolymorphicCall)
		{
			return; // bark
			Initialize();
			{
				Manager manager("Test Inherit Polymorphic Call", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(A);
				SN_DECLARE(B);
				SN_DECLARE(M);
				SN_DECLARE(F);
				{
					Transaction transaction;
					A.IsA(B).PartialAssert().Do();
					(F(B) == Long(10)).Assert().Do();
					M(B).Assert().Do();
					(F(A) == Long(10)).Evaluate().Do();
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestMappingForward)
		{
			Initialize();
			{
				Manager manager("Test Mapping Forward", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(M);
				SN_DECLARE(I);
				SN_DECLARE(B);
				SN_DECLARE(R);

				(M == Mapping()).Assert().Do();
				(M[String("Name")] == String("Max")).Assert().Do();
				(M[String("Name")] == String("Max")).Evaluate().Do();
				(M[String("Name")] == R).Assert().Do();
				string R_text = R.GetString();
				Assert::IsTrue(R_text == "Max");
				(R == String("Max")).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestMappingForwardValueset)
		{
			return;
			Initialize();
			{
				Manager manager("Test Mapping Forward Valueset", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(M);
				SN_DECLARE(I);
				SN_DECLARE(B);
				SN_DECLARE(R);

				(M == Mapping()).Assert().Do();
				(M[String("Name") || String("Fullname")] == String("Max")).Assert().Do();
				(M[String("Name")] == String("Max")).Evaluate().Do();
				(M[String("Fullname")] == String("Max")).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestMappingReverse)
		{
			Initialize();
			{
				Manager manager("Test Mapping Reverse", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(age);
				SN_DECLARE(I);
				SN_DECLARE(S);
				SN_DECLARE(K);
				SN_DECLARE(E);

				(age == Mapping()).Assert().Do();
				(age[String("Max")] == Long(43)).Assert().Do();
				(age[String("George")] == Long(55)).Assert().Do();
				(age[String("Roger")] == Long(43)).Assert().Do();

				age.Fix(String(""));

				(age[I] == Long(43)).Assert().Do();
				
				(I.BuildSet() == S).Assert().Do();
				(K == (String("Max") || String("Roger"))).Assert().Do();
				(E == K.BuildSet()).Assert().Do();
				string I_text = I.GetVariableValue().DisplaySN();
				string S_text = S.GetVariableValue().DisplaySN();
				string K_text = K.GetVariableValue().DisplaySN();
				string E_text = E.GetVariableValue().DisplaySN();
				Assert::IsTrue(S_text == "{String(\"Max\"), String(\"Roger\")}");
				(S == E).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestMappingCount)
		{
			Initialize();
			{
				Manager manager("Test Mapping Count", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(age);
				SN_DECLARE(X);

				(age == Mapping()).Assert().Do();
				(age[String("Max")] == Long(43)).Assert().Do();
				(age[String("George")] == Long(55)).Assert().Do();
				(age[String("Roger")] == Long(43)).Assert().Do();
				(age[String("Bob")] == Long(43)).Assert().Do();
				(age[String("Ken")] == Long(55)).Assert().Do();

				age.Fix(Long(0));

				SN_DECLARE(count43);
				SN_DECLARE(count55);
				SN_DECLARE(countBoth);
				SN_DECLARE(sum);

				(age.CountIf(Lambda(X, X == Long(43))) == count43).Assert().Do();
				(age.CountIf(Lambda(X, X == Long(55))) == count55).Assert().Do();
				(age.CountAll() == countBoth).Assert().Do();
				(age.Sum() == sum).Assert().Do();

				(count43 == Long(3)).Evaluate().Do();
				(count55 == Long(2)).Evaluate().Do();
				(countBoth == count43 + count55).Evaluate().Do();
				(sum == Long(239)).Evaluate().Do();

				string count43_text = count43.GetVariableValue().DisplaySN();
				string count55_text = count55.GetVariableValue().DisplaySN();
				string countBoth_text = countBoth.GetVariableValue().DisplaySN();
				string sum_text = sum.GetVariableValue().DisplaySN();

				Assert::IsTrue(count43_text == "3");
				Assert::IsTrue(count55_text == "2");
				Assert::IsTrue(countBoth_text == "5");
				Assert::IsTrue(sum_text == "239");
			}
			Cleanup();
		}

		TEST_METHOD(TestMappingFixErrors)
		{
			Initialize();
			{
				Manager manager("Test Mapping Fix Errors", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer2);
				{
					try
					{
						SN_DECLARE(age);
						SN_DECLARE(count43);
						SN_DECLARE(X);

						(age == Mapping()).Assert().Do();
						(age[String("Max")] == Long(43)).Assert().Do();
						(age[String("George")] == Long(55)).Assert().Do();
						(age[String("Roger")] == Long(43)).Assert().Do();
						(age[String("Bob")] == Long(43)).Assert().Do();
						(age[String("Ken")] == Long(55)).Assert().Do();

						(age.CountIf(Lambda(X, X == Long(43))) == count43).Assert().Do();
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

		TEST_METHOD(TestVector)
		{
			Initialize();
			{
				Manager manager("Test Vector", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE_VALUE(fib, Vector());

				(fib[Long(0)] == Long(1)).Assert().Do();
				(fib[Long(1)] == Long(1)).Assert().Do();
				(fib[Long(2)] == Long(2)).Assert().Do();
				(fib[Long(3)] == Long(3)).Assert().Do();
				(fib[Long(4)] == Long(5)).Assert().Do();
				(fib[Long(5)] == Long(8)).Assert().Do();
				(fib[Long(6)] == Long(13)).Assert().Do();

				(fib[Long(0)] == Long(1)).Evaluate().Do();
				(fib[Long(1)] == Long(1)).Evaluate().Do();
				(fib[Long(2)] == Long(2)).Evaluate().Do();
				(fib[Long(3)] == Long(3)).Evaluate().Do();
				(fib[Long(4)] == Long(5)).Evaluate().Do();
				(fib[Long(5)] == Long(8)).Evaluate().Do();
				(fib[Long(6)] == Long(13)).Evaluate().Do();

				SN_DECLARE(Z);
				(fib[Long(6)] == Z).Assert().Do();
				(Z == Long(13)).Evaluate().Do();
				string Z_text = Z.GetVariableValue().DisplayValueSN();
				Assert::IsTrue(Z_text == "Long(13)");

				fib.Fix();

				SN_DECLARE(Y);
				(fib[Y] == Long(1)).Assert().Do();
				(Y.BuildSet() == (Long(0) || Long(1)).BuildSet()).Evaluate().Do();

				string Y_text = Y.BuildSet().DoEvaluate().DisplaySN();
				Assert::IsTrue(Y_text == "{Long(0), Long(1)}");

				SN_DECLARE(countAll);

				(fib.CountAll() == countAll).Assert().Do();
				(countAll == Long(7)).Evaluate().Do();

				SN_DECLARE(countGreater2);
				SN_DECLARE(X);

				(fib.CountIf(Lambda(X, X > Long(2))) == countGreater2).Assert().Do();
				(countGreater2 == Long(4)).Evaluate().Do();

				SN_DECLARE(sum);

				(fib.Sum() == sum).Assert().Do();
				(sum == Long(33)).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestDerived)
		{
			Initialize();
			{
				Manager manager("Test Derived", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE_VALUE(fib, Derived());
				SN_DECLARE(N);
				(fib(Long(0)) == Long(1)).PartialAssert().Do();
				(fib(Long(1)) == Long(1)).PartialAssert().Do();
				(Define(fib)(N) == (N >Long(1)).If(fib(N - Long(1)) + fib(N - Long(2)))).PartialAssert().Do();
				
				fib.Fix();

				(fib(Long(0)) == Long(1)).Evaluate().Do();
				(fib(Long(1)) == Long(1)).Evaluate().Do();
				(fib(Long(2)) == Long(2)).Evaluate().Do();
				(fib(Long(3)) == Long(3)).Evaluate().Do();
				(fib(Long(4)) == Long(5)).Evaluate().Do();
				(fib(Long(5)) == Long(8)).Evaluate().Do();
				(fib(Long(6)) == Long(13)).Evaluate().Do();

				(fib(Long(0)) == Long(1)).Assert().Do();
				(fib(Long(1)) == Long(1)).Assert().Do();
				(fib(Long(2)) == Long(2)).Assert().Do();
				(fib(Long(3)) == Long(3)).Assert().Do();
				(fib(Long(4)) == Long(5)).Assert().Do();
				(fib(Long(5)) == Long(8)).Assert().Do();
				(fib(Long(6)) == Long(13)).Assert().Do();

			}
			Cleanup();
		}

		TEST_METHOD(TestVirtual)
		{
			return;
			Initialize();
			{
				Manager manager("Test Virtual", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE_VALUE(fib, Virtual());
				SN_DECLARE(N);
				(fib(Long(0)) == Long(1)).PartialAssert().Do();
				(fib(Long(1)) == Long(1)).PartialAssert().Do();
				(Define(fib)(N) == fib(N - Long(1)) + fib(N - Long(2))).PartialAssert().Do();

				fib.Fix();

				(fib(Long(0)) == Long(1)).Evaluate().Do();
				(fib(Long(1)) == Long(1)).Evaluate().Do();
				(fib(Long(2)) == Long(2)).Evaluate().Do();
				(fib(Long(3)) == Long(3)).Evaluate().Do();
				(fib(Long(4)) == Long(5)).Evaluate().Do();
				(fib(Long(5)) == Long(8)).Evaluate().Do();
				(fib(Long(6)) == Long(13)).Evaluate().Do();

				(fib(Long(0)) == Long(1)).Assert().Do();
				(fib(Long(1)) == Long(1)).Assert().Do();
				(fib(Long(2)) == Long(2)).Assert().Do();
				(fib(Long(3)) == Long(3)).Assert().Do();
				(fib(Long(4)) == Long(5)).Assert().Do();
				(fib(Long(5)) == Long(8)).Assert().Do();
				(fib(Long(6)) == Long(13)).Assert().Do();

			}
			Cleanup();
		}

		TEST_METHOD(TestVirtualPolymorphic1)
		{
			Initialize();
			{
				Manager manager("Test Virtual Polymorphic2", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE_VALUE(typeChecker, Virtual());
				SN_DECLARE_VALUE(shortType, Short::Class());

				(typeChecker(shortType) == String("short")).PartialAssert().Do();

				string sTypeChecker_before = typeChecker.DisplayValueSN();
				typeChecker.Fix();
				string sTypeChecker_after = typeChecker.DisplayValueSN();

				// Polymorphic call.
				(typeChecker(Short(1)) == String("short")).Evaluate().Do();

				SN_DECLARE(A);

				(typeChecker(Short(1)) == A).Assert().Do();

				string A_text = A.GetString();

				Assert::IsTrue(A_text == "short");
			}
			Cleanup();
		}

		TEST_METHOD(TestVirtualPolymorphic2)
		{
			Initialize();
			{
				Manager manager("Test Virtual Polymorphic 2", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE_VALUE(typeChecker, Virtual());
				SN_DECLARE_VALUE(shortType, Short::Class());
				SN_DECLARE_VALUE(longType, Long::Class());

				(typeChecker(shortType) == String("short")).PartialAssert().Do();
				(typeChecker(longType) == String("long")).PartialAssert().Do();

				string sTypeChecker_before = typeChecker.DisplayValueSN();
				typeChecker.Fix();
				string sTypeChecker_after = typeChecker.DisplayValueSN();

				// Polymorphic call.
				(typeChecker(Short(1)) == String("short")).Evaluate().Do();
				(typeChecker(Long(1)) == String("long")).Evaluate().Do();

				(typeChecker(Short(1)) == String("short")).Assert().Do();
				(typeChecker(Long(1)) == String("long")).Assert().Do();

				SN_DECLARE(A);
				SN_DECLARE(B);

				(typeChecker(Short(1)) == A).Assert().Do();
				(typeChecker(Long(1)) == B).Assert().Do();

				string A_text = A.GetString();
				string B_text = B.GetString();

				Assert::IsTrue(A_text == "short");
				Assert::IsTrue(B_text == "long");
			}
			Cleanup();
		}

		TEST_METHOD(TestVirtualPolymorphic3)
		{
			Initialize();
			{
				Manager manager("Test Virtual Polymorphic3", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE_VALUE(typeChecker, Virtual());
				SN_DECLARE_VALUE(shortType, Short::Class());
				SN_DECLARE_VALUE(longType, Long::Class());
				SN_DECLARE_VALUE(longLongType, LongLong::Class());

				(typeChecker(shortType) == String("short")).PartialAssert().Do();
				(typeChecker(longType) == String("long")).PartialAssert().Do();
				(typeChecker(longLongType) == String("long long")).PartialAssert().Do();

				string sTypeChecker_before = typeChecker.DisplayValueSN();
				typeChecker.Fix();
				string sTypeChecker_after = typeChecker.DisplayValueSN();

				// Polymorphic call.
				(typeChecker(Short(1)) == String("short")).Evaluate().Do();
				(typeChecker(Long(1)) == String("long")).Evaluate().Do();
				(typeChecker(LongLong(1)) == String("long long")).Evaluate().Do();

				(typeChecker(Short(1)) == String("short")).Assert().Do();
				(typeChecker(Long(1)) == String("long")).Assert().Do();
				(typeChecker(LongLong(1)) == String("long long")).Assert().Do();

				SN_DECLARE(A);
				SN_DECLARE(B);
				SN_DECLARE(C);

				(typeChecker(Short(1)) == A).Assert().Do();
				(typeChecker(Long(1)) == B).Assert().Do();
				(typeChecker(LongLong(1)) == C).Assert().Do();

				string A_text = A.GetString();
				string B_text = B.GetString();
				string C_text = C.GetString();

				Assert::IsTrue(A_text == "short");
				Assert::IsTrue(B_text == "long");
				Assert::IsTrue(C_text == "long long");
			}
			Cleanup();
		}

		TEST_METHOD(TestVirtualPolymorphicReverse1)
		{
			Initialize();
			{
				Manager manager("Test Virtual Polymorphic Reverse 1", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE_VALUE(typeChecker, Virtual());
				SN_DECLARE_VALUE(shortType, Short::Class());

				(typeChecker(shortType) == String("short")).PartialAssert().Do();

				string sTypeChecker_before = typeChecker.DisplayValueSN();
				typeChecker.Fix();
				string sTypeChecker_after = typeChecker.DisplayValueSN();

				// Reverse Polymorphic call.
				SN_DECLARE_VALUE(numbers1, Short(1) || Long(2));
				string n1_text_before = numbers1.DoEvaluate().DisplayValueSN();

				(typeChecker(numbers1) == String("short")).Assert().Do();
				string n1_text = numbers1.DoEvaluate().DisplayValueSN();
				Assert::IsTrue(n1_text == "Short(1)");
				(numbers1 == Short(1)).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestVirtualPolymorphicReverse2)
		{
			Initialize();
			{
				Manager manager("Test Virtual Polymorphic Reverse 2", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE_VALUE(typeChecker, Virtual());
				SN_DECLARE_VALUE(shortType, Short::Class());
				SN_DECLARE_VALUE(longType, Long::Class());

				(typeChecker(shortType) == String("short")).PartialAssert().Do();
				(typeChecker(longType) == String("long")).PartialAssert().Do();

				string sTypeChecker_before = typeChecker.DisplayValueSN();
				typeChecker.Fix();
				string sTypeChecker_after = typeChecker.DisplayValueSN();

				// Reverse Polymorphic call.
				//SN_DECLARE_VALUE(numbers1, Short(1) || Long(2));
				SN_DECLARE(numbers1);
				(numbers1 == (Short(1) || Long(2))).PartialAssert().Do();

				string n1_text_before = numbers1.DoEvaluate().DisplayValueSN();
				(typeChecker(numbers1) == String("short")).Assert().Do();
				string n1_text = numbers1.DoEvaluate().DisplayValueSN();
				Assert::IsTrue(n1_text == "Short(1)");
				(numbers1 == Short(1)).Evaluate().Do();

				SN_DECLARE_VALUE(numbers2, Short(1) || Long(2));
				(typeChecker(numbers2) == String("long")).Assert().Do();
				string n2_text = numbers2.DoEvaluate().DisplayValueSN();
				Assert::IsTrue(n2_text == "Long(2)");
				(numbers2 == Long(2)).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestVirtualPolymorphicReverse3)
		{
			Initialize();
			{
				Manager manager("Test Virtual Polymorphic Reverse3", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE_VALUE(typeChecker, Virtual());
				SN_DECLARE_VALUE(shortType, Short::Class());
				SN_DECLARE_VALUE(longType, Long::Class());
				SN_DECLARE_VALUE(longLongType, LongLong::Class());

				(typeChecker(shortType) == String("short")).PartialAssert().Do();
				(typeChecker(longType) == String("long")).PartialAssert().Do();
				(typeChecker(longLongType) == String("long long")).PartialAssert().Do();

				string sTypeChecker_before = typeChecker.DisplayValueSN();
				typeChecker.Fix();
				string sTypeChecker_after = typeChecker.DisplayValueSN();

				// Reverse Polymorphic call.
				SN_DECLARE(numbers1);
				(numbers1 == (Short(1) || Long(2) || LongLong(3))).PartialAssert().Do();
				string numbers1_text = numbers1.DisplayValueSN();
				(typeChecker(numbers1) == String("short")).Assert().Do();
				string n1_text = numbers1.GetValue().DisplayValueSN();
				Assert::IsTrue(n1_text == "Short(1)");
				(numbers1 == Short(1)).Evaluate().Do();

				SN_DECLARE(numbers2);
				(numbers2 == (Short(1) || Long(2) || LongLong(3))).PartialAssert().Do();
				(typeChecker(numbers2) == String("long")).Assert().Do();
				string n2_text = numbers2.GetValue().DisplayValueSN();
				Assert::IsTrue(n2_text == "Long(2)");
				(numbers2 == Long(2)).Evaluate().Do();

				SN_DECLARE(numbers3);
				(numbers3 == (Short(1) || Long(2) || LongLong(3))).PartialAssert().Do();
				(typeChecker(numbers3) == String("long long")).Assert().Do();
				string n3_text = numbers3.GetValue().DisplayValueSN();
				Assert::IsTrue(n3_text == "LongLong(3)");
				(numbers3 == LongLong(3)).Evaluate().Do();
			}
			Cleanup();
		}
	};
}