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

namespace test_pgc_concurrent
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

	TEST_CLASS(test_pgc_concurrent)
	{
	public:
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
						PGCX::PGC_Transaction txn;

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
				PGCX::PGC_Transaction txn;

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

