#include "pgc.h"

#include "testclassusingref_A.h"
#include "testclassusingref_A.h"

#include "test_pgc_pch.h"

#include "something.h"

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
				PGCX::PGC_Transaction source(user, false, PGC::PromotionStrategy::DoubleDipping);

				TestClassUsingRef_B* dyingDestination= new TestClassUsingRef_B();

				Assert::IsTrue(user.TotalNetMemoryUsed() == sizeof(TestClassUsingRef_B)- PGC_OVERHEAD);
				size_t totalGross = user.TotalGrossMemoryUsed();
				size_t base_gross = sizeof(PGCX::PGC_Transaction);
				size_t source_gross = source.GrossMemoryUsed();
				size_t user_promotional = user.TotalPromotionMemory();
				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					//sizeof(PGCX::PGC_Transaction) +
					source.GrossMemoryUsed() + 
					user.TotalPromotionMemory(),
					L"Gross memory error");

				{
					PGCX::PGC_Transaction destination(user, false, PGC::PromotionStrategy::DoubleDipping);
					Ref<TestClassUsingRef_B> ref(dyingDestination, &destination);

					Assert::IsTrue(user.TotalNetMemoryUsed() ==
						sizeof(TestClassUsingRef_B) - PGC_OVERHEAD, L"Net memory error");
					Assert::IsTrue(user.TotalGrossMemoryUsed() ==
						//sizeof(PGCX::PGC_Transaction) +
						source.GrossMemoryUsed() +
						destination.GrossMemoryUsed() +
						user.TotalPromotionMemory(),
						L"Gross memory error");
				}
				Assert::IsTrue(user.TotalNetMemoryUsed() ==
					sizeof(TestClassUsingRef_B) - PGC_OVERHEAD, L"Net memory error");
				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					//sizeof(PGCX::PGC_Transaction) +
					source.GrossMemoryUsed() +
					user.TotalPromotionMemory(),
					L"Gross memory error");
			}
			Assert::IsTrue(user.TotalNetMemoryUsed() ==
				0, L"Net memory error");
			Assert::IsTrue(user.TotalGrossMemoryUsed() ==
				//sizeof(PGCX::PGC_Transaction) +
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

					Assert::IsTrue(user.TotalProcessedRefAttachedMemory() == 0,
						L"Processed double dipping memory should be zero");
				}
				// This should have triggered PromotedKeep behavior
				Assert::IsTrue(user.TotalProcessedRefAttachedMemory() == sizeof(PGC::PGC_Promotion),
					L"Processed double dipping memory should be updated");

				// ref.Get() called -> triggers freeing of the double dipped promotion (the second dip).
				TestClassUsingRef_B* value2 = ref.Get();

				Assert::IsTrue(user.TotalProcessedRefAttachedMemory() == 0,
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

		TEST_METHOD(TestSimpleInOutCyclePGC)
		{
			PGC_User user(ClassRegistry, AssertErrorHandler);

			{
				PGCX::PGC_Transaction parentTransaction(user, false, PGC::PromotionStrategy::DoubleDipping);

				Assert::IsTrue(TestClassUsingRef_B::m_ActiveCount == 0, L"TestClassUsingRef_B count should be 0");

				Ref<TestClassUsingRef_B> a = new TestClassUsingRef_B();
				a->SetDescription("TestClassUsingRef_B a");
				Ref<TestClassUsingRef_B> e = new TestClassUsingRef_B();
				e->SetDescription("TestClassUsingRef_B e");

				auto e_raw = e.Get();
				Pin<TestClassUsingRef_B> e_pin = e.Pinned();
				auto e_tx = e_pin->GetTransaction();

				Assert::IsTrue(e.Get() == e_raw, L"e identity changed (downward clone?)");
				Assert::IsTrue(e->GetTransaction() == e_tx, L"e moved tx (downward clone?)");
				{
					PGCX::PGC_Transaction transaction(user, false, PGC::PromotionStrategy::DoubleDipping);

					TestClassUsingRef_B* b = new TestClassUsingRef_B();
					b->SetDescription("TestClassUsingRef_B b");
					a->SetNext(b);

					TestClassUsingRef_B* c = new TestClassUsingRef_B();
					c->SetDescription("TestClassUsingRef_B c");
					b->SetNext(c);

					TestClassUsingRef_B* d = new TestClassUsingRef_B();
					d->SetDescription("TestClassUsingRef_B d");
					c->SetNext(d);

					d->SetNext(e.Get());

					TestClassUsingRef_B* f = new TestClassUsingRef_B();
					f->SetDescription("TestClassUsingRef_B f");
					e->SetNext(f);

					f->SetNext(a.Get());

					Assert::IsTrue(TestClassUsingRef_B::m_ActiveCount == 6, L"TestClassUsingRef_B count should be 6");
					Assert::IsTrue(e.Get() == e_raw, L"e identity changed (downward clone?)");
					Assert::IsTrue(e->GetTransaction() == e_tx, L"e moved tx (downward clone?)");
				}
				Assert::IsTrue(TestClassUsingRef_B::m_ActiveCount == 6, L"TestClassUsingRef_B count should be 6");
			}
			Assert::IsTrue(TestClassUsingRef_B::m_ActiveCount == 0, L"TestClassUsingRef_B count should be 0");
		}

		TEST_METHOD(Test_Ref_ReferenceCyclePGC)
		{
			PGC_User user(ClassRegistry, AssertErrorHandler);

			size_t stackTransactionSize = sizeof(StackTransaction);
			Assert::IsTrue(stackTransactionSize == 1, L"Stack transaction size is one byte");

			Assert::IsTrue(user.PromotionUsedMemory() == 0, L"Promotional memory cleared");
			Assert::IsTrue(user.PromotionFreeMemory() == user.TotalGrossMemoryUsed(), L"Promotional free memory == gross");
			{
				PGCX::PGC_Transaction parentTransaction(user);
				Ref<TestClassUsingRef_A> a = new TestClassUsingRef_A();
				a->SetDescription("TestClassUsingRef_A a");
				{
					PGCX::PGC_Transaction transaction(user);
					TestClassUsingRef_A* b = new TestClassUsingRef_A();
					a->SetNext(b);
					TestClassUsingRef_A* c = new TestClassUsingRef_A();
					b->SetNext(c);
					b->SetDescription("TestClassUsingRef_A b");
					c->SetNext(a.Get());
					c->SetDescription("TestClassUsingRef_A c");
				}
				Pin<TestClassUsingRef_A> a_pin = a.Pinned();
				TestClassUsingRef_A* a_next = a_pin->GetNext();
				auto a_next_next = a_next->GetNext();
				std::string a_description = a_pin->GetDescription();
				Assert::IsTrue(a->GetDescription() == "TestClassUsingRef_A a", L"a Correct description");
				Assert::IsTrue(a->GetNext()->GetDescription() == "TestClassUsingRef_A b", L"b Correct description");
				Assert::IsTrue(a->GetNext()->GetNext()->GetDescription() == "TestClassUsingRef_A c", L"c Correct description");

				Assert::IsTrue(a->GetNext()->GetNext()->GetNext()->GetDescription() == "TestClassUsingRef_A a", L"a Loop correct description");
				Assert::IsTrue(a->GetNext()->GetNext()->GetNext()->GetNext()->GetDescription() == "TestClassUsingRef_A b", L"b Loop correct description");
				Assert::IsTrue(a->GetNext()->GetNext()->GetNext()->GetNext()->GetNext()->GetDescription() == "TestClassUsingRef_A c", L"c Loop correct description");

				Assert::IsTrue(user.TotalNetMemoryUsed() == 3 * (sizeof(TestClassUsingRef_A) - PGC_OVERHEAD));

				Assert::IsTrue(TestClassUsingRef_A::m_ActiveCount == 3, L"Correct active count TestClassUsingRef_A");
				Assert::IsTrue(TestClassUsingRef_B::m_ActiveCount == 0, L"Correct active count TestClassUsingRef_B");
			}
			Assert::IsTrue(user.TotalNetMemoryUsed() == 0, L"Net memory cleared");
			Assert::IsTrue(user.PromotionUsedMemory() == 0, L"Promotional memory cleared");
			Assert::IsTrue(user.PromotionFreeMemory() + user.TotalProcessedRefAttachedMemory() == user.TotalGrossMemoryUsed(), L"Promotional free memory == gross");

			Assert::IsTrue(TestClassUsingRef_A::m_ActiveCount == 0, L"All TestClassUsingRef_A destructors called.");  //  
			Assert::IsTrue(TestClassUsingRef_B::m_ActiveCount == 0, L"All TestClassUsingRef_B destructors called.");
		}
	};
}

