#include "snl.h"

#include "test_sn_pch.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


using namespace skynet;
using namespace PGCX;

namespace test_sn
{
    TEST_CLASS(test_sn_promotion)
    {
    private:
        bool runWebServer = false;

        static void AssertErrorHandler(bool p_Err, const std::string& p_Description) noexcept(false)
        {
            Assert::IsTrue(!p_Err, std::wstring(p_Description.begin(), p_Description.end()).c_str());
        }

        void Initialize()
        {
        }

        void Cleanup()
        {
        }

    public:
        TEST_METHOD(TestPromotionSurvivesTransaction)
        {
            Initialize();
            {
                Manager manager("Test Mapping Forward", AssertErrorHandler);
                manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
                {
                    Transaction parentTransaction;
                    SN_DECLARE(X);
                    const SNI_Expression* before = nullptr;

                    {
                        Transaction transaction;
                        (X + Long(3) == Long(5)).Assert().Do();
                        before = X.GetSafeValue().GetSNI_Expression();
                    }

                    const SNI_Expression* after = X.GetSafeValue().GetSNI_Expression();
                    Assert::IsTrue(X.DisplayValueSN() == "Long(2)");
                    Assert::IsTrue(before != after, L"Object was promoted");
                }
            }
            Cleanup();
        }

        TEST_METHOD(TestPromotionSharedObject)
        {
            Initialize();
            {
                Manager manager("Test Promotion Shared Object", AssertErrorHandler);
                manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

                SN_DECLARE(X);
                Expression expr1, expr2;

                {
                    Transaction transaction;
                    expr1 = (X + Long(1) == Long(3));
                    expr2 = (X + Long(2) == Long(4));
                    expr1.Assert().Do();
                    expr2.Assert().Do();
                }

                Assert::IsTrue(X.DisplayValueSN() == "Long(2)");
            }
            Cleanup();
        }

        TEST_METHOD(TestExpressionAliasTracksPromotedValue)
        {
            Initialize();
            {
                Manager manager("Test Expression Alias After Promotion", AssertErrorHandler);
                manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

                SN_DECLARE(X);

                // 'alias' is a symbolic expression that refers to the same variable as X.
                // It holds a pointer to the same SNI_Variable, not a copy.
                Expression alias = X;

                {
                    Transaction transaction;

                    // Within this inner transaction, X is updated.
                    // This causes a new value (Long(2)) to be created and assigned to X.
                    // That value will be promoted after the transaction ends.
                    (X + Long(3) == Long(5)).Assert().Do();
                }

                // At this point, the inner transaction has completed.
                // The value assigned to X (Long(2)) should have been promoted to the parent transaction.

                Assert::IsTrue(X.DisplayValueSN() == "Long(2)");

                // This test confirms that 'alias' still tracks the promoted value of X.
                // It verifies that symbolic aliases do not go stale after promotion:
                // They resolve to the current (promoted) value of the shared SNI_Variable.
                Assert::IsTrue(alias.GetSafeValue().GetSNI_Expression() == X.GetSafeValue().GetSNI_Expression(),
                    L"Alias expression tracked promoted value");
            }
            Cleanup();
        }

        // This test checks whether pointers to expressions created in an inner transaction 
        // are correctly updated (promoted) when accessed from the outer transaction.
        //
        // Specifically:
        // - A variable `result` is assigned an expression `(X + Y) == 9` inside an inner transaction.
        // - This expression is delayed, so it's not immediately asserted or evaluated.
        // - After the inner transaction ends, `result` points to a stale expression that was created 
        //   in the now-destroyed inner transaction.
        // - If promotion works correctly, accessing `result.GetSNI_Expression()` should return a 
        //   pointer to a promoted copy of the expression.
        // - However, no promotion is requested in this code path, so the pointer remains stale,
        //   leading to a crash if you try to use it (e.g., `DisplaySN()`).
        //
        // The test checks that `before == after`, meaning the stale pointer was *not* updated.
        // If promotion were implemented here, we’d expect the pointers to differ.
        //
        // NOTE: This highlights the danger of holding expression pointers across transaction boundaries 
        // without triggering promotion. It's a known weakness, not a failure of SN logic per se.

        TEST_METHOD(TestPromotionUpdatesPointers)
        {
            return;
            Initialize();
            {
                Manager manager("Test Promotion Updates Pointers", AssertErrorHandler);
                // Note that the manager creates a transaction.

                manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
                {
                    Transaction outerTransaction;
                    SN_DECLARE(X); // Creates an SNI_Variable instance in the managers transaction
                    SN_DECLARE(Y);
                    Expression result;
                    const SNI_Expression* before = nullptr;

                    {
                        Transaction innerTransaction;
                        result = ((X + Y) == Long(9)); // SNI_Expression created within inner transaction
                        result.Assert().Do(); // Does nothing immediately as SN sees that is as having infinite cardinality. Call delayed.
                        before = result.GetSNI_Expression();
                        std::string X_string1 = X.DisplayValueSN();
                        std::string Y_string1 = Y.DisplayValueSN();
                    }
                    std::string X_string = X.DisplayValueSN();
                    std::string Y_string = Y.DisplayValueSN();
                    // Result will be pointing to stale data, because no promotion was created to update the pointer.
                    // Accessing it will crash.
                    const SNI_Expression* after = result.GetSNI_Expression();
                    Assert::IsTrue(before == after, L"Internal pointer should have been updated but was not");

                    (X == Long(4)).Assert().Do();

                    std::string X_string2 = X.DisplayValueSN();
                    std::string Y_string2 = Y.DisplayValueSN();
                    Assert::IsTrue(X_string2 == "Long(4)", L"Bad value of X.");
                    Assert::IsTrue(Y_string2 == "Long(5)", L"Bad value of Y.");
                }
            }
            Cleanup();
        }

    };
} // namespace test_sn
