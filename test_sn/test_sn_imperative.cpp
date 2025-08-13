#include "snl.h"

#include <cstdio>
#include <fstream>

#include "test_sn_pch.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


using namespace skynet;
using namespace PGCX;

namespace test_sn
{
	TEST_CLASS(test_sn_imperative)
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

				(x == Local(User::GetOperators().FunctionCall, Let(Function(Function(Equals, User::GetOperators().FunctionCall), testCall), Long(4) + Long(5)))).Assert().Do();

				(x == Long(15)).Evaluate().Do();

				(Long(15) == Local(User::GetOperators().FunctionCall, Let(Function(Function(Equals, User::GetOperators().FunctionCall), testCall), Long(4) + Long(5)))).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestSimpleStateThreading)
		{
			Initialize();
			{
				Manager manager("Test Simple State Threading", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				manager.SetTailCallOptimization(true);

				std::string nv1;
				std::string nw1;

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
				std::string s2 = testSimpleImperative.DisplayValueSN();

				SN_DECLARE(x);
				SN_DECLARE(t);
				(x == Local(User::GetOperators().FunctionCall, Let(Function(Function(Equals, User::GetOperators().FunctionCall), testSimpleImperative), Function(Long(4) + Long(5), t))).Value()).Assert().Do();

				(x == Long(9)).Evaluate().Do();

				(Long(9) == Local(User::GetOperators().FunctionCall, Let(Function(Function(Equals, User::GetOperators().FunctionCall), testSimpleImperative), Function(Long(4) + Long(5), t))).Value()).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestSimpleStateThreading2)
		{
			Initialize();
			{
				Manager manager("Test Simple State Threading 2", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				std::string s2 = User::GetOperators().ImperativeCall.DisplayValueSN();
				SN_DECLARE(y);
				SN_DECLARE(t);
				(y == Local(User::GetOperators().FunctionCall, Let(Function(Function(Equals, User::GetOperators().FunctionCall), User::GetOperators().ImperativeCall), Function(Long(4) + Long(5), t))).Value()).Assert().Do();
				std::string y1 = y.DisplayValueSN();

				(y == Long(9)).Evaluate().Do();

				(Long(9) == Local(User::GetOperators().FunctionCall, Let(Function(Function(Equals, User::GetOperators().FunctionCall), User::GetOperators().ImperativeCall), Function(Long(4) + Long(5), t))).Value()).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestSimpleAssignment)
		{
			Initialize();
			{
				Manager manager("Test Simple Assignment", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				std::string s2 = User::GetOperators().ImperativeCall.DisplayValueSN();
				SN_DECLARE(y);
				SN_DECLARE(t);
				SN_DECLARE(l);
				manager.Breakpoint();
				(y == Local(User::GetOperators().FunctionCall, Local(User::GetOperators().Assign, Let(Function(Function(Equals, User::GetOperators().FunctionCall), User::GetOperators().ImperativeCall), Let(Function(Function(Equals, User::GetOperators().Assign), User::GetOperators().ImperativeAssign), Function(String("m").Assign(Long(4)) + Long(5), State()))))).Value()).Assert().Do();
				std::string y1 = y.DisplayValueSN();

				(y == Long(9)).Evaluate().Do();

				SN_DECLARE(m);
				(Long(9) == Local(User::GetOperators().FunctionCall, Local(User::GetOperators().Assign, Let(Function(Function(Equals, User::GetOperators().FunctionCall), User::GetOperators().ImperativeCall), Let(Function(Function(Equals, User::GetOperators().Assign), User::GetOperators().ImperativeAssign), Function(String("m").Assign(Long(4)) + Long(5), State()))))).Value()).Evaluate().Do();
			}
			Cleanup();
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

										Variable y(String("sheep"));
										Expression b((x = r));
										Expression c((y = r));
										Expression a((l = r).ImperativeTransform());
										(Define(Assignment)(l)(r) == ((l = r).ImperativeTransform())).PartialAssert().Do();
										std::string sAssignment = Assignment.DisplayValueSN();
									}
									SN_DECLARE(While);
									SN_DECLARE(c);
									SN_DECLARE(a);
									std::string s = (Define(While)(c)(a) == (Stateful(Mutable, c).If(Stateful(Mutable, a).Compose(Stateful(Mutable, While(c)(a))), False)).ImperativeTransform()).DisplaySN();

									(Define(While)(c)(a) == (Stateful(Mutable, c).If(Stateful(Mutable, a).Compose(Stateful(Mutable, While(c)(a))), True)).ImperativeTransform()).PartialAssert().Do();

									std::string sWhile = While.DisplayValueSN();

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