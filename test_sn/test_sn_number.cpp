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
	TEST_CLASS(test_sn_number)
	{
	private:
		bool runWebServer = false;

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
	};
}