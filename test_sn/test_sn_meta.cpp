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

		static void AssertErrorHandler(bool p_Err, const string& p_Description)
		{
			Assert::IsTrue(!p_Err, wstring(p_Description.begin(), p_Description.end()).c_str());
		}

		void Initialize()
		{
		}

		void Cleanup()
		{
		}

	public:
		TEST_METHOD(TestMetaEvaluate)
		{
			Initialize();
			{
				Manager manager("Test Meta Evaluate", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE_VALUE(dog, String("Vincent "));
				SN_DECLARE_VALUE(cat, String("Emma "));

				Assert::IsTrue(Meta(1, String("Vincent ") + String("Emma ")).DoEvaluate().Equivalent(Meta(1, String("Vincent ") + String("Emma "))));
				Assert::IsTrue(Meta(1, dog + cat).DoEvaluate().Equivalent(Meta(1, dog + cat)));

				string s1 = Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoEvaluate().DisplaySN();

				Assert::IsTrue(Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoEvaluate().Equivalent(Meta(1, String("Vincent ") + String("Emma "))));

				string s2 = Meta(-1, Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoEvaluate()).DoEvaluate().DisplaySN();

				Assert::IsTrue(Meta(-1, Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoEvaluate()).DoEvaluate().Equivalent(String("Vincent Emma ")));

				string s3 = Meta(1, Meta(-1, dog)).DoEvaluate().DisplaySN();

				Assert::IsTrue(Meta(1, Meta(-1, dog)).DoEvaluate().Equivalent(String("Vincent ")));

				SN_DECLARE_VALUE(e, Meta(2, Meta(-1, dog) + Meta(-1, cat)));

				string s4 = e.DoEvaluate().DisplaySN();

				Assert::IsTrue(e.DoEvaluate().Equivalent(Meta(2, Meta(-1, dog) + Meta(-1, cat))));

				string s5 = Meta(-1, e).DoEvaluate().DisplaySN();


				Assert::IsTrue(Meta(-1, e).DoEvaluate().Equivalent(Meta(1, String("Vincent ") + String("Emma "))));
				Assert::IsTrue(e.DoQuery(-1).Equivalent(Meta(1, String("Vincent ") + String("Emma "))));

				string s6 = Meta(-1, Meta(-1, e).DoEvaluate()).DoEvaluate().DisplaySN();

				Assert::IsTrue(Meta(-1, Meta(-1, e).DoEvaluate()).DoEvaluate().Equivalent(String("Vincent Emma ")));
				Assert::IsTrue(e.DoQuery(-2).Equivalent(String("Vincent Emma ")));

				Assert::IsTrue(Meta(1, Meta(-1, dog)).DoEvaluate().Equivalent(String("Vincent ")));
			}
			Cleanup();
		}

		TEST_METHOD(TestMetaPartialEvaluateOnValues)
		{
			Initialize();
			{
				Manager manager("Test Meta Partial Evaluate On Values", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE_VALUE(dog, String("Vincent "));
				SN_DECLARE_VALUE(cat, String("Emma "));

				Assert::IsTrue(Meta(1, String("Vincent ") + String("Emma ")).DoPartialEvaluate().Equivalent(Meta(1, String("Vincent ") + String("Emma "))));

				string s1 = Meta(1, dog + cat).DoPartialEvaluate().DisplaySN();

				Assert::IsTrue(Meta(1, dog + cat).DoPartialEvaluate().Equivalent(Meta(1, dog + cat)));

				string s2 = Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoPartialEvaluate().DisplaySN();

				Assert::IsTrue(Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoPartialEvaluate().Equivalent(Meta(1, String("Vincent ") + String("Emma "))));

				string s3 = Meta(-1, Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoPartialEvaluate()).DoPartialEvaluate().DisplaySN();

				Assert::IsTrue(Meta(-1, Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoPartialEvaluate()).DoPartialEvaluate().Equivalent(String("Vincent Emma ")));

				string s4 = Meta(1, Meta(-1, dog)).DoPartialEvaluate().DisplaySN();

				Assert::IsTrue(Meta(1, Meta(-1, dog)).DoPartialEvaluate().Equivalent(String("Vincent ")));

				SN_DECLARE_VALUE(e, Meta(2, Meta(-1, dog) + Meta(-1, cat)));

				string s5 = e.DoPartialEvaluate().DisplaySN();

				Assert::IsTrue(e.DoPartialEvaluate().Equivalent(Meta(2, Meta(-1, dog) + Meta(-1, cat))));

				string s6 = Meta(-1, e).DoPartialEvaluate().DisplaySN();

				Assert::IsTrue(Meta(-1, e).DoPartialEvaluate().Equivalent(Meta(1, String("Vincent ") + String("Emma "))));
				Assert::IsTrue(e.DoReduce(-1).Equivalent(Meta(1, String("Vincent ") + String("Emma "))));

				string s7 = Meta(-1, Meta(-1, e).DoPartialEvaluate()).DoPartialEvaluate().DisplaySN();

				Assert::IsTrue(Meta(-1, Meta(-1, e).DoPartialEvaluate()).DoPartialEvaluate().Equivalent(String("Vincent Emma ")));
				Assert::IsTrue(e.DoReduce(-2).Equivalent(String("Vincent Emma ")));
			}
			Cleanup();
		}

		TEST_METHOD(TestMetaPartialEvaluate)
		{
			Initialize();
			{
				Manager manager("Test Meta Partial Evaluate", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(dog);
				SN_DECLARE(cat);
				SN_DECLARE_VALUE_INLINE(maxDog, String("Max "));
				SN_DECLARE_VALUE_INLINE(chiChiCat, String("Chi Chi"));

				Assert::IsTrue(Meta(1, String("Vincent ") + String("Emma ")).DoPartialEvaluate().Equivalent(Meta(1, String("Vincent ") + String("Emma "))));

				string s0 = Meta(1, maxDog + chiChiCat).DoPartialEvaluate().DisplaySN();

				Assert::IsTrue(Meta(1, maxDog + chiChiCat).DoPartialEvaluate().Equivalent(Meta(1, maxDog + chiChiCat)));

				string s1 = Meta(1, Meta(-1, maxDog) + Meta(-1, chiChiCat)).DoPartialEvaluate().DisplaySN();

				Assert::IsTrue(Meta(1, Meta(-1, maxDog) + Meta(-1, chiChiCat)).DoPartialEvaluate().Equivalent(Meta(1, String("Max ") + String("Chi Chi"))));

				string s2 = Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoPartialEvaluate().DisplaySN();

				Assert::IsTrue(Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoPartialEvaluate().Equivalent(Meta(1, Meta(-1, dog) + Meta(-1, cat))));

				string s3 = Meta(-1, Meta(1, Meta(-1, maxDog) + Meta(-1, chiChiCat)).DoPartialEvaluate()).DoPartialEvaluate().DisplaySN();

				Assert::IsTrue(Meta(-1, Meta(1, Meta(-1, maxDog) + Meta(-1, chiChiCat)).DoPartialEvaluate()).DoPartialEvaluate().Equivalent(String("Max Chi Chi")));

				string s4 = Meta(-1, Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoPartialEvaluate()).DoPartialEvaluate().DisplaySN();

				Assert::IsTrue(Meta(-1, Meta(1, Meta(-1, dog) + Meta(-1, cat)).DoPartialEvaluate()).DoPartialEvaluate().Equivalent(Function(Function(Add, dog), cat)));

				string s5 = Meta(1, Meta(-1, dog)).DoPartialEvaluate().DisplaySN();

				Assert::IsTrue(Meta(1, Meta(-1, dog)).DoPartialEvaluate().Equivalent(dog));

				SN_DECLARE_VALUE_INLINE(e, Meta(2, Meta(-1, maxDog) + Meta(-1, chiChiCat)));

				string s6 = e.DoPartialEvaluate().DisplaySN();

				Assert::IsTrue(e.DoPartialEvaluate().Equivalent(Meta(2, Meta(-1, maxDog) + Meta(-1, chiChiCat))));

				string s7 = e.DoReduce(-1).DisplaySN();

				Assert::IsTrue(e.DoReduce(-1).Equivalent(Meta(1, String("Max ") + String("Chi Chi"))));

				string s8 = Meta(-1, Meta(-1, e).DoPartialEvaluate()).DoPartialEvaluate().DisplaySN();
				string s9 = e.DoPartialEvaluate(-2).DoPartialEvaluate().DisplaySN();

				Assert::IsTrue(Meta(-1, Meta(-1, e).DoPartialEvaluate()).DoPartialEvaluate().Equivalent(String("Max Chi Chi")));
				Assert::IsTrue(e.DoPartialEvaluate(-2).Equivalent(String("Max Chi Chi")));

				SN_DECLARE_VALUE_INLINE(f, Meta(2, Meta(-1, dog) + Meta(-1, cat)));

				string s10 = f.DoPartialEvaluate().DisplaySN();
				string s11 = f.DoReduce(-2).DoPartialEvaluate().DisplaySN();

				Assert::IsTrue(Meta(-1, Meta(-1, f).DoPartialEvaluate()).DoPartialEvaluate().Equivalent(Function(Function(Add, dog), cat)));
				Assert::IsTrue(f.DoReduce(-2).Equivalent(Function(Function(Add, dog), cat)));
			}
			Cleanup();
		}
	};
}