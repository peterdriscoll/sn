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
		TEST_METHOD(TestSetEvaluate)
		{
			Initialize();
			{
				Manager manager("Test Set Evaluate", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

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

		TEST_METHOD(TestAssertThrow)
		{
			Initialize();
			{
				Manager manager("Test Assert Throw", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
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