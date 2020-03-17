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
	TEST_CLASS(test_sn)
	{
	private:
		const string doc_root = "C:/Users/peter_driscoll/Documents/Source/Repos/skynet2/html";

		bool runWebServer = false;
		bool runWebServer2 = false;
		bool runWebServer3 = false;

		static void AssertErrorHandler(SN::SN_Error p_Result)
		{
			string description = p_Result.GetDescription();
			Assert::IsTrue(p_Result.GetBool(), wstring(description.begin(), description.end()).c_str());
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

		TEST_METHOD(TestPythagoras)
		{
			Initialize();
			{
				Manager manager("Test Pythagoras", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, false);

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
			return; // Valid test. Needs investigation.
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
		TEST_METHOD(TestComparisons)
		{
			Initialize();
			{
				Manager manager("Test Virtual Polymorphic Reverse3", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				(Long(5) != Long(6)).Assert().Do();
				(Long(5) == Long(5)).Assert().Do();
				(Long(5) < Long(6)).Assert().Do();
				(Long(6) > Long(5)).Assert().Do();
				(Long(5) <= Long(5)).Assert().Do();
				(Long(6) >= Long(6)).Assert().Do();
				(Long(7) >= Long(6)).Assert().Do();

				(String("555") != String("666")).Assert().Do();
				(String("555") == String("555")).Assert().Do();
				(String("555") < String("666")).Assert().Do();
				(String("666") > String("555")).Assert().Do();
				(String("555") <= String("555")).Assert().Do();
				(String("666") >= String("666")).Assert().Do();
				(String("777") >= String("666")).Assert().Do();
			}
			Cleanup();
		}
	};
}