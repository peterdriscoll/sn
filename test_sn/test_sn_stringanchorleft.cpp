#include "snl.h"

#include <cstdio>
#include <fstream>

#include "test_sn_pch.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


using namespace skynet;
using namespace PGCX;

namespace test_sn
{
	TEST_CLASS(test_sn_stringanchorleft)
	{
	private:
		bool runWebServer = false;

		static void AssertErrorHandler(bool p_Err, const std::string& p_Description)
		{
			std::wstring errDesciption(p_Description.begin(), p_Description.end());
			Assert::IsTrue(!p_Err, errDesciption.c_str());
		}

		void Initialize()
		{
		}

		void Cleanup()
		{
		}

	public:
		TEST_METHOD(TestLeftAnchoredStringRefPositive)
		{
			Initialize();
			{
				Manager manager("Test Left-Anchored Positive", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				{
					Transaction transaction;

					SN_DECLARE(end);

					// Known start, unknown end: left-anchored StringRef
					StringRef ref(String("abcdef"), Long(0), end);

					// Assert that the anchored slice equals "ab"
					(String("ab") == ref).Assert().Do();
					
					Long end_Long = end.GetSafeValue();
					long end_number = end_Long.GetNumber();
					std::wstring msg = std::wstring(L"End should be 2. Found ") + ToString(end_number);
					Assert::IsTrue(end_number == 2, msg.c_str());

					// Now end should have been resolved to 2
					(end == Long(2)).Assert().Do();
				}
			}
			Cleanup();
		}
		TEST_METHOD(TestLeftAnchoredStringRefMismatch)
		{
			Initialize();
			{
				Manager manager("Test Left-Anchored Mismatch", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				{
					Transaction transaction;

					SN_DECLARE(end);

					// Known start, unknown end: left-anchored StringRef
					StringRef ref(String("abcdef"), Long(0), end);

					// Attempt to match "zz" (which doesn't match the source "abcdef")
					Error err = (String("zz") == ref).Assert().DoReturnError();

					std::string errDescription = err.GetDescription();
					std::wstring msg = std::wstring(L"Expected mismatch error. Found: ") + ToString(errDescription.c_str());
					Assert::IsTrue(err.IsError(), msg.c_str());

					// Optionally check that the error string contains the mismatch detail
					std::string errDesc = err.GetDescription();
					Assert::IsTrue(errDesc.find("Contradiction: Strings do not match") != std::string::npos,
						L"Error should describe the mismatch");
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestLeftAnchoredStringRefNegation)
		{
			Initialize();
			{
				Manager manager("Test Left-Anchored Negation", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, false /*runWebServer*/);
				{
					Transaction transaction;

					SN_DECLARE(end);

					// Compare a fixed string to a left-anchored stringref with unknown end
					(!(String("ab") == StringRef(String("abcdef"), Long(0), end))).Assert().Do();

					// Asserting end == 2 forces:
					//   String("ab") == StringRef(String("abcdef"), Long(0), end)
					// This must fail because the delayed inequality will be violated.
					Error err = (end == Long(2)).Assert().DoReturnError();

					std::string errDescription = err.GetDescription();
					Assert::IsTrue(err.IsError());
				}
				{
					Transaction transaction;

					SN_DECLARE(end);

					// Compare a fixed string to a left-anchored stringref with unknown end
					((String("ab") != StringRef(String("abcdef"), Long(0), end))).Assert().Do();

					// Asserting end == 2 forces:
					//   String("ab") == StringRef(String("abcdef"), Long(0), end)
					// This must fail because the delayed inequality will be violated.
					manager.Breakpoint();
					Error err = (end == Long(2)).Assert().DoReturnError();

					std::string errDescription = err.GetDescription();
					Assert::IsTrue(err.IsError());
				}
				{
					Transaction transaction;

					SN_DECLARE(end);

					// Compare a fixed string to a left-anchored stringref with unknown end
					(StringRef(String("abcdef"), Long(0), end) != String("ab")).Assert().Do();

					// Asserting end == 2 forces:
					//   String("ab") == StringRef(String("abcdef"), Long(0), end)
					// This must fail because the delayed inequality will be violated.
					Error err = (end == Long(2)).Assert().DoReturnError();

					std::string errDescription = err.GetDescription();
					Assert::IsTrue(err.IsError());
				}
			}
			Cleanup();
		}
	};
}