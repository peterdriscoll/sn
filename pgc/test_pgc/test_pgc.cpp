#include "pgc.h"

#include "test_pgc_pch.h"
#include "CppUnitTest.h"

using namespace PGCX;

#include "testpgc_a.h"
#include "testpgc_b.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test_pgc
{
	TEST_CLASS(test_pgc)
	{
	private:
		void Initialize()
		{
			Transaction::ResetNetMemoryUsed();
			Transaction::ResetGrossMemoryUsed();
		}

		void Cleanup()
		{
			Assert::IsTrue(Transaction::TotalNetMemoryUsed() == 0, L"No memory used");
			Assert::IsTrue(Promotion::PromotionUsedMemory() == 0, L"No promotional memory used.");
			Assert::IsTrue(Promotion::PromotionFreeMemory() == Transaction::TotalGrossMemoryUsed(), L"PromotionFreeMemory == TotalGrossMemoryUsed");
		}

		// UTILITY

		void RecursivelyDoThis(long p_Depth)
		{
			size_t totalNetMemoryUsed = Transaction::TotalNetMemoryUsed() - Transaction::TopTransaction()->NetMemoryUsed();
			size_t totalGrossMemoryUsed = Transaction::TotalGrossMemoryUsed() - Transaction::TopTransaction()->GrossMemoryUsed();
			Transaction::StartStackTransaction();
			if (p_Depth > 0)
			{
				SRef<TestPgc_A> x = new TestPgc_A();
				SRef<TestPgc_A> y = new TestPgc_A();
				SRef<TestPgc_B> z = new TestPgc_B();

				RecursivelyDoThat(100);
				RecursivelyDoThis(p_Depth - 1);
			}
			Transaction::EndStackTransaction();
			Assert::IsTrue(totalNetMemoryUsed + Transaction::TopTransaction()->NetMemoryUsed() == Transaction::TotalNetMemoryUsed(), L"Sum of net memory = total");
			Assert::IsTrue(totalGrossMemoryUsed + Transaction::TopTransaction()->GrossMemoryUsed() == Transaction::TotalGrossMemoryUsed(), L"Sum of gross memory = total");
			Assert::IsTrue(Promotion::PromotionUsedMemory() == 0, L"No promotions");
		}

		void RecursivelyDoThat(long p_Depth)
		{
			size_t totalNetMemoryUsed = Transaction::TotalNetMemoryUsed() - Transaction::TopTransaction()->NetMemoryUsed();
			size_t totalGrossMemoryUsed = Transaction::TotalGrossMemoryUsed() - Transaction::TopTransaction()->GrossMemoryUsed();
			Transaction::StartStackTransaction();
			if (p_Depth > 0)
			{
				SRef<TestPgc_A> x = new TestPgc_A();
				SRef<TestPgc_A> y = new TestPgc_A();
				SRef<TestPgc_B> z = new TestPgc_B();

				RecursivelyDoThat(p_Depth - 1);
			}
			Transaction::EndStackTransaction();
			Assert::IsTrue(totalNetMemoryUsed + Transaction::TopTransaction()->NetMemoryUsed() == Transaction::TotalNetMemoryUsed(), L"Sum of net memory = total");
			Assert::IsTrue(totalGrossMemoryUsed + Transaction::TopTransaction()->GrossMemoryUsed() == Transaction::TotalGrossMemoryUsed(), L"Sum of gross memory = total");
			Assert::IsTrue(Promotion::PromotionUsedMemory() == 0, L"No promotions");
		}

		void RecursivelyDoKeep(TestPgc_A *keep, long p_Depth)
		{
			Transaction::StartStackTransaction();
			if (p_Depth > 0)
			{
				if (p_Depth == 1)
				{
					long dog = 10;
				}
				SRef<TestPgc_A> newKeep = new TestPgc_A();
				keep->SetNext(newKeep);
				SRef<TestPgc_B> z = new TestPgc_B();
				SRef<TestPgc_A> y = new TestPgc_A();

				RecursivelyDoKeep(newKeep, p_Depth - 1);
			}
			Transaction::EndStackTransaction();
		}

	public:
		// TESTS.

		//  General test of the Promotional Garbage Collector memory management.
		//  Test multi level promotion.
		TEST_METHOD(TestBasicPGC)
		{
			Initialize();

			{
				Transaction parentTransaction;
				SRef<TestPgc_B> b = new TestPgc_B();

				Assert::AreEqual(parentTransaction.NetMemoryUsed(), sizeof(TestPgc_B) - OVERHEAD, L"Memory used by TestPgc_B");
				Assert::IsTrue(parentTransaction.NetMemoryUsed() == Transaction::TotalNetMemoryUsed());
				Assert::IsTrue(parentTransaction.GrossMemoryUsed() + Promotion::PromotionUsedMemory() == Transaction::TotalGrossMemoryUsed());

				{
					Transaction transaction;
					SRef<TestPgc_A> a;
					a = new TestPgc_A();
					a->SetDescription("a");
					b->SetTestA((TestPgc_A *)a);
					SRef<TestPgc_A> a1 = new TestPgc_A();
					a1->SetDescription("a1");
					a->SetNext(a1);
					SRef<TestPgc_A> a2 = new TestPgc_A();
					a2->SetDescription("a2");
					a1->SetNext(a2);
					SRef<TestPgc_A> a3 = new TestPgc_A();
					a3->SetDescription("a3");
					a2->SetNext(a3);

					Assert::IsTrue(parentTransaction.NetMemoryUsed() + transaction.NetMemoryUsed() == Transaction::TotalNetMemoryUsed(), L"NetMemoryUsed == TotalNetMemoryUsed");
					Assert::IsTrue(parentTransaction.GrossMemoryUsed() + transaction.GrossMemoryUsed() + Promotion::PromotionUsedMemory() == Transaction::TotalGrossMemoryUsed(), L"PromotionUsedMemory == TotalGrossMemoryUsed");

					Assert::IsTrue(TestPgc_A::m_ActiveCount == 4);
					Assert::IsTrue(TestPgc_B::m_ActiveCount == 1);

					{
						Transaction childTransaction;

						SRef<TestPgc_A> c;
						c = new TestPgc_A();
						c->SetDescription("c");
						SRef<TestPgc_A> c1 = new TestPgc_A();
						c1->SetDescription("c1");
						c->SetNext(c1);
						SRef<TestPgc_A> c2 = new TestPgc_A();
						c2->SetDescription("c2");
						c1->SetNext(c2);
						SRef<TestPgc_A> c3 = new TestPgc_A();
						c3->SetDescription("c3");
						c2->SetNext(c3);

						a3->SetNext(c2);

						Assert::IsTrue(TestPgc_A::m_ActiveCount == 8);
						Assert::IsTrue(TestPgc_B::m_ActiveCount == 1);

						Assert::IsTrue(parentTransaction.NetMemoryUsed() + transaction.NetMemoryUsed() + childTransaction.NetMemoryUsed() == Transaction::TotalNetMemoryUsed(), L"Sum of net memory used == total");
						Assert::IsTrue(parentTransaction.GrossMemoryUsed() + transaction.GrossMemoryUsed() + childTransaction.GrossMemoryUsed() + Promotion::PromotionUsedMemory() == Transaction::TotalGrossMemoryUsed(), L"Sum of gross and promotional memory used == total gross");
					}

					Assert::IsTrue(TestPgc_A::m_ActiveCount == 6);
					Assert::IsTrue(TestPgc_B::m_ActiveCount == 1);

					b->SetTestA(a3->GetNext());
					a3->SetNext(0);
					b->SetTestA((TestPgc_A *)a);

					Assert::IsTrue(parentTransaction.NetMemoryUsed() + transaction.NetMemoryUsed() == Transaction::TotalNetMemoryUsed(), L"Sum of net memory used == total");
					Assert::IsTrue(parentTransaction.GrossMemoryUsed() + transaction.GrossMemoryUsed() + Promotion::PromotionUsedMemory() == Transaction::TotalGrossMemoryUsed(), L"Sum of gross and promotional memory used == total gross");
					// Promotion occurs in the destructor of the transaction here.
				}

				Assert::IsTrue(parentTransaction.NetMemoryUsed() == Transaction::TotalNetMemoryUsed(), L"Parent net == Total net");
				Assert::IsTrue(parentTransaction.GrossMemoryUsed() + Promotion::PromotionFreeMemory() + Promotion::PromotionUsedMemory() == Transaction::TotalGrossMemoryUsed(), L"Gross + promotional = total gross");

				SRef<TestPgc_A> xa = b->GetTestA();
				Assert::IsTrue(xa->GetDescription() == "a");
				SRef<TestPgc_A> xa1 = xa->GetNext();
				Assert::IsTrue(xa1->GetDescription() == "a1");
				SRef<TestPgc_A> xa2 = xa1->GetNext();
				Assert::IsTrue(xa2->GetDescription() == "a2");
				SRef<TestPgc_A> xa3 = xa2->GetNext();
				Assert::IsTrue(xa3->GetDescription() == "a3");
				Assert::IsTrue(xa3->GetNext() == 0);

				Assert::IsTrue(TestPgc_A::m_ActiveCount == 4);
				Assert::IsTrue(TestPgc_B::m_ActiveCount == 1);

				// All destructors called here in the destructor of the transaction.
			}

			Assert::IsTrue(Transaction::TotalNetMemoryUsed() == 0, L"Net mwmory cleared");
			Assert::IsTrue(Promotion::PromotionUsedMemory() == 0, L"Promotional memory cleared");
			Assert::IsTrue(Promotion::PromotionFreeMemory() == Transaction::TotalGrossMemoryUsed(), L"Promotional free memory == total gross");

			Assert::IsTrue(TestPgc_A::m_ActiveCount == 0, L"All TestPgc_A destructors called.");  //  All destructors called.
			Assert::IsTrue(TestPgc_B::m_ActiveCount == 0, L"All TestPgc_B destructors called.");

			Cleanup();
		}

		//  Test automatic transaction creation.  No promotion.
		//  * StartStackTransaction
		//  * EndStackTransaction
		TEST_METHOD(TestRecursivePGC)
		{
			Initialize();

			{
				Transaction transaction;
				{
					Transaction::StartStackTransaction();
					{
						Transaction transaction;
						RecursivelyDoThis(100);
					}
					Transaction::EndStackTransaction();
				}
			}
			Assert::IsTrue(Transaction::TotalNetMemoryUsed() == 0, L"Net memory cleared");
			Assert::IsTrue(Promotion::PromotionUsedMemory() == 0, L"Promotional memory cleared");
			Assert::IsTrue(Promotion::PromotionFreeMemory() == Transaction::TotalGrossMemoryUsed(), L"Free memory == gross memory");

			Assert::IsTrue(TestPgc_A::m_ActiveCount == 0, L"All TestPgc_A destructors called.");
			Assert::IsTrue(TestPgc_B::m_ActiveCount == 0, L"All TestPgc_B destructors called.");

			Cleanup();
		}

		//  Test automatic transaction creation with promotion.
		TEST_METHOD(TestRecursiveKeepPGC)
		{
			Initialize();

			Assert::IsTrue(Promotion::PromotionUsedMemory() == 0, L"Promotional memory cleared");
			Assert::IsTrue(Promotion::PromotionFreeMemory() == Transaction::TotalGrossMemoryUsed(), L"Promotional free memeory == gross");
			{
				Transaction parentTransaction;
				long depth = 2000; //583
				SRef<TestPgc_A> keep = new TestPgc_A();
				{
					Transaction transaction;
					RecursivelyDoKeep(keep, depth);
					long allocatedInFirstTransaction = depth;
					long numThatFitsInFirstTransaction = 582;
					if (numThatFitsInFirstTransaction < allocatedInFirstTransaction)
					{
						allocatedInFirstTransaction = numThatFitsInFirstTransaction;
					}
					size_t total = Transaction::TotalNetMemoryUsed();
					size_t size_A1 = (depth + 1) * (sizeof(TestPgc_A) - OVERHEAD);
					size_t size_A = allocatedInFirstTransaction * (sizeof(TestPgc_A) - OVERHEAD);
					size_t size_B = allocatedInFirstTransaction * (sizeof(TestPgc_B) - OVERHEAD);
					size_t size_total = size_A1 + size_A + size_B;
					Assert::IsTrue(total == size_total, L"Memory usage accounted for.");
				}
				long countKeep = keep->CountList();
				Assert::IsTrue(countKeep == depth, L"Kept 1 for each recursion");
				Assert::IsTrue(Transaction::TotalNetMemoryUsed() == (countKeep + 1) * (sizeof(TestPgc_A) - OVERHEAD), L"Memory usage accounted for.");

				Assert::IsTrue(TestPgc_A::m_ActiveCount == countKeep + 1, L"TestPgc_A active is correct.");
				Assert::IsTrue(TestPgc_B::m_ActiveCount == 0, L"TestPgc_AB active is zero.");
			}
			Assert::IsTrue(Transaction::TotalNetMemoryUsed() == 0, L"Net memory cleared");
			Assert::IsTrue(Promotion::PromotionUsedMemory() == 0, L"Promotional memory cleared");
			Assert::IsTrue(Promotion::PromotionFreeMemory() == Transaction::TotalGrossMemoryUsed(), L"Promotional free memory = gross");

			Assert::IsTrue(TestPgc_A::m_ActiveCount == 0, L"All TestPgc_A destructors called."); 
			Assert::IsTrue(TestPgc_B::m_ActiveCount == 0, L"All TestPgc_AB destructors called.");
		}

		TEST_METHOD(TestReferenceCyclePGC)
		{
			Initialize();

			size_t stackTramsactionSize = sizeof(StackTransaction);
			Assert::IsTrue(stackTramsactionSize == 1, L"Stack transaction size is one byte");

			Assert::IsTrue(Promotion::PromotionUsedMemory() == 0, L"Promotional memory cleared");
			Assert::IsTrue(Promotion::PromotionFreeMemory() == Transaction::TotalGrossMemoryUsed(), L"Promotional free memory == gross");
			{
				Transaction parentTransaction;
				SRef<TestPgc_A> a = new TestPgc_A();
				a->SetDescription("TestPgc_A a");
				{
					Transaction transaction;
					TestPgc_A *b = new TestPgc_A();
					a->SetNext(b);
					TestPgc_A *c = new TestPgc_A();
					b->SetNext(c);
					b->SetDescription("TestPgc_A b");
					c->SetNext(a);
					c->SetDescription("TestPgc_A c");
				}

				Assert::IsTrue(a->GetDescription() == "TestPgc_A a", L"a Correct description");
				Assert::IsTrue(a->GetNext()->GetDescription() == "TestPgc_A b", L"b Correct description");
				Assert::IsTrue(a->GetNext()->GetNext()->GetDescription() == "TestPgc_A c", L"c Correct description");

				Assert::IsTrue(a->GetNext()->GetNext()->GetNext()->GetDescription() == "TestPgc_A a", L"a Loop correct description");
				Assert::IsTrue(a->GetNext()->GetNext()->GetNext()->GetNext()->GetDescription() == "TestPgc_A b", L"b Loop correct description");
				Assert::IsTrue(a->GetNext()->GetNext()->GetNext()->GetNext()->GetNext()->GetDescription() == "TestPgc_A c", L"c Loop correct description");

				Assert::IsTrue(Transaction::TotalNetMemoryUsed() == 3 * (sizeof(TestPgc_A) - OVERHEAD));

				Assert::IsTrue(TestPgc_A::m_ActiveCount == 3, L"Correct active count TestPgc_A");
				Assert::IsTrue(TestPgc_B::m_ActiveCount == 0, L"Correct active count TestPgc_B");
			}
			Assert::IsTrue(Transaction::TotalNetMemoryUsed() == 0, L"Net memory cleared");
			Assert::IsTrue(Promotion::PromotionUsedMemory() == 0, L"Promotional memory cleared");
			Assert::IsTrue(Promotion::PromotionFreeMemory() == Transaction::TotalGrossMemoryUsed(), L"Promotional free memory == gross");

			Assert::IsTrue(TestPgc_A::m_ActiveCount == 0, L"All TestPgc_A destructors called.");  //  
			Assert::IsTrue(TestPgc_B::m_ActiveCount == 0, L"All TestPgc_B destructors called.");

			// Close the transaction setup by the static variables.
			delete Transaction::TopTransaction();

			Cleanup();
		}

	};

}