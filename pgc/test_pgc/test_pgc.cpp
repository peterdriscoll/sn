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


bool g_ForceMemcpyFallback = true;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test_pgc
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
	namespace {
		bool g_ErrorDetected = false;
		std::string g_ErrorDescription;
	}

	void TestErrorHandler(bool p_Err, const std::string& desc) {
		if (p_Err) {
			g_ErrorDetected = true;
			g_ErrorDescription = desc;
		}
	}
	TEST_CLASS(test_pgc)
	{
	private:
		// UTILITY

		void RecursivelyDoThis(PGC::PGC_User &p_User, long p_Depth)
		{
			size_t totalNetMemoryUsed = p_User.TotalNetMemoryUsed() - PGC_Transaction::TopTransaction()->NetMemoryUsed();
			size_t totalGrossMemoryUsed = p_User.TotalGrossMemoryUsed() - PGC_Transaction::TopTransaction()->GrossMemoryUsed();
			PGC_Transaction::StartStackTransaction(p_User);
			if (p_Depth > 0)
			{
				SRef<TestPGC_A> x = new TestPGC_A();
				SRef<TestPGC_A> y = new TestPGC_A();
				SRef<TestPGC_B> z = new TestPGC_B();

				RecursivelyDoThat(p_User, 100);
				RecursivelyDoThis(p_User, p_Depth - 1);
			}
			PGC_Transaction::EndStackTransaction();
			Assert::IsTrue(totalNetMemoryUsed + PGC_Transaction::TopTransaction()->NetMemoryUsed() == p_User.TotalNetMemoryUsed(), L"Sum of net memory = total");
			Assert::IsTrue(totalGrossMemoryUsed + PGC_Transaction::TopTransaction()->GrossMemoryUsed() == p_User.TotalGrossMemoryUsed(), L"Sum of gross memory = total");
			Assert::IsTrue(p_User.PromotionUsedMemory() == 0, L"No promotions");
		}

		void RecursivelyDoThat(PGC::PGC_User &p_User, long p_Depth)
		{
			size_t totalNetMemoryUsed = p_User.TotalNetMemoryUsed() - PGC_Transaction::TopTransaction()->NetMemoryUsed();
			size_t totalGrossMemoryUsed = p_User.TotalGrossMemoryUsed() - PGC_Transaction::TopTransaction()->GrossMemoryUsed();
			PGC_Transaction::StartStackTransaction(p_User);
			if (p_Depth > 0)
			{
				SRef<TestPGC_A> x = new TestPGC_A();
				SRef<TestPGC_A> y = new TestPGC_A();
				SRef<TestPGC_B> z = new TestPGC_B();

				RecursivelyDoThat(p_User, p_Depth - 1);
			}
			PGC_Transaction::EndStackTransaction();
			Assert::IsTrue(totalNetMemoryUsed + PGC_Transaction::TopTransaction()->NetMemoryUsed() == p_User.TotalNetMemoryUsed(), L"Sum of net memory = total");
			Assert::IsTrue(totalGrossMemoryUsed + PGC_Transaction::TopTransaction()->GrossMemoryUsed() == p_User.TotalGrossMemoryUsed(), L"Sum of gross memory = total");
			Assert::IsTrue(p_User.PromotionUsedMemory() == 0, L"No promotions");
		}

		void RecursivelyDoKeep(PGC::PGC_User &p_User, TestPGC_A *keep, long p_Depth)
		{
			PGC::PGC_StackTransaction txn(p_User);

			if (p_Depth > 0)
			{
				if (p_Depth == 1)
				{
					long dog = 10;
				}
				SRef<TestPGC_A> newKeep = new TestPGC_A();
				keep->SetNext(newKeep);
				SRef<TestPGC_B> z = new TestPGC_B();
				SRef<TestPGC_A> y = new TestPGC_A();

				RecursivelyDoKeep(p_User, newKeep, p_Depth - 1);
			}
		}

	public:
		// TESTS.

		TEST_METHOD(TestSimplePromotionOnMemberRef)
		{
			PGC_User user(AssertErrorHandler);

			{
				PGC_Transaction outerTransaction(user);

				SRef<TestPGC_C> outerRef = new TestPGC_C();
				TestPGC_C* instance = NULL;
				Base* instanceBase = NULL;
				{
					PGC_Transaction innerTransaction(user);

					instance = new TestPGC_C();
					instance->SetLength(42);
					instanceBase = dynamic_cast<Base*>(instance);
					outerRef->SetCMember(instance); // Should trigger promotion registration

					// innerTransaction ends here, triggering promotion if needed
				}

				// Now we are back in outer transaction.
				// The pointer in outerRef should now point to a promoted version of instance
				Assert::IsNotNull(outerRef.Ptr(), L"outerRef should not be null after promotion");

				Assert::IsTrue(42 == outerRef->GetCMember()->GetLength(), L"Promoted instance should retain data");
			}
		}

		//  General test of the Promotional Garbage Collector memory management.
		//  Test multi level promotion.
		TEST_METHOD(TestBasicPGC)
		{
			PGC::PGC_User user(AssertErrorHandler);

			{
				PGC_Transaction parentTransaction(user);
				SRef<TestPGC_B> b = new TestPGC_B();

				Assert::AreEqual(parentTransaction.NetMemoryUsed(), sizeof(TestPGC_B) - PGC_OVERHEAD, L"Memory used by TestPGC_B");
				Assert::IsTrue(parentTransaction.NetMemoryUsed() == user.TotalNetMemoryUsed());
				Assert::IsTrue(parentTransaction.GrossMemoryUsed() + user.PromotionFreeMemory() + user.PromotionUsedMemory() == user.TotalGrossMemoryUsed());

				size_t netMemoryUsed = 0;
				size_t totalMemoryUsed = 0;
				{
					PGC_Transaction transaction(user);
					SRef<TestPGC_A> a;
					a = new TestPGC_A();
					a->SetDescription("a");
					b->SetTestA((TestPGC_A *)a);
					SRef<TestPGC_A> a1 = new TestPGC_A();
					a1->SetDescription("a1");
					a->SetNext(a1);
					SRef<TestPGC_A> a2 = new TestPGC_A();
					a2->SetDescription("a2");
					a1->SetNext(a2);
					SRef<TestPGC_A> a3 = new TestPGC_A();
					a3->SetDescription("a3");
					a2->SetNext(a3);

					Assert::IsTrue(parentTransaction.NetMemoryUsed() + transaction.NetMemoryUsed() == user.TotalNetMemoryUsed(), L"NetMemoryUsed == TotalNetMemoryUsed");
					Assert::IsTrue(parentTransaction.GrossMemoryUsed() + transaction.GrossMemoryUsed() + user.PromotionUsedMemory() == user.TotalGrossMemoryUsed(), L"PromotionUsedMemory == TotalGrossMemoryUsed");

					Assert::IsTrue(TestPGC_A::m_ActiveCount == 4);
					Assert::IsTrue(TestPGC_B::m_ActiveCount == 1);

					{
						PGC_Transaction childTransaction(user);

						SRef<TestPGC_A> c;
						c = new TestPGC_A();
						c->SetDescription("c");
						SRef<TestPGC_A> c1 = new TestPGC_A();
						c1->SetDescription("c1");
						c->SetNext(c1);
						SRef<TestPGC_A> c2 = new TestPGC_A();
						c2->SetDescription("c2");
						c1->SetNext(c2);
						SRef<TestPGC_A> c3 = new TestPGC_A();
						c3->SetDescription("c3");
						c2->SetNext(c3);

						a3->SetNext(c2);

						Assert::IsTrue(TestPGC_A::m_ActiveCount == 8);
						Assert::IsTrue(TestPGC_B::m_ActiveCount == 1);

						Assert::IsTrue(parentTransaction.NetMemoryUsed() + transaction.NetMemoryUsed() + childTransaction.NetMemoryUsed() == user.TotalNetMemoryUsed(), L"Sum of net memory used == total");
						Assert::IsTrue(parentTransaction.GrossMemoryUsed() + transaction.GrossMemoryUsed() + childTransaction.GrossMemoryUsed() + user.TotalPromotionMemory() == user.TotalGrossMemoryUsed(), L"Sum of gross and promotional memory used == total gross");
					}

					Assert::IsTrue(TestPGC_A::m_ActiveCount == 6);
					Assert::IsTrue(TestPGC_B::m_ActiveCount == 1);

					b->SetTestA(a3->GetNext());
					a3->SetNext(0);
					b->SetTestA((TestPGC_A *)a);
					netMemoryUsed = parentTransaction.NetMemoryUsed();
					totalMemoryUsed = user.TotalNetMemoryUsed();
					Assert::IsTrue(parentTransaction.GrossMemoryUsed() + transaction.GrossMemoryUsed() + user.TotalPromotionMemory() == user.TotalGrossMemoryUsed(), L"Sum of gross and promotional memory used == total gross");
					// Promotion occurs in the destructor of the transaction here.
				}
				size_t newNetMemoryUsed = parentTransaction.NetMemoryUsed();
				size_t newTotalMemoryUsed = user.TotalNetMemoryUsed();
				Assert::IsTrue(parentTransaction.NetMemoryUsed() == user.TotalNetMemoryUsed(), L"Parent net == Total net");
				Assert::IsTrue(parentTransaction.GrossMemoryUsed() + user.TotalPromotionMemory() == user.TotalGrossMemoryUsed(), L"Gross + promotional = total gross");

				SRef<TestPGC_A> xa = b->GetTestA();
				Assert::IsTrue(xa->GetDescription() == "a");
				SRef<TestPGC_A> xa1 = xa->GetNext();
				Assert::IsTrue(xa1->GetDescription() == "a1");
				SRef<TestPGC_A> xa2 = xa1->GetNext();
				Assert::IsTrue(xa2->GetDescription() == "a2");
				SRef<TestPGC_A> xa3 = xa2->GetNext();
				Assert::IsTrue(xa3->GetDescription() == "a3");
				Assert::IsTrue(xa3->GetNext() == 0);

				Assert::IsTrue(TestPGC_A::m_ActiveCount == 4);
				Assert::IsTrue(TestPGC_B::m_ActiveCount == 1);

				// All destructors called here in the destructor of the transaction.
			}

			Assert::IsTrue(TestPGC_A::m_ActiveCount == 0, L"All TestPGC_A destructors called.");  //  All destructors called.
			Assert::IsTrue(TestPGC_B::m_ActiveCount == 0, L"All TestPGC_B destructors called.");

			Assert::IsTrue(user.TotalNetMemoryUsed() == 0, L"Net mwmory cleared");
			Assert::IsTrue(user.PromotionUsedMemory() == 0, L"Promotional memory cleared");
			Assert::IsTrue(user.TotalPromotionMemory() == user.TotalGrossMemoryUsed(), L"Promotional free memory == total gross");
		}

		//  Test automatic transaction creation.  No promotion.
		//  * StartStackTransaction
		//  * EndStackTransaction
		TEST_METHOD(TestRecursivePGC)
		{
			PGC_User user(AssertErrorHandler);

			{
				PGC_Transaction transaction(user);
				{
					PGC_Transaction::StartStackTransaction(user);
					{
						PGC_Transaction transaction(user);
						RecursivelyDoThis(user, 100);
					}
					PGC_Transaction::EndStackTransaction();
				}
			}
			Assert::IsTrue(user.TotalNetMemoryUsed() == 0, L"Net memory cleared");
			Assert::IsTrue(user.PromotionUsedMemory() == 0, L"Promotional memory cleared");
			Assert::IsTrue(user.PromotionFreeMemory() == user.TotalGrossMemoryUsed(), L"Free memory == gross memory");

			Assert::IsTrue(TestPGC_A::m_ActiveCount == 0, L"All TestPGC_A destructors called.");
			Assert::IsTrue(TestPGC_B::m_ActiveCount == 0, L"All TestPGC_B destructors called.");
		}

		//  Test automatic transaction creation with promotion.
		TEST_METHOD(TestRecursiveKeepPGC)
		{
			PGC_User user(AssertErrorHandler);

			// This test depends on the size of objects and how many fit of TestPGC_A fit in a block.
			// This varies for 64 bit and debug mode. So ony run this test in 32 bit debug.
			long NumberOf_A_B_InBlock = PGC::BlockSize / (sizeof(TestPGC_A) + sizeof(TestPGC_B));

			Assert::IsTrue(user.PromotionUsedMemory() == 0, L"Promotional memory cleared");
			Assert::IsTrue(user.PromotionFreeMemory() == user.TotalGrossMemoryUsed(), L"Promotional free memeory == gross");
			{
				PGC_Transaction parentTransaction(user);
				long depth = NumberOf_A_B_InBlock;
				SRef<TestPGC_A> keep = new TestPGC_A();
				{
					PGC_Transaction transaction(user);
					RecursivelyDoKeep(user, keep, depth);
					long allocatedInFirstTransaction = depth;
					long numThatFitsInFirstTransaction = NumberOf_A_B_InBlock;
					if (numThatFitsInFirstTransaction < allocatedInFirstTransaction)
					{
						allocatedInFirstTransaction = numThatFitsInFirstTransaction;
					}
					size_t total = user.TotalNetMemoryUsed();
					size_t size_A1 = (depth + 1) * (sizeof(TestPGC_A) - PGC_OVERHEAD);
					size_t size_A = allocatedInFirstTransaction * (sizeof(TestPGC_A) - PGC_OVERHEAD);
					size_t size_B = allocatedInFirstTransaction * (sizeof(TestPGC_B) - PGC_OVERHEAD);
					size_t size_total = size_A1 + size_A + size_B;
					Assert::IsTrue(total == size_total, L"Memory usage accounted for.");
				}
				long countKeep = keep->CountList();
				Assert::IsTrue(countKeep == depth, L"Kept 1 for each recursion");
				Assert::IsTrue(user.TotalNetMemoryUsed() == (countKeep + 1) * (sizeof(TestPGC_A) - PGC_OVERHEAD), L"Memory usage accounted for.");

				Assert::IsTrue(TestPGC_A::m_ActiveCount == countKeep + 1, L"TestPGC_A active is correct.");
				Assert::IsTrue(TestPGC_B::m_ActiveCount == 0, L"TestPGC_AB active is zero.");
			}
			Assert::IsTrue(user.TotalNetMemoryUsed() == 0, L"Net memory cleared");
			Assert::IsTrue(user.PromotionUsedMemory() == 0, L"Promotional memory cleared");
			Assert::IsTrue(user.PromotionFreeMemory() == user.TotalGrossMemoryUsed(), L"Promotional free memory = gross");

			Assert::IsTrue(TestPGC_A::m_ActiveCount == 0, L"All TestPGC_A destructors called."); 
			Assert::IsTrue(TestPGC_B::m_ActiveCount == 0, L"All TestPGC_AB destructors called.");
		}

		TEST_METHOD(TestSimpleReferenceCyclePGC)
		{
			PGC_User user(AssertErrorHandler);

			{
				PGC_Transaction parentTransaction(user, false, PGC::PromotionStrategy::DoubleDipping);

				Assert::IsTrue(TestPGC_B::m_ActiveCount == 0, L"TestPGC_B count should be 0");

				SRef<TestPGC_B> a = new TestPGC_B();
				a->SetDescription("TestPGC_B a");
				{
					PGC_Transaction transaction(user, false, PGC::PromotionStrategy::DoubleDipping);

					TestPGC_B* b = new TestPGC_B();
					b->SetDescription("TestPGC_B b");
					a->SetNext(b);

					TestPGC_B* c = new TestPGC_B();
					c->SetDescription("TestPGC_B c");
					b->SetNext(c);

					c->SetNext(a);

					Assert::IsTrue(TestPGC_B::m_ActiveCount == 3, L"TestPGC_B count should be 3");
				}
				Assert::IsTrue(TestPGC_B::m_ActiveCount == 3, L"TestPGC_B count should be 3");
			}
			Assert::IsTrue(TestPGC_B::m_ActiveCount == 0, L"TestPGC_B count should be 0");

			{
				PGC_Transaction parentTransaction(user, false, PGC::PromotionStrategy::DoubleDipping);

				Assert::IsTrue(TestPGC_B::m_ActiveCount == 0, L"TestPGC_B count should be 0");

				SRef<TestPGC_B> a = new TestPGC_B();
				a->SetDescription("TestPGC_B a");
				{
					PGC_Transaction transaction(user, false, PGC::PromotionStrategy::DoubleDipping);

					TestPGC_B* b = new TestPGC_B();
					b->SetDescription("TestPGC_B b");
					a->SetNext(b);

					TestPGC_B* c = new TestPGC_B();
					c->SetDescription("TestPGC_B c");
					b->SetNext(c);

					TestPGC_B* d = new TestPGC_B();
					d->SetDescription("TestPGC_B d");
					c->SetNext(d);

					TestPGC_B* e = new TestPGC_B();
					e->SetDescription("TestPGC_B e");
					d->SetNext(e);

					TestPGC_B* f = new TestPGC_B();
					f->SetDescription("TestPGC_B f");
					e->SetNext(f);

					f->SetNext(a);

					Assert::IsTrue(TestPGC_B::m_ActiveCount == 6, L"TestPGC_B count should be 6");
				}
				Assert::IsTrue(TestPGC_B::m_ActiveCount == 6, L"TestPGC_B count should be 6");
			}
			Assert::IsTrue(TestPGC_B::m_ActiveCount == 0, L"TestPGC_B count should be 0");
		}

		TEST_METHOD(TestSimpleInOutCyclePGC)
		{
			PGC_User user(AssertErrorHandler);

			{
				PGC_Transaction parentTransaction(user, false, PGC::PromotionStrategy::DoubleDipping);

				Assert::IsTrue(TestPGC_B::m_ActiveCount == 0, L"TestPGC_B count should be 0");

				SRef<TestPGC_B> a = new TestPGC_B();
				a->SetDescription("TestPGC_B a");
				SRef<TestPGC_B> e = new TestPGC_B();
				e->SetDescription("TestPGC_B e");

				auto e_raw = e.Ptr();
				auto e_tx = e->GetTransaction();

				Assert::IsTrue(e.Ptr() == e_raw, L"e identity changed (downward clone?)");
				Assert::IsTrue(e->GetTransaction() == e_tx, L"e moved tx (downward clone?)");
				{
					PGC_Transaction transaction(user, false, PGC::PromotionStrategy::DoubleDipping);

					TestPGC_B* b = new TestPGC_B();
					b->SetDescription("TestPGC_B b");
					a->SetNext(b);

					TestPGC_B* c = new TestPGC_B();
					c->SetDescription("TestPGC_B c");
					b->SetNext(c);

					TestPGC_B* d = new TestPGC_B();
					d->SetDescription("TestPGC_B d");
					c->SetNext(d);

					d->SetNext(e);

					TestPGC_B* f = new TestPGC_B();
					f->SetDescription("TestPGC_B f");
					e->SetNext(f);

					f->SetNext(a);

					Assert::IsTrue(TestPGC_B::m_ActiveCount == 6, L"TestPGC_B count should be 6");
					Assert::IsTrue(e.Ptr() == e_raw, L"e identity changed (downward clone?)");
					Assert::IsTrue(e->GetTransaction() == e_tx, L"e moved tx (downward clone?)");
				}
				Assert::IsTrue(TestPGC_B::m_ActiveCount == 6, L"TestPGC_B count should be 6");
			}
			Assert::IsTrue(TestPGC_B::m_ActiveCount == 0, L"TestPGC_B count should be 0");
		}

		TEST_METHOD(TestReferenceCyclePGC)
		{
			PGC_User user(AssertErrorHandler);

			size_t stackTransactionSize = sizeof(StackTransaction);
			Assert::IsTrue(stackTransactionSize == 1, L"Stack transaction size is one byte");

			Assert::IsTrue(user.PromotionUsedMemory() == 0, L"Promotional memory cleared");
			Assert::IsTrue(user.PromotionFreeMemory() == user.TotalGrossMemoryUsed(), L"Promotional free memory == gross");
			{
				PGC_Transaction parentTransaction(user);
				SRef<TestPGC_A> a = new TestPGC_A();
				a->SetDescription("TestPGC_A a");
				{
					PGC_Transaction transaction(user);
					TestPGC_A *b = new TestPGC_A();
					a->SetNext(b);
					TestPGC_A *c = new TestPGC_A();
					b->SetNext(c);
					b->SetDescription("TestPGC_A b");
					c->SetNext(a);
					c->SetDescription("TestPGC_A c");
				}

				Assert::IsTrue(a->GetDescription() == "TestPGC_A a", L"a Correct description");
				Assert::IsTrue(a->GetNext()->GetDescription() == "TestPGC_A b", L"b Correct description");
				Assert::IsTrue(a->GetNext()->GetNext()->GetDescription() == "TestPGC_A c", L"c Correct description");

				Assert::IsTrue(a->GetNext()->GetNext()->GetNext()->GetDescription() == "TestPGC_A a", L"a Loop correct description");
				Assert::IsTrue(a->GetNext()->GetNext()->GetNext()->GetNext()->GetDescription() == "TestPGC_A b", L"b Loop correct description");
				Assert::IsTrue(a->GetNext()->GetNext()->GetNext()->GetNext()->GetNext()->GetDescription() == "TestPGC_A c", L"c Loop correct description");

				Assert::IsTrue(user.TotalNetMemoryUsed() == 3 * (sizeof(TestPGC_A) - PGC_OVERHEAD));

				Assert::IsTrue(TestPGC_A::m_ActiveCount == 3, L"Correct active count TestPGC_A");
				Assert::IsTrue(TestPGC_B::m_ActiveCount == 0, L"Correct active count TestPGC_B");
			}
			Assert::IsTrue(user.TotalNetMemoryUsed() == 0, L"Net memory cleared");
			Assert::IsTrue(user.PromotionUsedMemory() == 0, L"Promotional memory cleared");
			Assert::IsTrue(user.PromotionFreeMemory() == user.TotalGrossMemoryUsed(), L"Promotional free memory == gross");

			Assert::IsTrue(TestPGC_A::m_ActiveCount == 0, L"All TestPGC_A destructors called.");  //  
			Assert::IsTrue(TestPGC_B::m_ActiveCount == 0, L"All TestPGC_B destructors called.");
		}

		TEST_METHOD(TestDoublePromotion)
		{
			PGC_User user(AssertErrorHandler);

			{
				PGC_Transaction outerTransaction(user);
				SRef<TestPGC_B> b = new TestPGC_B();  // b lives in outer transaction
				{
					PGC_Transaction innerTransaction(user);
					SRef<TestPGC_A> a = new TestPGC_A();  // a lives in inner transaction
					a->SetDescription("TestPGC_A a");
					b->SetTestA(a); // First promotion request: b holds a, must promote a

					// Manually re-request promotion of 'a' to simulate possible double-registration
					b->SimulateLegacyPromotionOnA();

					// Test passes if there's no crash and promotion bookkeeping is correct
				}

				Assert::IsTrue(TestPGC_A::m_ActiveCount == 1, L"All TestPGC_A instances destroyed.");
				Assert::IsTrue(TestPGC_B::m_ActiveCount == 1, L"All TestPGC_B instances destroyed.");
			}

			Assert::IsTrue(TestPGC_A::m_ActiveCount == 0, L"All TestPGC_A instances destroyed.");
			Assert::IsTrue(TestPGC_B::m_ActiveCount == 0, L"All TestPGC_B instances destroyed.");

			// Ensure memory is fully cleaned up and destructors called
		}

		TEST_METHOD(TestPromotionToDieingTransaction)
		{
			PGC_User user(AssertErrorHandler);

			{
				PGC_Transaction outerTransaction(user);
				SRef<TestPGC_A> a = new TestPGC_A();
				{
					PGC_Transaction dyingTransaction(user);
					SRef<TestPGC_A> b = new TestPGC_A();

					b->SetNext(a);
					// PGC::SetOnErrorHandler(TestErrorHandler);
				}
			}
			// Assert::IsTrue(g_ErrorDetected, (std::wstring(L"Expected error not detected. Got: ") + std::wstring(g_ErrorDescription.begin(), g_ErrorDescription.end())).c_str());
			// Assert::IsTrue(g_ErrorDescription.find("Promotion from an outer to an inner transaction") != std::string::npos,
			//	L"Expected specific error message not found.");
			Assert::IsTrue(TestPGC_A::m_ActiveCount == 0, L"All TestPGC_A destructors called.");
			Assert::IsTrue(TestPGC_B::m_ActiveCount == 0, L"All TestPGC_B destructors called.");
		}

		class Misaligner : public PGC::PGC_Base
		{
			PGC_CLASS(Misaligner);
		public:
			char junk[3]; // not a multiple of 8
		};

		// This test ensures that objects remain correctly aligned after promotion,
		// even when preceded by objects with odd sizes that could disrupt alignment.
		//
		// MSVC may silently pad Misaligner to preserve alignment, so this test might
		// pass even if alignment logic is flawed — but on other compilers or platforms,
		// misalignment could be exposed. Keep this test as a cross-platform safeguard.
		TEST_METHOD(TestAlignmentAfterPromotion)
		{
			PGC_User user(AssertErrorHandler);

			{
				PGC_Transaction parent(user);
				SRef<Misaligner> m = new Misaligner();
				std::size_t m_size= sizeof(Misaligner);

				SRef<TestPGC_B> b = new TestPGC_B();
				void* b_addr = b;

				Assert::IsTrue(reinterpret_cast<uintptr_t>(b_addr) % alignof(TestPGC_A) == 0, L"b alignment incorrect");
				{
					PGC_Transaction child(user);
					TestPGC_A* a = new TestPGC_A();

					void* a_addr = a;
					Assert::IsTrue(reinterpret_cast<uintptr_t>(a_addr) % alignof(TestPGC_A) == 0, L"a alignment incorrect");

					b->SetTestA(a);  // triggers promotion
				}

				void* addr = b->GetTestA();
				Assert::IsTrue(reinterpret_cast<uintptr_t>(addr) % alignof(TestPGC_A) == 0, L"a alignment incorrect after promotion.");
			}
		}

		TEST_METHOD(TestStalePointerAfterPromotion)
		{
			PGC_User user(AssertErrorHandler);

			{
				PGC_Transaction transaction(user);

				// Create object 'a' and get a second pointer to it
				TestPGC_B* b = new TestPGC_B();
				TestPGC_A* a;
				TestPGC_A* rawOld = nullptr;

				{
					PGC_Transaction subTransaction(user);
					a = new TestPGC_A();
					rawOld = a; // stale pointer before promotion
					a->SetDescription("magic value");
					b->SetTestA(a);  // This should trigger promotion of 'a'
				}

				// 'a' should now be promoted. Let's check if rawOld is now stale.
				// We expect rawOld to either be updated or clearly invalid.
				a = b->GetTestA();
				bool moved = (rawOld != a);
				Assert::IsTrue(moved, L"Object was moved");
			}

		}

		TEST_METHOD(TestPromotionResult_Dropped_DyingDestination)
		{
			PGC_User user;
			{
				PGC_Transaction source(user, false, PGC::PromotionStrategy::DoubleDipping);

				TestPGC_B* dyingDestination= new TestPGC_B();

				Assert::IsTrue(user.TotalNetMemoryUsed() == sizeof(TestPGC_B)- PGC_OVERHEAD);
				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					source.GrossMemoryUsed() + 
					user.TotalPromotionMemory(),
					L"Gross memory error");

				{
					PGC_Transaction destination(user, false, PGC::PromotionStrategy::DoubleDipping);
					MemberRef<TestPGC_B> ref(dyingDestination, &destination);

					Assert::IsTrue(user.TotalNetMemoryUsed() ==
						sizeof(TestPGC_B) - PGC_OVERHEAD, L"Net memory error");
					Assert::IsTrue(user.TotalGrossMemoryUsed() ==
						source.GrossMemoryUsed() +
						destination.GrossMemoryUsed() +
						user.TotalPromotionMemory(),
						L"Gross memory error");
				}
				Assert::IsTrue(user.TotalNetMemoryUsed() ==
					sizeof(TestPGC_B) - PGC_OVERHEAD, L"Net memory error");
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

		TEST_METHOD(TestPromotionResult_Nested_Dropped_DyingDestination)
		{
			PGC_User user;

			{
				// SOURCE transaction (outer)
				PGC_Transaction source(user, false, PGC::PromotionStrategy::Backstabbing);

				// Allocate TestPGC_A in source
				TestPGC_A* a = new TestPGC_A();

				Assert::IsTrue(user.TotalNetMemoryUsed() == sizeof(TestPGC_A) - PGC_OVERHEAD, L"Net after allocating A");
				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					source.GrossMemoryUsed() + user.TotalPromotionMemory(),
					L"Gross after allocating A");

				{
					// DESTINATION transaction (inner)
					PGC_Transaction destination(user, false, PGC::PromotionStrategy::Backstabbing);

					// Create TestPGC_B in destination
					SRef<TestPGC_B> sref(new TestPGC_B());

					Assert::IsTrue(user.TotalNetMemoryUsed() == (sizeof(TestPGC_A) - PGC_OVERHEAD) + (sizeof(TestPGC_B) - PGC_OVERHEAD), L"Net after allocating B");
					Assert::IsTrue(user.TotalGrossMemoryUsed() ==
						source.GrossMemoryUsed() +
						destination.GrossMemoryUsed() +
						user.TotalPromotionMemory(),
						L"Gross after allocating B");

					// Assign TestPGC_A pointer to TestA member on TestPGC_B
					sref->SetTestA(a);

					// Promotion should now be pending (destination -> source)
				}

				// Source is still alive
				// No promotion should have occurred.
				Assert::IsTrue(user.TotalNetMemoryUsed() == sizeof(TestPGC_A) - PGC_OVERHEAD, L"Net before source ends");

				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					source.GrossMemoryUsed() +
					user.TotalPromotionMemory(),
					L"Gross after destination dies");

				// Now, when source ends, promotion will occur
			}

			// Source is now gone — promotion should have completed (PromotedDone)
			Assert::IsTrue(user.TotalNetMemoryUsed() == 0, L"Net after promotion");

			Assert::IsTrue(user.TotalGrossMemoryUsed() ==
				user.TotalPromotionMemory(),
				L"Gross after promotion");

			Assert::IsTrue(user.TotalProcessedDoubleDippingMemory() == 0, L"No processed memory for Backstabbing");

			// Final cleanup
			Assert::IsTrue(user.TotalNetMemoryUsed() == 0, L"Final net memory");
			Assert::IsTrue(user.TotalGrossMemoryUsed() == user.TotalPromotionMemory(), L"Final gross memory");
		}

		TEST_METHOD(TestPromotionResult_PromotedDone_Backstabbing)
		{
			PGC_User user;

			{
				// DESTINATION transaction (must outlive source)
				PGC_Transaction destination(user, false, PGC::PromotionStrategy::Backstabbing);

				MemberRef<TestPGC_B> ref;

				Assert::IsTrue(user.TotalNetMemoryUsed() == 0, L"Net memory after allocation");

				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					destination.GrossMemoryUsed() +
					user.TotalPromotionMemory(),
					L"Gross memory after allocation");

				{
					// SOURCE transaction
					PGC_Transaction source(user, false, PGC::PromotionStrategy::Backstabbing);

					Assert::IsTrue(user.TotalNetMemoryUsed() == 0, L"Net memory after allocation");

					Assert::IsTrue(user.TotalGrossMemoryUsed() ==
						source.GrossMemoryUsed() +
						destination.GrossMemoryUsed() +
						user.TotalPromotionMemory(),
						L"Gross memory after allocation");

					// --- After allocation ---
					ref.Set(new TestPGC_B(), &destination);

					Assert::IsTrue(user.TotalNetMemoryUsed() == sizeof(TestPGC_B) - PGC_OVERHEAD, L"Net memory after allocation");

					Assert::IsTrue(user.TotalGrossMemoryUsed() ==
						source.GrossMemoryUsed() +
						destination.GrossMemoryUsed() +
						user.TotalPromotionMemory(),
						L"Gross memory after allocation");
				}

				// --- After source dies, before PromoteRequests ---
				Assert::IsTrue(user.TotalNetMemoryUsed() == sizeof(TestPGC_B) - PGC_OVERHEAD, L"Net before promotion");

				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					destination.GrossMemoryUsed() + user.TotalPromotionMemory(),
					L"Gross before promotion (source gone)");

				Assert::IsTrue(destination.NetMemoryUsed() == sizeof(TestPGC_B) - PGC_OVERHEAD, L"Destination owns object");

				Assert::IsTrue(user.TotalNetMemoryUsed() == sizeof(TestPGC_B) - PGC_OVERHEAD, L"Net memory after promotion");

				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					destination.GrossMemoryUsed() + user.TotalPromotionMemory(),
					L"Gross memory after promotion");

				Assert::IsTrue(user.TotalProcessedDoubleDippingMemory() == 0, L"No processed memory (Backstabbing)");
			}

			// --- After destination dies ---
			Assert::IsTrue(user.TotalNetMemoryUsed() == 0, L"Net memory after cleanup");

			Assert::IsTrue(user.TotalGrossMemoryUsed() == user.TotalPromotionMemory(), L"Gross memory after cleanup");
		}

		TEST_METHOD(TestPromotionResult_Nested_PromotedDone_Backstabbing_ThroughTestA)
		{
			PGC_User user;

			{
				// DESTINATION transaction (must outlive source)
				PGC_Transaction destination(user, false, PGC::PromotionStrategy::Backstabbing);

				TestPGC_A* a = nullptr;

				SRef<TestPGC_B> sref = new TestPGC_B();

				Assert::IsTrue(user.TotalNetMemoryUsed() == sizeof(TestPGC_B) - PGC_OVERHEAD, L"Initial net memory");
				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					destination.GrossMemoryUsed() + user.TotalPromotionMemory(),
					L"Initial gross memory");

				{
					// SOURCE transaction
					PGC_Transaction source(user, false, PGC::PromotionStrategy::Backstabbing);

					Assert::IsTrue(user.TotalNetMemoryUsed() == sizeof(TestPGC_B) - PGC_OVERHEAD, L"Net memory after source started");

					Assert::IsTrue(user.TotalGrossMemoryUsed() ==
						source.GrossMemoryUsed() +
						destination.GrossMemoryUsed() +
						user.TotalPromotionMemory(),
						L"Gross memory after source started");

					// Allocate TestPGC_A in source
					a = new TestPGC_A();

					// Set A into B
					sref->SetTestA(a);

					Assert::IsTrue(a == sref->GetTestA(), L"TestPGC_A pointer set correctly");

					Assert::IsTrue(user.TotalNetMemoryUsed() ==
						sizeof(TestPGC_A) + sizeof(TestPGC_B) - 2 * PGC_OVERHEAD,
						L"Net after allocating A and B");

					Assert::IsTrue(user.TotalGrossMemoryUsed() ==
						source.GrossMemoryUsed() +
						destination.GrossMemoryUsed() +
						user.TotalPromotionMemory(),
						L"Gross after SetTestA");
					// After source ends -> promotion occurs (PromotedDone)
				}

				Assert::IsTrue(user.TotalNetMemoryUsed() ==
					(sizeof(TestPGC_B) - PGC_OVERHEAD) + (sizeof(TestPGC_A) - PGC_OVERHEAD),
					L"Net after promotion");

				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					destination.GrossMemoryUsed() + user.TotalPromotionMemory(),
					L"Gross after promotion");

				Assert::IsTrue(destination.NetMemoryUsed() ==
					(sizeof(TestPGC_B) - PGC_OVERHEAD) + (sizeof(TestPGC_A) - PGC_OVERHEAD),
					L"Destination owns object");

				Assert::IsTrue(user.TotalProcessedDoubleDippingMemory() == 0,
					L"No processed memory (Backstabbing)");

				// Check that GetTestA returns the same pointer
				Assert::IsTrue(sref->GetTestA() != a, L"Promoted object must match original pointer");
			}

			// Destination dies — full cleanup
			Assert::IsTrue(user.TotalNetMemoryUsed() == 0, L"Final net memory");
			Assert::IsTrue(user.TotalGrossMemoryUsed() == user.TotalPromotionMemory(), L"Final gross memory");
		}

		TEST_METHOD(TestPromotionResult_PromotedKeep_DoubleDipping)
		{
			PGC_User user;

			{
				// DESTINATION transaction (ref lives here)
				PGC_Transaction destination(user, false, PGC::PromotionStrategy::DoubleDipping);

				MemberRef<TestPGC_B> ref;

				Assert::IsTrue(user.TotalNetMemoryUsed() == 0, L"Initial net memory");
				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					destination.GrossMemoryUsed() + user.TotalPromotionMemory(),
					L"Initial gross memory");

				{
					// SOURCE transaction (allocates object)
					PGC_Transaction source(user, false, PGC::PromotionStrategy::DoubleDipping);

					Assert::IsTrue(user.TotalNetMemoryUsed() == 0, L"Net memory after source start");

					Assert::IsTrue(user.TotalGrossMemoryUsed() ==
						source.GrossMemoryUsed() +
						destination.GrossMemoryUsed() +
						user.TotalPromotionMemory(),
						L"Gross memory after source start");

					// Allocate in source, ref is owned by destination
					ref.Set(new TestPGC_B(), &destination);

					Assert::IsTrue(user.TotalNetMemoryUsed() == sizeof(TestPGC_B) - PGC_OVERHEAD, L"Net memory after allocation");

					Assert::IsTrue(user.TotalGrossMemoryUsed() ==
						source.GrossMemoryUsed() +
						destination.GrossMemoryUsed() +
						user.TotalPromotionMemory(),
						L"Gross memory after allocation");

					// Simulate use of value in destination. Value accessed via the promotion.
					TestPGC_B* value = ref.Get(); 

					Assert::IsNotNull(value, L"ref.Get() returned null");

					Assert::IsTrue(user.TotalProcessedDoubleDippingMemory() == 0,
						L"Processed double dipping memory should be zero");
				}
				// This should have triggered PromotedKeep behavior
				Assert::IsTrue(user.TotalProcessedDoubleDippingMemory() == sizeof(PGC::PGC_Promotion),
					L"Processed double dipping memory should be updated");

				// ref.Get() called -> triggers freeing of the double dipped promotion (the second dip).
				TestPGC_B* value2 = ref.Get();

				Assert::IsTrue(user.TotalProcessedDoubleDippingMemory() == 0,
					L"Processed double dipping memory should be zero after free");

				Assert::IsTrue(destination.NetMemoryUsed() == sizeof(TestPGC_B) - PGC_OVERHEAD, L"Destination owns value");
				Assert::IsTrue(user.TotalNetMemoryUsed() == sizeof(TestPGC_B) - PGC_OVERHEAD, L"Net memory after promotion");

				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					destination.GrossMemoryUsed() +
					user.TotalPromotionMemory(),
					L"Gross memory after promotion");
			}

			// After destination dies TestPGC_B is deleted
			Assert::IsTrue(user.TotalNetMemoryUsed() == 0, L"Net memory after cleanup");

			Assert::IsTrue(user.TotalGrossMemoryUsed() == user.TotalPromotionMemory(),
				L"Gross memory after cleanup");
		}

		TEST_METHOD(TestPromotionResult_Nested_PromotedKeep_DoubleDipping_ThroughTestA)
		{
			PGC_User user;

			TestPGC_A* a = nullptr;

			{
				// DESTINATION transaction (lives longer)
				PGC_Transaction destination(user, false, PGC::PromotionStrategy::DoubleDipping);

				SRef<TestPGC_B> sref = new TestPGC_B();

				Assert::IsTrue(user.TotalNetMemoryUsed() == sizeof(TestPGC_B) - PGC_OVERHEAD, L"Initial net memory");
				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					destination.GrossMemoryUsed() + user.TotalPromotionMemory(),
					L"Initial gross memory");

				{
					// SOURCE transaction (where A is created)
					PGC_Transaction source(user, false, PGC::PromotionStrategy::DoubleDipping);

					Assert::IsTrue(user.TotalNetMemoryUsed() == (sizeof(TestPGC_B) - PGC_OVERHEAD), L"Net memory after source start");
					Assert::IsTrue(user.TotalGrossMemoryUsed() ==
						source.GrossMemoryUsed() +
						destination.GrossMemoryUsed() +
						user.TotalPromotionMemory(),
						L"Gross memory after source start");

					// Create A in source
					a = new TestPGC_A();

					// Set A into B
					sref->SetTestA(a);

					Assert::IsTrue(user.TotalNetMemoryUsed() ==
						sizeof(TestPGC_A) + sizeof(TestPGC_B) - 2 * PGC_OVERHEAD,
						L"Net after allocating A and B");

					Assert::IsTrue(user.TotalGrossMemoryUsed() ==
						source.GrossMemoryUsed() +
						destination.GrossMemoryUsed() +
						user.TotalPromotionMemory(),
						L"Gross after SetTestA");

					// First dip: assign triggers promotion request (not yet committed)

					// Test that accessing the pointer to TestPGC_A using GetTestA still works before promotion.
					TestPGC_A* returnedA = sref->GetTestA();

					Assert::IsTrue(returnedA == a, L"Returned pointer must match A");
					Assert::IsTrue(user.TotalProcessedDoubleDippingMemory() == 0,
						L"Processed double dipping memory should be zero before promotion");
				}

				// Source is now dead -> triggers PromotedKeep
				Assert::IsTrue(user.TotalProcessedDoubleDippingMemory() == sizeof(PGC::PGC_Promotion),
					L"Processed memory updated after PromotedKeep");

				// Final access — triggers second dip, accessing the promoted copy.
				TestPGC_A* returnedAgain = sref->GetTestA();

				Assert::IsTrue(returnedAgain != a, L"Returned pointer is to the promoted copy");
				Assert::IsTrue(user.TotalProcessedDoubleDippingMemory() == 0,
					L"Processed memory should be zero after second dip is freed");

				Assert::IsTrue(destination.NetMemoryUsed() ==
					(sizeof(TestPGC_B) - PGC_OVERHEAD) + (sizeof(TestPGC_A) - PGC_OVERHEAD),
					L"Destination owns B");

				Assert::IsTrue(user.TotalNetMemoryUsed() ==
					(sizeof(TestPGC_B) - PGC_OVERHEAD) + (sizeof(TestPGC_A) - PGC_OVERHEAD),
					L"Net memory after promotion");

				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					destination.GrossMemoryUsed() + user.TotalPromotionMemory(),
					L"Gross memory after promotion");
			}

			// All cleaned up after destination dies
			Assert::IsTrue(user.TotalNetMemoryUsed() == 0, L"Final net memory");
			Assert::IsTrue(user.TotalGrossMemoryUsed() == user.TotalPromotionMemory(), L"Final gross memory");
		}

		TEST_METHOD(TestPromotionResult_Nested_Keep_ThenPromotedKeep_ThroughTestA)
		{
			PGC_User user;

			TestPGC_A* a = nullptr;

			{
				// DESTINATION transaction (longer-lived)
				PGC_Transaction destination(user, false, PGC::PromotionStrategy::DoubleDipping);

				// Allocate B in destination
				SRef<TestPGC_B> sref = new TestPGC_B();

				Assert::IsTrue(user.TotalNetMemoryUsed() == sizeof(TestPGC_B) - PGC_OVERHEAD, L"Initial net memory");
				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					destination.GrossMemoryUsed() + user.TotalPromotionMemory(),
					L"Initial gross memory");

				{
					// SOURCE transaction (where A is created)
					PGC_Transaction source(user, false, PGC::PromotionStrategy::DoubleDipping);

					Assert::IsTrue(user.TotalNetMemoryUsed() == sizeof(TestPGC_B) - PGC_OVERHEAD, L"Net after source start");

					Assert::IsTrue(user.TotalGrossMemoryUsed() ==
						source.GrossMemoryUsed() +
						destination.GrossMemoryUsed() +
						user.TotalPromotionMemory(),
						L"Gross after source start");

					// Create A in source
					a = new TestPGC_A();

					// Assign to B (promotion request is created)
					sref->SetTestA(a);

					Assert::IsTrue(user.TotalNetMemoryUsed() ==
						sizeof(TestPGC_A) + sizeof(TestPGC_B) - 2 * PGC_OVERHEAD,
						L"Net after SetTestA");

					Assert::IsTrue(user.TotalGrossMemoryUsed() ==
						source.GrossMemoryUsed() +
						destination.GrossMemoryUsed() +
						user.TotalPromotionMemory(),
						L"Gross after SetTestA");

					// Step 1: Trigger PromoteRequests() early via unrelated inner transaction
					{
						PGC_Transaction inner(user, false, PGC::PromotionStrategy::DoubleDipping);
						// Inner exits — this calls PromoteRequests()
					}

					// Step 2: Promotion should still be deferred (Keep)
					Assert::IsTrue(destination.NetMemoryUsed() == sizeof(TestPGC_B) - PGC_OVERHEAD,
						L"Destination should not own TestPGC_A instance yet");

					Assert::IsTrue(user.TotalProcessedDoubleDippingMemory() == 0,
						L"No processed double dipping memory yet");

					Assert::IsTrue(user.TotalNetMemoryUsed() ==
						sizeof(TestPGC_A) + sizeof(TestPGC_B) - 2 * PGC_OVERHEAD,
						L"Net memory still in source");

					Assert::IsTrue(user.TotalGrossMemoryUsed() ==
						source.GrossMemoryUsed() +
						destination.GrossMemoryUsed() +
						user.TotalPromotionMemory(),
						L"Gross memory with pending promotion");
				}

				// Step 3: Source transaction ends — promotion now proceeds as PromotedKeep
				Assert::IsTrue(destination.NetMemoryUsed() ==
					sizeof(TestPGC_A) + sizeof(TestPGC_B) - 2 * PGC_OVERHEAD,
					L"Destination owns object after promotion");

				Assert::IsTrue(user.TotalProcessedDoubleDippingMemory() == sizeof(PGC::PGC_Promotion),
					L"Processed memory should now include promotion entry");

				Assert::IsTrue(user.TotalNetMemoryUsed() ==
					sizeof(TestPGC_A) + sizeof(TestPGC_B) - 2 * PGC_OVERHEAD,
					L"Net memory reflects destination ownership");

				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					destination.GrossMemoryUsed() + user.TotalPromotionMemory(),
					L"Gross memory after source cleanup and promotion");
			}

			// Final cleanup
			Assert::IsTrue(user.TotalNetMemoryUsed() == 0, L"Final net memory after all cleanup");

			Assert::IsTrue(user.TotalGrossMemoryUsed() == user.TotalPromotionMemory(),
				L"Final gross memory should match promotion memory only");
		}


		/*
		TEST_METHOD(TestConcurrentPromotionStress)
		{
			PGC_User user(AssertErrorHandler);

			Initialize();
			const int NumThreads = 8;
			const int NumIterations = 1000;

			std::atomic<bool> failed{ false };

			// Shared object to be promoted repeatedly
			SRef<TestPGC_A> shared = new TestPGC_A();
			shared->MagicValue = 42;  // some known value for validation

			auto promote_task = [&]()
			{
				for (int i = 0; i < NumIterations; ++i)
				{
					try
					{
						PGC_Transaction txn;

						// Manually request promotion — simulate contention
						shared->RequestPromotion((PGC::PGC_TypeCheck**)&shared);

						// Validate the object is still sane
						if (shared.Ptr() == nullptr || shared->MagicValue != 42)
						{
							failed = true;
							break;
						}
					}
					catch (...)
					{
						failed = true;
						break;
					}
				}
			};

			// Launch multiple threads
			std::vector<std::thread> threads;
			for (int i = 0; i < NumThreads; ++i)
			{
				threads.emplace_back(promote_task);
			}

			// Wait for all to finish
			for (auto& t : threads)
			{
				t.join();
			}

			Assert::IsFalse(failed, L"Concurrent promotion caused failure or corruption");
			Cleanup();
		}

		TEST_METHOD(TestConcurrentPromotionControlled)
		{
			PGC_User user(AssertErrorHandler);

			std::atomic<bool> failed{ false };

			// Shared object to promote
			SRef<TestPGC_A> shared = new TestPGC_A();
			shared->MagicValue = 99;

			// Create a reusable barrier for exact sync
			std::barrier sync_point(2); // requires C++20

			// Thread function
			auto promote_task = [&](int threadId)
			{
				PGC_Transaction txn;

				// Ensure both threads reach this point before proceeding
				sync_point.arrive_and_wait();  // Lined up for simultaneous promotion

				try
				{
					shared->RequestPromotion((PGC::PGC_Base**)&shared);

					if (shared == nullptr || shared->MagicValue != 99)
					{
						failed = true;
					}
				}
				catch (...)
				{
					failed = true;
				}
			};

			// Launch exactly two threads
			std::thread t1(promote_task, 1);
			std::thread t2(promote_task, 2);

			t1.join();
			t2.join();

			Assert::IsFalse(failed, L"Controlled concurrent promotion caused corruption or crash");
		}
		*/
	};
}

