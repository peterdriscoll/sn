#include "pgc.h"

#include "test_pgc_pch.h"

#include "snl.h"

const std::string doc_root = SOLUTION_DIR "html";

using namespace skynet;

#include "CppUnitTest.h"

#include <string>

#ifdef _WIN32
#include <windows.h>
#include <cstdio>
#include <conio.h>

static void EnsureConsoleForIO() {
    HWND hw = GetConsoleWindow();
    if (hw== nullptr) {
        FreeConsole();
        if (!AllocConsole()) return;

        FILE* fp;
        freopen_s(&fp, "CONIN$", "r", stdin);
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);

        // Make the window visible and foreground (it can spawn behind VS)
        hw = GetConsoleWindow();
        if (hw) {
            ShowWindow(hw, SW_SHOW);
            SetForegroundWindow(hw);
            SetConsoleTitleA("PGC Test Console");
        }

        std::cout << "[EnsureConsoleForIO] hwnd=" << hw
            << "  press any key in this window...\n" << std::flush;
    }
}
#endif

#include "something.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define PGC_CLASS_LIST(ACTION) \
	ACTION(DORI::Something)

//PGC_DEFINE_CLASS_REGISTRY(ClassRegistry);
const ::PGC::RegEntry ClassRegistry[] = { 
	::PGC::make_entry<DORI::Something>("Something"),
	::PGC::kEndSentinel };

namespace test_pgc
{

	TEST_CLASS(test_pgc_dori)
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
		TEST_METHOD(TestDORI_Something)
		{
			PGC_User user(ClassRegistry, &AssertErrorHandler);
			{
                return; // Temmporarily disabled because it requires user input.
                PGCX::PGC_Transaction parentTransaction(user, false, PGC::PromotionStrategy::DoubleDipping);

                PGC::RefA<DORI::Something> waggy =
                    DORI::make_dori<DORI::Something>(parentTransaction, "Waggy", "Cavoodle");

                PGC::RefA<DORI::Something> katara =
                    DORI::make_dori<DORI::Something>(parentTransaction, "Katara", "Samoyed", waggy);

                PGC::RefA<DORI::Something> luna =
                    DORI::make_dori<DORI::Something>(parentTransaction, "Luna", "Golden Retriever x Labrador", katara);

                luna->MakeJSON(std::cout);
                std::ostringstream a, b;
                luna->MakeJSON(a);

                PGC::RefA<DORI::Something>::Proxy view(luna);   // builds stack façade over same object
                Assert::IsTrue(view->IsStackAllocated());
                view->MakeJSON(b);

                Assert::AreEqual(a.str(), b.str());
            }
		}

        TEST_METHOD(TestDORI_Something_Promotion)
        {
			return; // Temmporarily disabled because it requires user input.
			EnsureConsoleForIO();

            PGC_User user(ClassRegistry);
            {
                std::jthread worker;               // <-- lives beyond child scope

                PGCX::PGC_Transaction parentTransaction(user, false, PGC::PromotionStrategy::DoubleDipping);
                {
                    PGCX::PGC_Transaction* waggyTxn = nullptr;
                    PGCX::PGC_Transaction* kataraTxn = nullptr;
                    PGCX::PGC_Transaction* lunaTxn = nullptr;

                    PGC::RefA<DORI::Something> list;
                    PGC::RefA<DORI::Something> waggy2;
                    PGC::RefA<DORI::Something> listCopy;
					DORI::Something* listPtr = nullptr;
                    {
                        PGCX::PGC_Transaction childTransaction(user, false, PGC::PromotionStrategy::DoubleDipping);

                        PGC::RefA<DORI::Something> waggy =
                            DORI::make_dori<DORI::Something>(childTransaction, "Waggy", "Cavoodle");

                        PGC::RefA<DORI::Something> katara =
                            DORI::make_dori<DORI::Something>(childTransaction, "Katara", "Samoyed", waggy);

                        PGC::RefA<DORI::Something> luna =
                            DORI::make_dori<DORI::Something>(childTransaction, "Luna", "Golden Retriever x Labrador", katara);

                        waggyTxn = waggy.Get()->GetTransaction();
                        kataraTxn = katara.Get()->GetTransaction();
                        lunaTxn = luna.Get()->GetTransaction();

                        waggy->SetNext(luna); // make a cycle
                        list = luna;
                        waggy2 = waggy;
                        // run in thread luna->MakeJSON(std::cout);

                        {
                            std::ios::sync_with_stdio(true);
                            std::cout << std::unitbuf;                  // flush after every insertion
                            // Make a copy so the thread owns its own handle
                            listPtr = list.Get();

                            worker = std::jthread([l = listPtr, u = &user]() mutable
                            {
                                PGC::PGC_Transaction workerTransaction(*u);

                                std::cerr << "[worker] alive, about to recurse\n";
                                PGC::RefA<DORI::Something> local(l);
                                local->MakeJSON(std::cout);  // may interleave with other output
                                std::cout << std::endl;
                                std::cerr << "[worker] completed task, about to end\n";
                            }); // auto-joins at scope end
                        }
                        WaitForCommandFair("Type 'promote' to move instances mid call", "promote");
                    }
                    PGC::RefA<DORI::Something> luna = list;
                    PGC::RefA<DORI::Something> katara = luna->GetNext();
                    PGC::RefA<DORI::Something> waggy = katara->GetNext();

                    Assert::IsTrue(waggyTxn != waggy.Get()->GetTransaction(), L"Waggy should have been promted");
                    Assert::IsTrue(kataraTxn != katara.Get()->GetTransaction(), L"Waggy should have been promted");
                    Assert::IsTrue(lunaTxn != luna.Get()->GetTransaction(), L"Waggy should have been promted");

                    WaitForCommandFair("Type 'stop' to stop recurrence", "stop");
                    waggy2->Stop(); // break the cycle
                    WaitForCommandFair("Type 'work' to join the worker thread", "work");
                    worker.join();
                    WaitForCommandFair("Worker stopped. Type 'end' to end test", "end");
                }
            }
        }

        /*
        TEST_METHOD(TestDORI_Something_MoveTo)
        {
            PGC_User user(ClassRegistry, &AssertErrorHandler);

            // Parent txn (heap-managed Something lives here initially)
            PGCX::PGC_Transaction parentTxn(user, autoCommit=false,
                PGC::PromotionStrategy::DoubleDipping);

            // 1) Construct managed object
            PGC::RefA<DORI::Something> r = DORI::make_dori<DORI::Something>(parentTxn);

            // Heap façade invariants
            Assert::IsFalse(r->IsStackAllocated(), L"Managed façade should NOT be stack-mode");

            // 2) Baseline JSON from managed façade
            std::ostringstream os0;
            r->MakeJSON(os0);
            const auto json0 = os0.str();
            Assert::IsTrue(!json0.empty(), L"Baseline JSON should not be empty");

            // 3) Build a stack façade proxy over the same object
            DORI::Proxy<DORI::Something> view(r);  // expects Something(RefA<>, StackAllocationTag) or equivalent
            Assert::IsTrue(view->IsStackAllocated(), L"Proxy façade must be stack-mode");

            // Stack façade JSON should match baseline (read-only)
            std::ostringstream os1;
            //view->MakeJSON(os1);
            const auto json1 = os1.str();
            Assert::AreEqual(json0, json1, L"Stack façade JSON must equal managed façade JSON");

            // 4) Relocate the managed object to a new transaction
            PGCX::PGC_Transaction childTxn(user, autoCommit=false,
                PGC::PromotionStrategy::DoubleDipping);

            // Use your actual MoveTo API here (name/signature may differ):
            // Example patterns (pick your real one):
            //   auto r2 = r.MoveTo(childTxn);
            //   auto r2 = PGC::MoveTo(r, childTxn);
            //   r = r.MoveTo(childTxn);   // if it mutates/ref-returns
            auto r2 = r.MoveTo(childTxn);   // <-- REPLACE with your real call

            // Managed façade still NOT stack, and now owned by child
            Assert::IsFalse(r2->IsStackAllocated(), L"Relocated façade should still be heap-mode");

            // 5) JSON after relocation
            std::ostringstream os2a, os2b;
            r2->MakeJSON(os2a);       // from managed façade in new txn
            view->MakeJSON(os2b);     // from old stack façade (must still work!)
            const auto json2a = os2a.str();
            const auto json2b = os2b.str();

            Assert::AreEqual(json2a, json2b, L"Stack façade must remain valid after relocation");
            // Optional: if MakeJSON is pure, new JSON should still match baseline
            // Assert::AreEqual(json0, json2a, L"Relocation should not change MakeJSON output");
        }
    */
    };
}

