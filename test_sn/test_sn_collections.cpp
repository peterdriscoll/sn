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
	TEST_CLASS(test_sn_collections)
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
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
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
						age.Fix(Long(0));
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

		TEST_METHOD(TestSet)
		{
			Initialize();
			{
				Manager manager("Test Set", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(X);
				SN_DECLARE(Y);
				(X == Set({ Long(3), Long(4), Long(5) })).Assert().Do();
				(Y == (Long(3) || Long(4) || Long(5)).BuildSet()).Assert().Do();
				(X == Y).Assert().Do();

				string X_text = X.DisplayValueSN();
				Assert::IsTrue(X_text == "Set({Long(3), Long(4), Long(5)})");

				string Y_text = Y.DisplayValueSN();
				Assert::IsTrue(Y_text == "Set({Long(3), Long(4), Long(5)})");
				
				((X.HasMember(Long(3)))).Assert();
				((X.HasMember(Long(4)))).Assert();
				((X.HasMember(Long(5)))).Assert();

				((Y.HasMember(Long(3)))).Assert();
				((Y.HasMember(Long(4)))).Assert();
				((Y.HasMember(Long(5)))).Assert();
			}

			Cleanup();
		}
	};
}