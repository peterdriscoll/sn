#include "sn.h"

#include <cstdio>
#include <fstream>

#include "test_sn_pch.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace std;
using namespace skynet;
using namespace PGCX;

namespace test_sn
{
	TEST_CLASS(test_sn_imperative)
	{
	private:
		const string doc_root = "C:/Users/peter_driscoll/Documents/Source/Repos/skynet2/html";

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
		TEST_METHOD(TestFactorial)
		{
			Initialize();
			{
				Manager manager("Test Factorial", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				{
					SN_DECLARE(Assignment);
					SN_DECLARE(l);
					SN_DECLARE(r);

					Expression x;
					x.Copy(l);

					Variable y(string("sheep"));
					Expression b((x = r));
					Expression c((y = r));
					Expression a((l = r).ImperativeTransform());
					(Define(Assignment)(l)(r) == ((l = r).ImperativeTransform())).PartialAssert().Do();
					string sAssignment = Assignment.DisplayValueSN();
				}
				SN_DECLARE(While);
				SN_DECLARE(c);
				SN_DECLARE(a);
				string s = (Define(While)(c)(a) == (Stateful(Mutable, c).If(Stateful(Mutable, a).Compose(Stateful(Mutable, While(c)(a))), False)).ImperativeTransform()).DisplaySN();

				(Define(While)(c)(a) == (Stateful(Mutable, c).If(Stateful(Mutable, a).Compose(Stateful(Mutable, While(c)(a))), True)).ImperativeTransform()).PartialAssert().Do();

				string sWhile = While.DisplayValueSN();

				SN_DECLARE(Fact);
				SN_DECLARE(m);
				SN_DECLARE(n);
				SN_DECLARE(j);

//				(Define(Fact)(m) == Local(n, Local(j,
//					(j = m).Compose(n = Long(1)).Compose(While(j > Long(0))((n = n * j).Compose(j = j - Long(1)))
//					)))).PartialAssert().Do();
			}
			Cleanup();
		}
		/*

				(Fact(Long(0)) == Long(1)).Assert().Do();
*/
	};
}