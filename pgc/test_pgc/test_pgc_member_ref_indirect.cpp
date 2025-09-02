#include "pgc.h"

#include "test_pgc_pch.h"
#include "CppUnitTest.h"

#include "testpgc_a.h"
#include "testpgc_b.h"
#include "test_pgc_c.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test_pgc
{
	TEST_CLASS(test_pgc_member_ref_indirect)
	{
	private:
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

	public:
		TEST_METHOD(TestSimplePromotionOnMemberRef)
		{
			PGC_User user(nullptr, AssertErrorHandler);

			{
				PGCX::PGC_Transaction outerTransaction(user);

				SRef<TestPGC_C> outerRef = new TestPGC_C();
				TestPGC_C* instance = NULL;
				Base* instanceBase = NULL;
				{
					PGCX::PGC_Transaction innerTransaction(user);

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
		TEST_METHOD(TestPromotionResult_Dropped_DyingDestination)
		{
			PGC_User user(nullptr, AssertErrorHandler);
			{
				PGCX::PGC_Transaction source(user, false, PGC::PromotionStrategy::DoubleDipping);

				TestPGC_B* dyingDestination= new TestPGC_B();

				Assert::IsTrue(user.TotalNetMemoryUsed() == sizeof(TestPGC_B)- PGC_OVERHEAD);
				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					source.GrossMemoryUsed() + 
					user.TotalPromotionMemory(),
					L"Gross memory error");

				{
					PGCX::PGC_Transaction destination(user, false, PGC::PromotionStrategy::DoubleDipping);
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
			PGC_User user(nullptr, AssertErrorHandler);

			{
				// SOURCE transaction (outer)
				PGCX::PGC_Transaction source(user, false, PGC::PromotionStrategy::Backstabbing);

				// Allocate TestPGC_A in source
				TestPGC_A* a = new TestPGC_A();

				Assert::IsTrue(user.TotalNetMemoryUsed() == sizeof(TestPGC_A) - PGC_OVERHEAD, L"Net after allocating A");
				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					source.GrossMemoryUsed() + user.TotalPromotionMemory(),
					L"Gross after allocating A");

				{
					// DESTINATION transaction (inner)
					PGCX::PGC_Transaction destination(user, false, PGC::PromotionStrategy::Backstabbing);

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

			Assert::IsTrue(user.TotalProcessedRefAttachedMemory() == 0, L"No processed memory for Backstabbing");

			// Final cleanup
			Assert::IsTrue(user.TotalNetMemoryUsed() == 0, L"Final net memory");
			Assert::IsTrue(user.TotalGrossMemoryUsed() == user.TotalPromotionMemory(), L"Final gross memory");
		}

		TEST_METHOD(TestPromotionResult_Nested_PromotedDone_Backstabbing_ThroughTestA)
		{
			PGC_User user(nullptr, AssertErrorHandler);

			{
				// DESTINATION transaction (must outlive source)
				PGCX::PGC_Transaction destination(user, false, PGC::PromotionStrategy::Backstabbing);

				TestPGC_A* a = nullptr;

				SRef<TestPGC_B> sref = new TestPGC_B();

				Assert::IsTrue(user.TotalNetMemoryUsed() == sizeof(TestPGC_B) - PGC_OVERHEAD, L"Initial net memory");
				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					destination.GrossMemoryUsed() + user.TotalPromotionMemory(),
					L"Initial gross memory");

				{
					// SOURCE transaction
					PGCX::PGC_Transaction source(user, false, PGC::PromotionStrategy::Backstabbing);

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

				Assert::IsTrue(user.TotalProcessedRefAttachedMemory() == 0,
					L"No processed memory (Backstabbing)");

				// Check that GetTestA returns the same pointer
				Assert::IsTrue(sref->GetTestA() != a, L"Promoted object must match original pointer");
			}

			// Destination dies — full cleanup
			Assert::IsTrue(user.TotalNetMemoryUsed() == 0, L"Final net memory");
			Assert::IsTrue(user.TotalGrossMemoryUsed() == user.TotalPromotionMemory(), L"Final gross memory");
		}

		TEST_METHOD(TestPromotionResult_Nested_PromotedKeep_DoubleDipping_ThroughTestA)
		{
			PGC_User user(nullptr, AssertErrorHandler);

			TestPGC_A* a = nullptr;

			{
				// DESTINATION transaction (lives longer)
				PGCX::PGC_Transaction destination(user, false, PGC::PromotionStrategy::DoubleDipping);

				SRef<TestPGC_B> sref = new TestPGC_B();

				Assert::IsTrue(user.TotalNetMemoryUsed() == sizeof(TestPGC_B) - PGC_OVERHEAD, L"Initial net memory");
				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					destination.GrossMemoryUsed() + user.TotalPromotionMemory(),
					L"Initial gross memory");

				{
					// SOURCE transaction (where A is created)
					PGCX::PGC_Transaction source(user, false, PGC::PromotionStrategy::DoubleDipping);

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
					Assert::IsTrue(user.TotalProcessedRefAttachedMemory() == 0,
						L"Processed double dipping memory should be zero before promotion");
				}

				// Source is now dead -> triggers PromotedKeep
				Assert::IsTrue(user.TotalProcessedRefAttachedMemory() == sizeof(PGC::PGC_Promotion),
					L"Processed memory updated after PromotedKeep");

				// Final access — triggers second dip, accessing the promoted copy.
				TestPGC_A* returnedAgain = sref->GetTestA();

				Assert::IsTrue(returnedAgain != a, L"Returned pointer is to the promoted copy");
				Assert::IsTrue(user.TotalProcessedRefAttachedMemory() == 0,
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
			PGC_User user(nullptr, AssertErrorHandler);

			TestPGC_A* a = nullptr;

			{
				// DESTINATION transaction (longer-lived)
				PGCX::PGC_Transaction destination(user, false, PGC::PromotionStrategy::DoubleDipping);

				// Allocate B in destination
				SRef<TestPGC_B> sref = new TestPGC_B();

				Assert::IsTrue(user.TotalNetMemoryUsed() == sizeof(TestPGC_B) - PGC_OVERHEAD, L"Initial net memory");
				Assert::IsTrue(user.TotalGrossMemoryUsed() ==
					destination.GrossMemoryUsed() + user.TotalPromotionMemory(),
					L"Initial gross memory");

				{
					// SOURCE transaction (where A is created)
					PGCX::PGC_Transaction source(user, false, PGC::PromotionStrategy::DoubleDipping);

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
						PGCX::PGC_Transaction inner(user, false, PGC::PromotionStrategy::DoubleDipping);
						// Inner exits — this calls PromoteRequests()
					}

					// Step 2: Promotion should still be deferred (Keep)
					Assert::IsTrue(destination.NetMemoryUsed() == sizeof(TestPGC_B) - PGC_OVERHEAD,
						L"Destination should not own TestPGC_A instance yet");

					Assert::IsTrue(user.TotalProcessedRefAttachedMemory() == 0,
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

				Assert::IsTrue(user.TotalProcessedRefAttachedMemory() == sizeof(PGC::PGC_Promotion),
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
	};
}

