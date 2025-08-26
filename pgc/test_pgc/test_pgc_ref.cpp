#include "pgc.h"

#include "test_pgc_pch.h"
#include "CppUnitTest.h"

#include <thread>
//#include <barrier>
#include <atomic>
#include <vector>
#include <chrono>
#include <string>

#include "testpgc_a.h"
#include "testpgc_b.h"
#include "test_pgc_c.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test_pgc_ref
{
	void AssertErrorHandler(bool p_Err, const std::string& p_Description)
	{
		Assert::IsTrue(!p_Err, std::wstring(p_Description.begin(), p_Description.end()).c_str());
	};
	
	void ThrowErrorHandler(bool p_Err, const std::string& p_Description)
	{
		if (p_Err)
		{
			throw PGC::PGC_Exception(p_Description);
		}
	}

	TEST_CLASS(test_pgc_ref)
	{

	public:
		// TESTS.

		TEST_METHOD(TestPromotionResult_Ref_Dropped_DyingDestination)
		{
			PGC_User user;
			{
				PGCX::PGC_Transaction source(user, false, PGC::PromotionStrategy::DoubleDipping);

				TestClassUsingRef_B* dyingDestination= new TestClassUsingRef_B();

				Assert::IsTrue(user.TotalNetMemoryUsed() == sizeof(TestClassUsingRef_B)- PGC_OVERHEAD);
				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					source.GrossMemoryUsed() + 
					user.TotalPromotionMemory(),
					L"Gross memory error");

				{
					PGCX::PGC_Transaction destination(user, false, PGC::PromotionStrategy::DoubleDipping);
					Ref<TestClassUsingRef_B> ref(dyingDestination, &destination);

					Assert::IsTrue(user.TotalNetMemoryUsed() ==
						sizeof(TestClassUsingRef_B) - PGC_OVERHEAD, L"Net memory error");
					Assert::IsTrue(user.TotalGrossMemoryUsed() ==
						source.GrossMemoryUsed() +
						destination.GrossMemoryUsed() +
						user.TotalPromotionMemory(),
						L"Gross memory error");
				}
				Assert::IsTrue(user.TotalNetMemoryUsed() ==
					sizeof(TestClassUsingRef_B) - PGC_OVERHEAD, L"Net memory error");
				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					source.GrossMemoryUsed() +
					user.TotalPromotionMemory(),
					L"Gross memory error");
			}
			Assert::IsTrue(user.TotalNetMemoryUsed() ==
				0, L"Net memory error");
			Assert::IsTrue(user.TotalGrossMemoryUsed() ==
				user.TotalPromotionMemory(),
				L"Gross memory error");
		}

		TEST_METHOD(TestPromotionResult_Ref_PromotedKeep_DoubleDipping)
		{
			PGC_User user;

			{
				// DESTINATION transaction (ref lives here)
				PGCX::PGC_Transaction destination(user, false, PGC::PromotionStrategy::DoubleDipping);

				Ref<TestClassUsingRef_B> ref;

				Assert::IsTrue(user.TotalNetMemoryUsed() == 0, L"Initial net memory");
				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					destination.GrossMemoryUsed() + user.TotalPromotionMemory(),
					L"Initial gross memory");

				{
					// SOURCE transaction (allocates object)
					PGCX::PGC_Transaction source(user, false, PGC::PromotionStrategy::DoubleDipping);

					Assert::IsTrue(user.TotalNetMemoryUsed() == 0, L"Net memory after source start");

					Assert::IsTrue(user.TotalGrossMemoryUsed() ==
						source.GrossMemoryUsed() +
						destination.GrossMemoryUsed() +
						user.TotalPromotionMemory(),
						L"Gross memory after source start");

					// Allocate in source, ref is owned by destination
					ref.Set(new TestClassUsingRef_B());

					Assert::IsTrue(user.TotalNetMemoryUsed() == sizeof(TestClassUsingRef_B) - PGC_OVERHEAD, L"Net memory after allocation");

					Assert::IsTrue(user.TotalGrossMemoryUsed() ==
						source.GrossMemoryUsed() +
						destination.GrossMemoryUsed() +
						user.TotalPromotionMemory(),
						L"Gross memory after allocation");

					// Simulate use of value in destination. Value accessed via the promotion.
					TestClassUsingRef_B* value = ref.Get();

					Assert::IsNotNull(value, L"ref.Get() returned null");

					Assert::IsTrue(user.TotalProcessedDoubleDippingMemory() == 0,
						L"Processed double dipping memory should be zero");
				}
				// This should have triggered PromotedKeep behavior
				Assert::IsTrue(user.TotalProcessedDoubleDippingMemory() == sizeof(PGC::PGC_Promotion),
					L"Processed double dipping memory should be updated");

				// ref.Get() called -> triggers freeing of the double dipped promotion (the second dip).
				TestClassUsingRef_B* value2 = ref.Get();

				Assert::IsTrue(user.TotalProcessedDoubleDippingMemory() == 0,
					L"Processed double dipping memory should be zero after free");

				Assert::IsTrue(destination.NetMemoryUsed() == sizeof(TestClassUsingRef_B) - PGC_OVERHEAD, L"Destination owns value");
				Assert::IsTrue(user.TotalNetMemoryUsed() == sizeof(TestClassUsingRef_B) - PGC_OVERHEAD, L"Net memory after promotion");

				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					destination.GrossMemoryUsed() +
					user.TotalPromotionMemory(),
					L"Gross memory after promotion");
			}

			// After destination dies TestClassUsingRef_B is deleted
			Assert::IsTrue(user.TotalNetMemoryUsed() == 0, L"Net memory after cleanup");

			Assert::IsTrue(user.TotalGrossMemoryUsed() == user.TotalPromotionMemory(),
				L"Gross memory after cleanup");
		}

	};
}

