#include "snl.h"

#include <cstdio>
#include <fstream>

#include "test_sn_pch.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


using namespace skynet;
using namespace PGCX;

namespace test_sn
{
	TEST_CLASS(test_sn_factorial)
	{
	private:
		bool runWebServer = false;

		static void AssertErrorHandler(bool p_Err, const std::string& p_Description)
		{
			Assert::IsTrue(!p_Err, std::wstring(p_Description.begin(), p_Description.end()).c_str());
		}

		void Initialize()
		{
		}

		void Cleanup()
		{
		}

	public:
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
					std::string s_RemovePrefix = RemovePrefix.GetVariableValue().DisplaySN();
					Assert::IsTrue(s_RemovePrefix == "@p.@x.SubtractLeft x p");
					std::string v_RemovePrefix = Lambda(p, Lambda(x, x.SubtractLeft(p))).DoPartialEvaluate().DisplaySN();
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
					((Fact(Long(0)) == Long(1)) && (Fact(k).Condition(Lambda(n, n > Long(0))) == k * Fact(k - Long(1)).Condition(Lambda(n, n < Long(0))))).PartialAssert().Do();
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestYCombinator)
		{
			Initialize();
			{
				Manager manager("Test Y Combinator", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				if (manager.GetEvaluationType() == skynet::Strict)
				{
					return;
				}

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
	};
}