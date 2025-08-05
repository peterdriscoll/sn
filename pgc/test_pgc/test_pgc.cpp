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
		Assert::IsTrue(!p_Err, wstring(p_Description.begin(), p_Description.end()).c_str());
	};
	
	void ThrowErrorHandler(bool p_Err, const string& p_Description)
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

		TEST_METHOD(TestReferenceCyclePGC)
		{
			PGC_User user(AssertErrorHandler);

			size_t stackTramsactionSize = sizeof(StackTransaction);
			Assert::IsTrue(stackTramsactionSize == 1, L"Stack transaction size is one byte");

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

