#include "pgc.h"

#include "testclassusingref_A.h"
#include "testclassusingref_A.h"

#include "test_pgc_pch.h"
#include "CppUnitTest.h"

#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define PGC_CLASS_LIST(ACTION) \
	ACTION(TestClassUsingRef_A) \
	ACTION(TestClassUsingRef_B)

// PGC_DEFINE_CLASS_REGISTRY(ClassRegistry);
const ::PGC::RegEntry ClassRegistry[] =
{
	::PGC::make_entry<TestClassUsingRef_A>("TestClassUsingRef_A"),
	::PGC::make_entry<TestClassUsingRef_B>("TestClassUsingRef_B"),
	::PGC::kEndSentinel
};

namespace test_pgc
{

	TEST_CLASS(test_pgc_ref)
	{
	public:
		static void AssertErrorHandler(bool p_Err, const std::string& p_Description)
		{
			Assert::IsTrue(!p_Err, std::wstring(p_Description.begin(), p_Description.end()).c_str());
		};

		static void ThrowErrorHandler(bool p_Err, const std::string& p_Description)
		{
			if (p_Err)
			{
				throw PGC::PGC_Exception(p_Description);
			}
		}

		// TESTS.
				TEST_METHOD(TestPromotionResult_Ref_Dropped_DyingDestination)
		{
			PGC_User user(ClassRegistry, &AssertErrorHandler);
			{
				Ref<TestClassUsingRef_B> ref1;

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
			PGC_User user(nullptr, &AssertErrorHandler);

			{
				// DESTINATION transaction (ref lives here)
				PGCX::PGC_Transaction destination(user, false, PGC::PromotionStrategy::DoubleDipping);

				Ref<TestClassUsingRef_B> ref;
				ref.RequestPromotion();

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

