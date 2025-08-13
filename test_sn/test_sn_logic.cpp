#include "snl.h"

#include <cstdio>
#include <fstream>

#include "test_sn_pch.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace skynet;
using namespace PGCX;

namespace test_sn
{
	TEST_CLASS(test_sn_logic)
	{
	private:
		bool runWebServer = false;

		static void AssertErrorHandler(bool p_Err, const std::string& p_Description)
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
		TEST_METHOD(TestBoolEquivalent)
		{
			return;
			Initialize();
			{
				Manager manager("Test Bool Equivalent", AssertErrorHandler);

				Assert::IsTrue((Bool(true) == Bool(true)).Equivalent(Function(Function(FunctionCall, Function(Function(FunctionCall, Equals), Bool(true))), Bool(true))));
				Assert::IsTrue(!(Bool(true) == Bool(true)).Equivalent(Function(Function(FunctionCall, Function(Function(FunctionCall, Equals), Bool(true))), Bool(false))));
				Assert::IsTrue((Bool(true) && Bool(false)).Equivalent(Function(Function(FunctionCall, Function(Function(FunctionCall, And), Bool(true))), Bool(false))));
				Assert::IsTrue((Bool(false) || Bool(false)).Equivalent(Function(Function(FunctionCall, Function(Function(FunctionCall, Or), Bool(false))), Bool(false))));
				Assert::IsTrue((!False).Equivalent(Function(Function(FunctionCall, Not), Bool(false))));
				Assert::IsTrue(False.If(False, True).Equivalent(Function(Function(FunctionCall, Function(Function(FunctionCall, Function(Function(FunctionCall, If), False)), False)), True)));				
			}
			Cleanup();
		}

		TEST_METHOD(TestBoolEvaluate)
		{
			Initialize();
			{
				Manager manager("Test Bool Evaluate", AssertErrorHandler);

				{
					Transaction transaction;

					(Bool(true) == Bool(true)).Evaluate().Do();
					(!(Bool(true) == Bool(false))).Evaluate().Do();
				}
				{
					Transaction transaction;

					SN_DECLARE_VALUE(z, Bool(true));
					(Bool(true) == z).Evaluate().Do();

					SN_DECLARE_VALUE(k, True);
					SN_DECLARE_VALUE(l, False);
					SN_DECLARE_VALUE(m, True);
					(k.If(l, m) == False).Evaluate().Do();
					(True.If("dog", "cat") == "dog").Evaluate().Do();
					(False.If("dog", "cat") == "cat").Evaluate().Do();
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestBoolEvaluate2)
		{
			Initialize();
			{
				Manager manager("Test Bool Evaluate2", AssertErrorHandler);

				{
					Transaction transaction;
					//Truth table AND
					(!(Bool(false) && Bool(false))).Evaluate().Do();
					(!(Bool(false) && Bool(true))).Evaluate().Do();
					(!(Bool(true) && Bool(false))).Evaluate().Do();
					(Bool(true) && Bool(true)).Evaluate().Do();
				}
				{
					Transaction transaction;

					// Truth table OR
					(!(Bool(false) || Bool(false))).Evaluate().Do();
					(Bool(false) || Bool(true)).Evaluate().Do();
					(Bool(true) || Bool(false)).Evaluate().Do();
					(Bool(true) || Bool(true)).Evaluate().Do();
				}
				{
					Transaction transaction;

					// Truth table AND
					((Bool(false) && Bool(false)) == Bool(false)).Evaluate().Do();
					((Bool(false) && Bool(true)) == Bool(false)).Evaluate().Do();
					((Bool(true) && Bool(false)) == Bool(false)).Evaluate().Do();
					((Bool(true) && Bool(true)) == Bool(true)).Evaluate().Do();
				}
				{
					Transaction transaction;

					// Truth table OR
					((Bool(false) || Bool(false)) == Bool(false)).Evaluate().Do();
					((Bool(false) || Bool(true)) == Bool(true)).Evaluate().Do();
					((Bool(true) || Bool(false)) == Bool(true)).Evaluate().Do();
					((Bool(true) || Bool(true)) == Bool(true)).Evaluate().Do();
				}
				{
					Transaction transaction;

					// Truth table if A then B else C
					(False.If(False, False) == False).Evaluate().Do();
					(False.If(False, True) == True).Evaluate().Do();
					(False.If(True, False) == False).Evaluate().Do();
					(False.If(True, True) == True).Evaluate().Do();
					(True.If(False, False) == False).Evaluate().Do();
					(True.If(False, True) == False).Evaluate().Do();
					(True.If(True, False) == True).Evaluate().Do();
					(True.If(True, True) == True).Evaluate().Do();
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestBoolPartialEvaluate)
		{
			Initialize();
			{
				Manager manager("Test Bool Partial Evaluate", AssertErrorHandler);

				SN_DECLARE(y);
				SN_DECLARE(x);
				SN_DECLARE_VALUE(z, Bool(true));


				Assert::IsTrue((Bool(true) == y).DoPartialEvaluate().Equivalent(Function(Function(Equals, Bool(true)), y)));
				Assert::IsTrue(!(Bool(true) == y).DoPartialEvaluate().Equivalent(Function(Function(Equals, Bool(true)), x)));
				Assert::IsTrue((Bool(true) == Bool(true)).DoPartialEvaluate().Equivalent(Bool(true)));
				(Bool(true) == z).DoPartialEvaluate().Do();

				Assert::IsTrue((Bool(false) || y).DoPartialEvaluate().Equivalent(y));
				Assert::IsTrue((y || Bool(false)).DoPartialEvaluate().Equivalent(y));
				Assert::IsTrue((Bool(true) || y).DoPartialEvaluate().Equivalent(Bool(true)));
				Assert::IsTrue((y || Bool(true)).DoPartialEvaluate().Equivalent(Bool(true)));

				Assert::IsTrue((Bool(true) && y).DoPartialEvaluate().Equivalent(y));
				Assert::IsTrue((y && Bool(true)).DoPartialEvaluate().Equivalent(y));
				Assert::IsTrue((Bool(false) && y).DoPartialEvaluate().Equivalent(Bool(false)));
				Assert::IsTrue((y && Bool(false)).DoPartialEvaluate().Equivalent(Bool(false)));

				(Bool(false) || z).DoPartialEvaluate().Do();
				(Bool(true) && true).DoPartialEvaluate().Do();

				(Bool(true) && true).DoPartialEvaluate().Do();

				SN_DECLARE(i);
				(i == i).DoPartialEvaluate().Do();

				// If

				SN_DECLARE(k);
				SN_DECLARE(l);

				Assert::IsTrue((k.If(False, True) == l).DoPartialEvaluate().Equivalent((!k == l).DoPartialEvaluate()));
				Assert::IsTrue((k.If(True, False) == l).DoPartialEvaluate().Equivalent((k == l).DoPartialEvaluate()));
				Assert::IsTrue(k.If(True, True).DoPartialEvaluate().Equivalent(True));
				Assert::IsTrue(k.If(False, False).DoPartialEvaluate().Equivalent(False));
				Assert::IsTrue(True.If(k, l).DoPartialEvaluate().Equivalent(k));
				Assert::IsTrue(False.If(k, l).DoPartialEvaluate().Equivalent(l));

				Assert::IsTrue(True.If(True, False).DoPartialEvaluate().Equivalent(True));

				// Not
				Assert::IsTrue((!!k).DoPartialEvaluate().Equivalent(k));
				Assert::IsTrue((!(k.If(False, True))).DoPartialEvaluate().Equivalent(k));
			}
			Cleanup();
		}

		TEST_METHOD(TestBoolAssert)
		{
			Initialize();
			{
				Manager manager("Test Bool Assert", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(z);

				((z || Bool(false)) == Bool(true)).Assert().Do();

				(Bool(true) == z).Evaluate().Do();
				(!(Bool(false) && z)).DoPartialEvaluate().Do();
				(Bool(true) && Bool(true)).DoPartialEvaluate().Do();


				SN_DECLARE(y);

				(Bool(true) == y.Debug()).Assert().Do();
				(Bool(true) == y).Evaluate().Do();

				SN_DECLARE(k1);
				SN_DECLARE_VALUE(l1, False);

				(k1.Debug().If(False, True) == l1).Assert().Do();
				(k1 == !l1).Evaluate().Do();

				SN_DECLARE(k2);
				SN_DECLARE_VALUE(l2, True);

				(k2.If(False, True) == l2).Assert().Do();
				(k2 == !l2).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestBoolAssertIf)
		{
			Initialize();
			{
				Manager manager("Test Bool Assert If", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(m);
				SN_DECLARE(n);
				SN_DECLARE(o);

				(o.If(False, True) == False).Assert().Do();
				o.Evaluate().Do();
				(o.If(m, n) == True).Assert().Do();
				m.Evaluate().Do();
				((!o).If(m, n) == m).Assert().Do();
				n.Evaluate().Do();
			}
			Cleanup();
		}
		TEST_METHOD(TestBoolNotAndNot)
		{
			Initialize();
			{
				Manager manager("Test Bool Not And Not", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				(!((Bool(true) && Bool(false)))).Assert().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestOr1)
		{
			Initialize();
			{
				Manager manager("Test Or 1", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(x);
				(x == Long(3)).Assert().Do();
				std::string x_string = x.GetVariableValue().DisplayValueSN();

				Assert::IsTrue(x_string == "Long(3)");
				(x == Long(3)).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestOr2)
		{
			Initialize();
			{
				Manager manager("Test Or 2", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(x);
				(x == Long(3) || x == Long(4)).Assert().Do();
				std::string x_string = x.DisplaySN();
				std::string x_valueset = x.DoEvaluate().DisplaySN();
				std::string x_buildset = x.BuildSet().DoEvaluate().DisplaySN();
				std::string t_valueset = (Long(3) || Long(4)).DoEvaluate().DisplaySN();
				std::string t_buildset = (Long(3) || Long(4)).BuildSet().DoEvaluate().DisplaySN();

				SN_DECLARE(y);
				(y == (Long(3) || Long(4))).Assert().Do();
				std::string y_string = y.DisplaySN();
				std::string y_valueset = y.DoEvaluate().DisplaySN();
				std::string y_buildset = y.BuildSet().DoEvaluate().DisplaySN();

				(x.BuildSet() == y.BuildSet()).Evaluate().Do();
				(x.BuildSet() == (Long(3) || Long(4)).BuildSet()).Evaluate().Do();
				std::cout << x.DisplaySN();
			}
			Cleanup();
		}

		TEST_METHOD(TestOr3)
		{
			Initialize();
			{
				Manager manager("Test Or 3", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(x);
				(x == Long(3) || x == Long(4) || x == Long(5)).Assert().Do();
				std::string x_string = x.DisplaySN();
				std::string x_valueset = x.DoEvaluate().DisplaySN();
				std::string x_buildset = x.BuildSet().DoEvaluate().DisplaySN();
				x.LogDisplaySN();
				x.DoEvaluate().LogDisplaySN();
				x.BuildSet().DoEvaluate().LogDisplaySN();

				SN_DECLARE(y);
				(y == (Long(3) || Long(4) || Long(5))).Assert().Do();
				std::string y_string = y.DisplaySN();
				std::string y_valueset = y.DoEvaluate().DisplaySN();
				std::string y_buildset = y.BuildSet().DoEvaluate().DisplaySN();

				(x.BuildSet() == y.BuildSet()).Evaluate().Do();
				(x.BuildSet() == (Long(3) || (Long(4) || Long(5))).BuildSet()).Evaluate().Do();
				std::cout << x.DisplaySN();
			}
			Cleanup();
		}

		TEST_METHOD(TestOr4)
		{
			Initialize();
			{
				Manager manager("Test Or 4", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(x);
				(x == Long(3) || x == Long(4) || x == Long(5) || x == Long(6)).Assert().Do();
				std::string x_string = x.DisplaySN();
				std::string x_valueset = x.DoEvaluate().DisplaySN();
				std::string x_buildset = x.BuildSet().DoEvaluate().DisplaySN();

				(x.BuildSet() == (Long(3) || Long(4) || Long(5) || Long(6)).BuildSet()).Evaluate().Do();
				std::cout << x.DisplaySN();
			}

			Cleanup();
		}

		TEST_METHOD(TestOr4WithCalc)
		{
			Initialize();
			{
				Manager manager("Test Or 4 With Calc", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(x);
				(x == Long(3) + Long(3) || x == Long(4) + Long(4) || x == Long(5) + Long(5) || x == Long(6) + Long(6)).Assert().Do();
				std::string x_string = x.DisplaySN();
				std::string x_valueset = x.DoEvaluate().DisplaySN();
				std::string x_buildset = x.BuildSet().DoEvaluate().DisplaySN();

				(x.BuildSet() == (Long(3) + Long(3) || Long(4) + Long(4) || Long(5) + Long(5) || Long(6) + Long(6)).BuildSet()).Evaluate().Do();
				std::cout << x.DisplaySN();
			}
			Cleanup();
		}

		TEST_METHOD(TestOrReversed4WithCalc)
		{
			Initialize();
			{
				Manager manager("Test Or Reversed 4 With Calc", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(x);
				(Long(3) + Long(3) == x || Long(4) + Long(4) == x || Long(5) + Long(5) == x || Long(6) + Long(6) == x).Assert().Do();
				std::string x_string = x.DisplaySN();
				std::string x_valueset = x.DoEvaluate().DisplaySN();
				std::string x_buildset = x.BuildSet().DoEvaluate().DisplaySN();

				(x.BuildSet() == (Long(3) + Long(3) || Long(4) + Long(4) || Long(5) + Long(5) || Long(6) + Long(6)).BuildSet()).Evaluate().Do();
				std::cout << x.DisplaySN();
			}
			Cleanup();
		}

		TEST_METHOD(TestOrReversed4WithSolve)
		{
			Initialize();
			{
				Manager manager("Test Or Reversed 4 With Solve", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(x);
				(Long(6) == x + Long(3) || Long(8) == x + Long(4) || Long(10) == x + Long(5) || Long(12) == x + Long(6)).Assert().Do();
				std::string x_string = x.DisplaySN();
				std::string x_valueset = x.DoEvaluate().DisplaySN();
				std::string x_buildset = x.BuildSet().DoEvaluate().DisplaySN();

				(x.BuildSet() == (Long(3) || Long(4) || Long(5) || Long(6)).BuildSet()).Evaluate().Do();
				std::cout << x.DisplaySN();
			}
			Cleanup();
		}
	};
}