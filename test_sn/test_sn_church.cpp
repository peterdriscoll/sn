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
	TEST_CLASS(test_sn_church)
	{
	private:
		const string doc_root = "C:/Users/peter/Documents/Source/Repos/skynet2/html";

		bool runWebServer = false;

		static void AssertErrorHandler(SN::SN_Error p_Result)
		{
			string description = p_Result.GetDescription();
			Assert::IsTrue(p_Result.GetBool(), wstring(description.begin(), description.end()).c_str());
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
		TEST_METHOD(TestChurchSucc)
		{
			Initialize();
			{
				Manager manager("Test Church Succ", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(inc);
				SN_DECLARE(succ);
				SN_DECLARE(n);
				SN_DECLARE(f);
				SN_DECLARE(x);
				SN_DECLARE(v);
				SN_DECLARE(r0);
				SN_DECLARE(r1);
				SN_DECLARE(r2);
				SN_DECLARE(r3);
				(inc(v) == v + Long(1)).PartialAssert().Do();
				(inc(inc(inc(Long(0)))) == Long(3)).Assert().Do();

				// succ n f x = f (n f x)
				(Define(succ)(n)(f)(x) == f(n(f)(x))).PartialAssert().Do();

				(succ(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == Long(1)).Evaluate().Do();
				(succ(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == r0).Assert().Do();
				(r0 == Long(1)).Evaluate().Do();


				(succ(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == Long(2)).Evaluate().Do();
				(succ(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == r1).Assert().Do();
				(r1 == Long(2)).Evaluate().Do();

				(succ(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == Long(3)).Evaluate().Do();
				(succ(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == r2).Assert().Do();
				(r2 == Long(3)).Evaluate().Do();

				(succ(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == Long(4)).Evaluate().Do();
				(succ(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == r3).Assert().Do();
				(r3 == Long(4)).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestChurchPlus)
		{
			Initialize();
			{
				Manager manager("Test Church Plus", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(plus);
				SN_DECLARE(n);
				SN_DECLARE(m);
				SN_DECLARE(f);
				SN_DECLARE(inc);
				SN_DECLARE(x);
				SN_DECLARE(v);
				SN_DECLARE(r0);
				SN_DECLARE(r1);
				SN_DECLARE(r2);
				SN_DECLARE(r3);
				(inc(v) == v + Long(1)).PartialAssert().Do();
				(inc(inc(inc(Long(0)))) == Long(3)).Assert().Do();

				// plus m n f x = m f (n f x) 
				(Define(plus) == Lambda(m, Lambda(n, Lambda(f, Lambda(x, m(f)((n(f)(x)))))))).PartialAssert().Do();

				(plus(Lambda(f, Lambda(x, x)))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == Long(0)).Evaluate().Do();
				(plus(Lambda(f, Lambda(x, x)))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == r0).Assert().Do();
				(r0 == Long(0)).Evaluate().Do();

				(plus(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == Long(2)).Evaluate().Do();
				(plus(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == r1).Assert().Do();
				(r1 == Long(2)).Evaluate().Do();

				(plus(Lambda(f, Lambda(x, f(f(x)))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == Long(4)).Evaluate().Do();
				(plus(Lambda(f, Lambda(x, f(f(x)))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == r2).Assert().Do();
				(r2 == Long(4)).Evaluate().Do();

				(plus(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == Long(6)).Evaluate().Do();
				(plus(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == r3).Assert().Do();
				(r3 == Long(6)).Evaluate().Do();
			}
			Cleanup();
		}
		
		TEST_METHOD(TestChurchMultiply)
		{
			Initialize();
			{
				Manager manager("Test Church Multiply", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(mult);
				SN_DECLARE(n);
				SN_DECLARE(m);
				SN_DECLARE(f);
				SN_DECLARE(inc);
				SN_DECLARE(x);
				SN_DECLARE(v);
				SN_DECLARE(r0);
				SN_DECLARE(r1);
				SN_DECLARE(r2);
				SN_DECLARE(r3);
				(inc(v) == v + Long(1)).PartialAssert().Do();
				(inc(inc(inc(Long(0)))) == Long(3)).Assert().Do();

				// multiply m n f x = m (n f) x
				(Define(mult)(m)(n)(f)(x) == m(n(f))(x)).PartialAssert().Do();

				(mult(Lambda(f, Lambda(x, x)))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == Long(0)).Evaluate().Do();
				(mult(Lambda(f, Lambda(x, x)))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == r0).Assert().Do();
				(r0 == Long(0)).Evaluate().Do();

				(mult(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == Long(1)).Evaluate().Do();
				(mult(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == r1).Assert().Do();
				(r1 == Long(1)).Evaluate().Do();

				(mult(Lambda(f, Lambda(x, f(f(x)))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == Long(4)).Evaluate().Do();
				(mult(Lambda(f, Lambda(x, f(f(x)))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == r2).Assert().Do();
				(r2 == Long(4)).Evaluate().Do();

				(mult(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == Long(9)).Evaluate().Do();
				(mult(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == r3).Assert().Do();
				(r3 == Long(9)).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestChurchExp)
		{
			Initialize();
			{
				Manager manager("Test Church Exp", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(exp);
				SN_DECLARE(n);
				SN_DECLARE(m);
				SN_DECLARE(f);
				SN_DECLARE(inc);
				SN_DECLARE(x);
				SN_DECLARE(v);
				SN_DECLARE(r0);
				SN_DECLARE(r1);
				SN_DECLARE(r2);
				SN_DECLARE(r3);
				(inc(v) == v + Long(1)).PartialAssert().Do();
				(inc(inc(inc(Long(0)))) == Long(3)).Assert().Do();

				// exp m n f x = (n m) f x 
				(Define(exp)(m)(n)(f)(x) == m(n)(f)(x)).PartialAssert().Do();

				(exp(Lambda(f, Lambda(x, x)))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == Long(1)).Evaluate().Do();
				(exp(Lambda(f, Lambda(x, x)))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == r0).Assert().Do();
				(r0 == Long(1)).Evaluate().Do();


				(exp(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == Long(1)).Evaluate().Do();
				(exp(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == r1).Assert().Do();
				(r1 == Long(1)).Evaluate().Do();

				(exp(Lambda(f, Lambda(x, f(f(x)))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == Long(4)).Evaluate().Do();
				(exp(Lambda(f, Lambda(x, f(f(x)))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == r2).Assert().Do();
				(r2 == Long(4)).Evaluate().Do();

				(exp(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == Long(27)).Evaluate().Do();
				(exp(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == r3).Assert().Do();
				(r3 == Long(27)).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestChurchPred)
		{
			Initialize();
			{
				Manager manager("Test Church Pred", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(pred);
				SN_DECLARE(n);
				SN_DECLARE(m);
				SN_DECLARE(h);
				SN_DECLARE(g);
				SN_DECLARE(u);
				SN_DECLARE(f);
				SN_DECLARE(inc);
				SN_DECLARE(x);
				SN_DECLARE(v);
				SN_DECLARE(r0);
				SN_DECLARE(r1);
				SN_DECLARE(r2);
				SN_DECLARE(r3);
				(inc(v) == v + Long(1)).PartialAssert().Do();
				(inc(inc(inc(Long(0)))) == Long(3)).Assert().Do();

				// \n.\f.\x.n (\g.\h.h (g f)) (\u.x) (\u.u)
				(Define(pred) == Lambda(n, Lambda(f, Lambda(x, n(Lambda(g, Lambda(h, h(g(f))))) (Lambda(u, x))(Lambda(u, u)))))).PartialAssert().Do();

				(pred(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == Long(0)).Evaluate().Do();
				(pred(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == r0).Assert().Do();
				(r0 == Long(0)).Evaluate().Do();

				(pred(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == Long(0)).Evaluate().Do();
				(pred(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == r1).Assert().Do();
				(r1 == Long(0)).Evaluate().Do();

				(pred(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == Long(1)).Evaluate().Do();
				(pred(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == r2).Assert().Do();
				(r2 == Long(1)).Evaluate().Do();

				(pred(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == Long(2)).Evaluate().Do();
				(pred(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == r3).Assert().Do();
				(r3 == Long(2)).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestChurchMinus)
		{
			Initialize();
			{
				Manager manager("Test Church Minus", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(pred);
				SN_DECLARE(h);
				SN_DECLARE(g);
				SN_DECLARE(u);
				SN_DECLARE(minus);
				SN_DECLARE(n);
				SN_DECLARE(m);
				SN_DECLARE(f);
				SN_DECLARE(inc);
				SN_DECLARE(x);
				SN_DECLARE(v);
				SN_DECLARE(r0);
				SN_DECLARE(r1);
				SN_DECLARE(r2);
				SN_DECLARE(r3);
				(inc(v) == v + Long(1)).PartialAssert().Do();
				(inc(inc(inc(Long(0)))) == Long(3)).Assert().Do();

				// \n.\f.\x.n (\g.\h.h (g f)) (\u.x) (\u.u)
				(Define(pred) == Lambda(n, Lambda(f, Lambda(x, n(Lambda(g, Lambda(h, h(g(f))))) (Lambda(u, x))(Lambda(u, u)))))).PartialAssert().Do();

				// minus m n = (n pred) m	
				(Define(minus)(m)(n) == n(pred)(m)).PartialAssert().Do();

				// This works too.
				// (Define(minus)(m)(n)(f)(x) == n(pred)(m)(f)(x)).PartialAssert().Do();

				(minus(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == Long(1)).Evaluate().Do();
				(minus(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, x)))(inc)(Long(0)) == r0).Assert().Do();
				(r0 == Long(1)).Evaluate().Do();

				(minus(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == Long(2)).Evaluate().Do();
				(minus(Lambda(f, Lambda(x, f(f(f(x))))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == r1).Assert().Do();
				(r1 == Long(2)).Evaluate().Do();

				(minus(Lambda(f, Lambda(x, f(f(f(f(f(x))))))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == Long(3)).Evaluate().Do();
				(minus(Lambda(f, Lambda(x, f(f(f(f(f(x))))))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == r2).Assert().Do();
				(r2 == Long(3)).Evaluate().Do();

				(minus(Lambda(f, Lambda(x, f(f(f(f(f(f(f(f(x)))))))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == Long(5)).Evaluate().Do();
				(minus(Lambda(f, Lambda(x, f(f(f(f(f(f(f(f(x)))))))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == r3).Assert().Do();
				(r3 == Long(5)).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestChurchDivide)
		{
			Initialize();
			{
				Manager manager("Test Church Divide", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				if (manager.GetEvaluationType() == skynet::Strict)
				{
					return;
				}

				SN_DECLARE(divide);

				SN_DECLARE(div);
				SN_DECLARE(divide1);
				SN_DECLARE(succ);
				SN_DECLARE(Y);
				SN_DECLARE(zero);
				SN_DECLARE(IsZero);
				SN_DECLARE(falseL);
				SN_DECLARE(trueL);
				SN_DECLARE(minus);
				SN_DECLARE(pred);

				SN_DECLARE(inc);
				SN_DECLARE(f);
				SN_DECLARE(x);
				SN_DECLARE(n);
				SN_DECLARE(m);
				SN_DECLARE(a);
				SN_DECLARE(b);
				SN_DECLARE(c);
				SN_DECLARE(d);
				SN_DECLARE(u);
				SN_DECLARE(v);
				SN_DECLARE(g);
				SN_DECLARE(h);

				SN_DECLARE(r1);
				SN_DECLARE(r2);
				SN_DECLARE(r3);

				(inc(v) == v + Long(1)).PartialAssert().Do();
				(inc(inc(inc(Long(0)))) == Long(3)).Assert().Do();

				// Y = \f.(\x.x x) (\x.f (x x))
				(Define(Y) == Lambda(f, Lambda(x, f(x(x)))(Lambda(x, f(x(x)))))).PartialAssert().Do();

				// \n.\f.\x.n (\g.\h.h (g f)) (\u.x) (\u.u)
				(Define(pred) == Lambda(n, Lambda(f, Lambda(x, n(Lambda(g, Lambda(h, h(g(f))))) (Lambda(u, x))(Lambda(u, u)))))).PartialAssert().Do();

				// minus m n = (n pred) m	
				(Define(minus)(m)(n) == n(pred)(m)).PartialAssert().Do();

				// div = \c.\n.\m.\f.\x.(\d.IsZero d (0 f x) (f (c d m f x))) (minus n m)
				(Define(div) == Lambda(c, Lambda(n, Lambda(m, Lambda(f, Lambda(x, (Lambda(d, IsZero(d)(zero(f)(x))(f(c(d)(m)(f)(x))))(minus(n)(m))))))))).PartialAssert().Do();

				// divide1 == Y div
				(Define(divide1) == Y(div)).PartialAssert().Do();

				// divide = \n.divide1 (succ n)
				(Define(divide) == Lambda(n, divide1(succ(n)))).PartialAssert().Do();

				// succ n f x = f (n f x)
				(Define(succ)(n)(f)(x) == f(n(f)(x))).PartialAssert().Do();

				// zero = \f.\x.x
				(Define(zero) == Lambda(f, Lambda(x, x))).PartialAssert().Do();

				// trueL = \a.\b.a
				(Define(trueL) == Lambda(a, Lambda(b, a))).PartialAssert().Do();

				// falseL = \a.\b.a
				(Define(falseL) == Lambda(a, Lambda(b, b))).PartialAssert().Do();

				// IsZero = \n.n (\x.false) true
				(Define(IsZero) == Lambda(n, n(Lambda(x, falseL))(trueL))).PartialAssert().Do();

				(divide(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == Long(1)).Evaluate().Do();
				(divide(Lambda(f, Lambda(x, f(x))))(Lambda(f, Lambda(x, f(x))))(inc)(Long(0)) == r1).Assert().Do();
				(r1 == Long(1)).Evaluate().Do();

				(divide(Lambda(f, Lambda(x, f(f(f(f(x)))))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == Long(2)).Evaluate().Do();
				(divide(Lambda(f, Lambda(x, f(f(f(f(x)))))))(Lambda(f, Lambda(x, f(f(x)))))(inc)(Long(0)) == r2).Assert().Do();
				(r2 == Long(2)).Evaluate().Do();

				(divide(Lambda(f, Lambda(x, f(f(f(f(f(f(f(f(f(x))))))))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == Long(3)).Evaluate().Do();
				(divide(Lambda(f, Lambda(x, f(f(f(f(f(f(f(f(f(x))))))))))))(Lambda(f, Lambda(x, f(f(f(x))))))(inc)(Long(0)) == r3).Assert().Do();
				(r3 == Long(3)).Evaluate().Do();
			}
			Cleanup();
		}
	};
}