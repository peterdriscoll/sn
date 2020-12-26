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
	TEST_CLASS(test_sn_meta)
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
		TEST_METHOD(TestMetaEvaluate)
		{
			Initialize();
			{
				Manager manager("Test Meta Evaluate", AssertErrorHandler);

				SN_DECLARE_VALUE(dog, String("Vincent "));
				SN_DECLARE_VALUE(cat, String("Emma "));

				Assert::IsTrue(Meta(1, String("Vincent ") + String("Emma ")).DoEvaluate().Equivalent(Meta(1, String("Vincent ") + String("Emma "))));
				Assert::IsTrue(Meta(1, dog + cat).DoEvaluate().Equivalent(Meta(1, dog + cat)));

				string s1 = Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoEvaluate().DisplaySN();

				Assert::IsTrue(Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoEvaluate().Equivalent(Meta(1, String("Vincent ") + String("Emma "))));

				string s2 = Meta(-1, Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoEvaluate()).DoEvaluate().DisplaySN();

				Assert::IsTrue(Meta(-1, Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoEvaluate()).DoEvaluate().Equivalent(String("Vincent Emma ")));

				string s3 = Meta(1, Meta(-1, dog)).DoEvaluate().DisplaySN();

				Assert::IsTrue(Meta(1, Meta(-1, dog)).DoEvaluate().Equivalent(Meta(1, dog.DoEvaluate())));

				SN_DECLARE_VALUE(e, Meta(2, Meta(-1, dog) + Meta(-1, cat)));

				string s4 = e.DoEvaluate().DisplaySN();

				Assert::IsTrue(e.DoEvaluate().Equivalent(Meta(2, Meta(-1, dog) + Meta(-1, cat))));

				string s5 = Meta(-1, e).DoEvaluate().DisplaySN();


				Assert::IsTrue(Meta(-1, e).DoEvaluate().Equivalent(Meta(1, String("Vincent ") + String("Emma "))));
				Assert::IsTrue(e.DoEvaluate(-1).Equivalent(Meta(1, String("Vincent ") + String("Emma "))));

				string s6 = Meta(-1, Meta(-1, e).DoEvaluate()).DoEvaluate().DisplaySN();

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

				Assert::IsTrue(Meta(1, String("Vincent ") + String("Emma ")).DoPartialEvaluate().Equivalent(Meta(1, String("Vincent ") + String("Emma "))));

				std::cout << std::endl << Meta(1, dog + cat).DoPartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(1, dog + cat).DoPartialEvaluate().Equivalent(Meta(1, dog + cat)));

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

				Assert::IsTrue(Meta(1, String("Vincent ") + String("Emma ")).DoPartialEvaluate().Equivalent(Meta(1, String("Vincent ") + String("Emma "))));

				std::cout << std::endl << Meta(1, dog + cat).DoPartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(1, dog + cat).DoPartialEvaluate().Equivalent(Meta(1, dog + cat)));

				std::cout << std::endl << Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoPartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoPartialEvaluate().Equivalent(Meta(1, dog + cat)));

				string s1 = Meta(-1, Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoPartialEvaluate()).DoPartialEvaluate().DisplaySN();

				Assert::IsTrue(Meta(-1, Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoPartialEvaluate()).DoPartialEvaluate().Equivalent(Function(Function(Add, dog), cat)));

				string s2 = Meta(1, Meta(-1, dog)).DoPartialEvaluate().DisplaySN();

				Assert::IsTrue(Meta(1, Meta(-1, dog)).DoPartialEvaluate().Equivalent(Meta(1, dog.DoPartialEvaluate())));

				SN_DECLARE_VALUE(e, Meta(2, Meta(-1, dog) + Meta(-1, cat)));

				std::cout << std::endl << e.DoPartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(e.DoPartialEvaluate().Equivalent(Meta(2, Meta(-1, dog) + Meta(-1, cat))));

				std::cout << std::endl << Meta(-1, e).DoPartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(-1, e).DoPartialEvaluate().Equivalent(Meta(1, dog + cat)));
				Assert::IsTrue(e.DoPartialEvaluate(-1).Equivalent(Meta(1, dog + cat)));

				std::cout << std::endl << Meta(-1, Meta(-1, e).DoPartialEvaluate()).DoPartialEvaluate().DisplaySN() << std::endl;

				Assert::IsTrue(Meta(-1, Meta(-1, e).DoPartialEvaluate()).DoPartialEvaluate().Equivalent(Function(Function(Add, dog), cat)));
				Assert::IsTrue(e.DoPartialEvaluate(-1).DoPartialEvaluate(-1).Equivalent(Function(Function(Add, dog), cat)));
			}
			Cleanup();
		}
	};
}