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
	TEST_CLASS(test_sn_stringanchorright)
	{
	private:
		bool runWebServer = false;

		static void AssertErrorHandler(bool p_Err, const string& p_Description)
		{
			wstring errDesciption(p_Description.begin(), p_Description.end());
			Assert::IsTrue(!p_Err, errDesciption.c_str());
		}

		void Initialize()
		{
		}

		void Cleanup()
		{
		}

	public:
		TEST_METHOD(TestRightAnchoredStringRefPositive)
		{
			Initialize();
			{
				Manager manager("Test Right-Anchored Positive", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				{
					Transaction transaction;

					SN_DECLARE(start);

					// Known end, unknown start: right-anchored StringRef
					StringRef ref(String("abcdef"), start, Long(6));

					// Assert that the anchored slice equals "ef"
					(String("ef") == ref).Assert().Do();

					Long start_Long = start.GetSafeValue();
					long start_number = start_Long.GetNumber();
					std::wstring msg = std::wstring(L"Start should be 4. Found ") + ToString(start_number);
					Assert::IsTrue(start_number == 4, msg.c_str());

					// Now start should have been resolved to 4
					(start == Long(4)).Assert().Do();
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestRightAnchoredStringRefMismatch)
		{
			Initialize();
			{
				Manager manager("Test Right-Anchored Mismatch", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				{
					Transaction transaction;

					SN_DECLARE(start);

					// Known end, unknown start: right-anchored StringRef
					StringRef ref(String("abcdef"), start, Long(6));

					// Attempt to match "zz" (which doesn't match the source "abcdef")
					Error err = (String("zz") == ref).Assert().DoReturnError();

					string errDescription = err.GetDescription();
					std::wstring msg = std::wstring(L"Expected mismatch error. Found: ") + ToString(errDescription.c_str());
					Assert::IsTrue(err.IsError(), msg.c_str());

					// Check that the error string contains the mismatch detail
					Assert::IsTrue(
						errDescription.find("Contradiction: Strings do not match") != std::string::npos,
						L"Error should describe the mismatch");
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestRightAnchoredStringRefNegation)
		{
			Initialize();
			{
				Manager manager("Test Right-Anchored Negation", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				{
					Transaction transaction;

					SN_DECLARE(start);

					// Compare a fixed string to a left-anchored stringref with unknown end
					((String("ef") != StringRef(String("abcdef"), start, Long(6)))).Assert().Do();

					// Asserting start == 4 forces:
					//   String("ef") == StringRef(String("abcdef"), start, Long(6))
					// This must fail because the delayed inequality will be violated.
					Error err = (start == Long(4)).Assert().DoReturnError();

					string errDescription = err.GetDescription();
					Assert::IsTrue(err.IsError());
				}
			}
			Cleanup();
		}
	};
}