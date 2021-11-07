#include "snl.h"

#include <cstdio>
#include <fstream>

#include "test_sn_pch.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace std;
using namespace skynet;
using namespace PGCX;

namespace test_sn
{
	TEST_CLASS(test_sn_imperative)
	{
	private:
		bool runWebServer = false;

		static void AssertErrorHandler(bool p_Err, const string& p_Description)
		{
			Assert::IsTrue(!p_Err, wstring(p_Description.begin(), p_Description.end()).c_str());
		}

		void Initialize()
		{
			Manager::LogicSetup();
			Transaction::ResetNetMemoryUsed();
			Transaction::ResetGrossMemoryUsed();

			Thread thread;
			if (thread.HasTopManager())
			{
				Assert::IsTrue(!thread.HasTopManager());
			}
		}

		void Cleanup()
		{
			Thread thread;
			if (thread.HasTopManager())
			{
				Assert::IsTrue(!thread.HasTopManager());
			}
			if (Transaction::TotalNetMemoryUsed() != 0)
			{
				Assert::IsTrue(Transaction::TotalNetMemoryUsed() == 0);
			}
			if (Promotion::PromotionUsedMemory() != 0)
			{
				Assert::IsTrue(Promotion::PromotionUsedMemory() == 0);
			}
			if (Promotion::PromotionFreeMemory() != Transaction::TotalGrossMemoryUsed())
			{
				Assert::IsTrue(Promotion::PromotionFreeMemory() == Transaction::TotalGrossMemoryUsed());
			}
		}

	public:
		TEST_METHOD(TestOverrideFunctionCallOperator)
		{
			Initialize();
			{
				Manager manager("Test Override Function Call Operator", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(testCall);
				{
					SN_LOCAL(f);
					SN_LOCAL(p);
					(testCall(f)(p) == f(p + Long(3))).PartialAssert().Do();
				}

				SN_DECLARE(x);

				(x == Local(Operators.FunctionCall, Let(Function(Function(Equals, Operators.FunctionCall), testCall), Long(4) + Long(5)))).Assert().Do();

				(x == Long(15)).Evaluate().Do();

				(Long(15) == Local(Operators.FunctionCall, Let(Function(Function(Equals, Operators.FunctionCall), testCall), Long(4) + Long(5)))).Evaluate().Do();
			}
		}

		TEST_METHOD(TestSimpleStateThreading)
		{
			return;
			Initialize();
			{
				Manager manager("Test Simple State Threading", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				manager.SetTailCallOptimization(true);

				string nv1;
				string nw1;

				SN_DECLARE(testSimpleImperative);
				{
					SN_LOCAL_INLINE(f);
					SN_LOCAL_INLINE(p);
					SN_LOCAL_INLINE(s);
					SN_LOCAL_INLINE(v);
					SN_LOCAL_INLINE(w);
					nv1 = v.DisplaySN();
					nw1 = w.DisplaySN();
					(testSimpleImperative(f)(p)(s) == Local(v, Local(w, Let(
						v == f.BuildMeta(Short(1)).IsA(Value::Class()).If(StateValue(f, s), f(s))
					&&	w == p.BuildMeta(Short(1)).IsA(Value::Class()).If(StateValue(p, v.State()), p(v.State()))
					,	StateValue(v.Value()(w.Value()), w.State()))))).PartialAssert().Do();
				}
				string s2 = testSimpleImperative.DisplayValueSN();

				SN_DECLARE(x);
				SN_DECLARE(t);
				(x == Local(Operators.FunctionCall, Let(Function(Function(Equals, Operators.FunctionCall), testSimpleImperative), Function(Long(4) + Long(5), t))).Value()).Assert().Do();

				(x == Long(9)).Evaluate().Do();

				(Long(9) == Local(Operators.FunctionCall, Let(Function(Function(Equals, Operators.FunctionCall), testSimpleImperative), Function(Long(4) + Long(5), t))).Value()).Evaluate().Do();
			}
		}

		TEST_METHOD(TestAssignment)
		{
			Initialize();
			{
				Manager manager("Test Assignment", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				{
					/*
										SN_DECLARE(Assignment);
										SN_DECLARE(l);
										SN_DECLARE(r);

										Expression x;
										x.Copy(l);

										Variable y(string("sheep"));
										Expression b((x = r));
										Expression c((y = r));
										Expression a((l = r).ImperativeTransform());
										(Define(Assignment)(l)(r) == ((l = r).ImperativeTransform())).PartialAssert().Do();
										string sAssignment = Assignment.DisplayValueSN();
									}
									SN_DECLARE(While);
									SN_DECLARE(c);
									SN_DECLARE(a);
									string s = (Define(While)(c)(a) == (Stateful(Mutable, c).If(Stateful(Mutable, a).Compose(Stateful(Mutable, While(c)(a))), False)).ImperativeTransform()).DisplaySN();

									(Define(While)(c)(a) == (Stateful(Mutable, c).If(Stateful(Mutable, a).Compose(Stateful(Mutable, While(c)(a))), True)).ImperativeTransform()).PartialAssert().Do();

									string sWhile = While.DisplayValueSN();

									SN_DECLARE(Fact);
									SN_DECLARE(m);
									SN_DECLARE(n);
									SN_DECLARE(j);

					*/
				}
			}
			Cleanup();
		}
	};
}