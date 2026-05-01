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
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", port, doc_root, runWebServer);
				manager.SetApplyOverride(true);

				SN_DECLARE(testCall);
				{
					SN_LOCAL(f);
					SN_LOCAL(p);
					(testCall(f)(p) == f(p + Long(3))).PartialAssert().Do();
				}

				SN_DECLARE(x);

				(x == Local(User::GetOperators().FunctionCall, Let(Function(Function(Equals, User::GetOperators().FunctionCall), testCall), Long(4) + Long(5)))).Assert().Do();

				(x == Long(15)).Evaluate().Do().CheckValue();

				(Long(15) == Local(User::GetOperators().FunctionCall, Let(Function(Function(Equals, User::GetOperators().FunctionCall), testCall), Long(4) + Long(5)))).Evaluate().Do().CheckValue();
			}
			Cleanup();
		}

		TEST_METHOD(TestSimpleStateThreading)
		{
			Initialize();
			{
				Manager manager("Test Simple State Threading", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", port, doc_root, runWebServer);
				manager.SetTailCallOptimization(true);
				manager.SetApplyOverride(true);

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
						v == f.BuildMeta(Short(1)).IsA(MetaType(1, Value::Class())).If(StateValue(f, s), f(s))
					&&	w == p.BuildMeta(Short(1)).IsA(MetaType(1, Value::Class())).If(StateValue(p, v.State()), p(v.State()))
					,	StateValue(v.Value()(w.Value()), w.State()))))).PartialAssert().Do();
				}
				std::string s2 = testSimpleImperative.DisplayValueSN();
				std::string s2_expected = "testSimpleImperative:=Lambda(@f.@p.@s.#v.#w.(let v=if f.Meta(1) is a {Value} then StateValue(f, s) else f s &w=if p.Meta(1) is a {Value} then StateValue(p, v.State()) else p v.State()  in StateValue(v.Value() w.Value(), w.State())))";
			    Assert::IsTrue(s2 == s2_expected, L"Expected value for testSimpleImperative not found.");
				
				SN_DECLARE(x);
				SN_DECLARE(t);
				(x == Local(User::GetOperators().FunctionCall, Let(Function(Function(Equals, User::GetOperators().FunctionCall), testSimpleImperative), Function(Long(4) + Long(5), t))).Value()).Assert().Do();

				(x == Long(9)).Evaluate().Do().CheckValue();

				(Long(9) == Local(User::GetOperators().FunctionCall, Let(Function(Function(Equals, User::GetOperators().FunctionCall), testSimpleImperative), Function(Long(4) + Long(5), t))).Value()).Evaluate().Do().CheckValue();
			}
			Cleanup();
		}

		TEST_METHOD(TestBuildMetaTypeInheritance)
		{
			Initialize();
			{
				Manager manager("Test Meta Inheritance", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", port, doc_root, runWebServer);

				SN_DECLARE_VALUE(v, Long(5));
                Meta(1, v).IsA(Meta::Class()).Assert().Do();
                Meta(1, v).IsA(MetaType(1, Variable::ExprClass())).Assert().Do();

                v.BuildMeta(Short(1)).IsA(Meta::Class()).Assert().Do();
                v.BuildMeta(Short(1)).IsA(MetaType(1, Long::Class())).Assert().Do();

				SN_DECLARE(n);
				n.SetValue(v);
                n.BuildMeta(Short(1)).IsA(Meta::Class()).Assert().Do();
				n.BuildMeta(Short(1)).IsA(MetaType(1, Variable::ExprClass())).Assert().Do();

				SN_DECLARE(l);
				l.SetValue(Let(skynet::True, skynet::True));
                l.BuildMeta(Short(1)).IsA(Meta::Class()).Assert().Do();
                l.BuildMeta(Short(1)).IsA(MetaType(1, Let::ExprClass())).Assert().Do();

				SN_DECLARE(o);
				o.SetValue(Local(v, skynet::True));
                o.BuildMeta(Short(1)).IsA(Meta::Class()).Assert().Do();
                o.BuildMeta(Short(1)).IsA(MetaType(1, Local::ExprClass())).Assert().Do();

				SN_DECLARE(p);
				p.SetValue(Define(v));
                p.BuildMeta(Short(1)).IsA(Meta::Class()).Assert().Do();
                p.BuildMeta(Short(1)).IsA(MetaType(1, Define::ExprClass())).Assert().Do();

				SN_DECLARE(f);
				SN_DECLARE(q);
				q.SetValue(Function(f, Long(5)));
                q.BuildMeta(Short(1)).IsA(Meta::Class()).Assert().Do();
                q.BuildMeta(Short(1)).IsA(MetaType(1, Function::ExprClass())).Assert().Do();

				SN_DECLARE(x);
				SN_DECLARE_VALUE(r, Lambda(x, Long(5)));
                r.BuildMeta(Short(1)).IsA(Meta::Class()).Assert().Do();
                r.BuildMeta(Short(1)).IsA(MetaType(1, Lambda::ExprClass())).Assert().Do();
			}
			Cleanup();
		}
		
		TEST_METHOD(TestSimpleStateThreading2)
		{
			Initialize();
			{
				Manager manager("Test Simple State Threading 2", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", port, doc_root, runWebServer);
				manager.SetApplyOverride(true);

				std::string s2 = User::GetOperators().ImperativeCall.DisplayValueSN();
				SN_DECLARE(y);
				SN_DECLARE(t);
				(y == Local(User::GetOperators().FunctionCall, Let(Function(Function(Equals, User::GetOperators().FunctionCall), User::GetOperators().ImperativeCall), Function(Long(4) + Long(5), t))).Value()).Assert().Do();
				std::string y1 = y.DisplayValueSN();

				(y == Long(9)).Evaluate().Do().CheckValue();

				(Long(9) == Local(User::GetOperators().FunctionCall, Let(Function(Function(Equals, User::GetOperators().FunctionCall), User::GetOperators().ImperativeCall), Function(Long(4) + Long(5), t))).Value()).Evaluate().Do().CheckValue();
			}
			Cleanup();
		}

		TEST_METHOD(TestSimpleAssignment)
		{
			Initialize();
			{
				Manager manager("Test Simple Assignment", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", port, doc_root, runWebServer);
				manager.SetApplyOverride(true);

				std::string s2 = User::GetOperators().ImperativeCall.DisplayValueSN();
				SN_DECLARE(y);
				SN_DECLARE(t);
				SN_DECLARE(l);

				(y == Local(User::GetOperators().FunctionCall, Local(User::GetOperators().Assign, Let(Function(Function(Equals, User::GetOperators().FunctionCall), User::GetOperators().ImperativeCall), Let(Function(Function(Equals, User::GetOperators().Assign), User::GetOperators().ImperativeAssign), Function(String("m").Assign(Long(4)) + Long(5), State()))))).Value()).Assert().Do();
				std::string y1 = y.DisplayValueSN();

				(y == Long(9)).Evaluate().Do().CheckValue();

				SN_DECLARE(m);
				(Long(9) == Local(User::GetOperators().FunctionCall, Local(User::GetOperators().Assign, Let(Function(Function(Equals, User::GetOperators().FunctionCall), User::GetOperators().ImperativeCall), Let(Function(Function(Equals, User::GetOperators().Assign), User::GetOperators().ImperativeAssign), Function(String("m").Assign(Long(4)) + Long(5), State()))))).Value()).Evaluate().Do().CheckValue();
			}
			Cleanup();
		}

		TEST_METHOD(TestAssignment)
		{
			Initialize();
			{
				Manager manager("Test Assignment", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", port, doc_root, runWebServer);
				manager.SetApplyOverride(true);

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