#include "snl.h"

#include <cstdio>
#include <fstream>

#include "test_sn_pch.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


using namespace skynet;
using namespace PGCX;

namespace test_sn
{
	TEST_CLASS(test_sn_inherit)
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
		TEST_METHOD(TestInheritInstance)
		{
			Initialize();
			{
				Manager manager("Test Simple Inherit1", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				skynet::True.IsA(Bool::Class()).Assert().Do();
				String("woof").IsA(String::Class()).Assert().Do();
				Char('X').IsA(Char::Class()).Assert().Do();
				(String("DogFood") || String("CatFood")).BuildSet().IsA(Set::Class()).Assert().Do();

				Meta(1, String("Vincent ") + String("Emma ")).IsA(Function::Class()).Assert().Do();

				Short(5).IsA(Short::Class()).Assert().Do();
				Int(5).IsA(Int::Class()).Assert().Do();
				Long(5).IsA(Long::Class()).Assert().Do();
				LongLong(5).IsA(LongLong::Class()).Assert().Do();
				Float(5).IsA(Float::Class()).Assert().Do();
				Double(5).IsA(Double::Class()).Assert().Do();
				LongDouble(5).IsA(LongDouble::Class()).Assert().Do();
				Mapping().IsA(Mapping::Class()).Assert().Do();

				SN_DECLARE(a);
				SN_DECLARE(b);
				(String("dogcat") == a + b).Assert().Do();

				a.IsA(StringRef::Class()).Assert().Do();

				skynet::True.Assert().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestInheritShort)
		{
			Initialize();
			{
				Manager manager("Test Inherit Short", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				Short::Class().IsA(Long::Class()).Evaluate().Do();
				SN_DECLARE(x1);
				(Short::Class().IsA(Long::Class()) == x1).Assert().Do();
				x1.Evaluate().Do();
				bool b1 = x1.GetBool();
				Assert::IsTrue(b1);

				Short::Class().IsA(LongLong::Class()).Evaluate().Do();
				SN_DECLARE(x2);
				(Short::Class().IsA(LongLong::Class()) == x2).Assert().Do();
				x2.Evaluate().Do();
				bool b2 = x2.GetBool();
				Assert::IsTrue(b2);

				Short::Class().IsA(Float::Class()).Evaluate().Do();
				SN_DECLARE(x3);
				(Short::Class().IsA(Float::Class()) == x3).Assert().Do();
				x3.Evaluate().Do();
				bool b3 = x3.GetBool();
				Assert::IsTrue(b3);

				Short::Class().IsA(Double::Class()).Evaluate().Do();
				SN_DECLARE(x4);
				(Short::Class().IsA(Double::Class()) == x4).Assert().Do();
				x4.Evaluate().Do();
				bool b4 = x4.GetBool();
				Assert::IsTrue(b4);

				Short::Class().IsA(LongDouble::Class()).Evaluate().Do();
				SN_DECLARE(x5);
				(Short::Class().IsA(LongDouble::Class()) == x5).Assert().Do();
				x5.Evaluate().Do();
				bool b5 = x5.GetBool();
				Assert::IsTrue(b5);

			}
			Cleanup();
		}

		TEST_METHOD(TestInheritLong)
		{
			Initialize();
			{
				Manager manager("Test Inherit Long", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				Long::Class().IsA(LongLong::Class()).Evaluate().Do();
				SN_DECLARE(x1);
				(Long::Class().IsA(LongLong::Class()) == x1).Assert().Do();
				x1.Evaluate().Do();
				bool b1 = x1.GetBool();
				Assert::IsTrue(b1);

				Long::Class().IsA(Double::Class()).Evaluate().Do();
				SN_DECLARE(x2);
				(Long::Class().IsA(Double::Class()) == x2).Assert().Do();
				x2.Evaluate().Do();
				bool b2 = x2.GetBool();
				Assert::IsTrue(b2);

				Long::Class().IsA(LongDouble::Class()).Evaluate().Do();
				SN_DECLARE(x3);
				(Long::Class().IsA(LongDouble::Class()) == x3).Assert().Do();
				x3.Evaluate().Do();
				bool b3 = x3.GetBool();
				Assert::IsTrue(b3);

			}
			Cleanup();
		}

		TEST_METHOD(TestInheritLongLong)
		{
			Initialize();
			{
				Manager manager("Test Inherit LongLong", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				LongLong::Class().IsA(LongDouble::Class()).Evaluate().Do();
				SN_DECLARE(x1);
				(LongLong::Class().IsA(LongDouble::Class()) == x1).Assert().Do();
				x1.Evaluate().Do();
				bool b1 = x1.GetBool();
				Assert::IsTrue(b1);

			}
			Cleanup();
		}

		TEST_METHOD(TestInheritFloat)
		{
			Initialize();
			{
				Manager manager("Test Inherit Float", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				Float::Class().IsA(Double::Class()).Evaluate().Do();
				SN_DECLARE(x1);
				(Float::Class().IsA(Double::Class()) == x1).Assert().Do();
				x1.Evaluate().Do();
				bool b1 = x1.GetBool();
				Assert::IsTrue(b1);

				Float::Class().IsA(LongDouble::Class()).Evaluate().Do();
				SN_DECLARE(x2);
				(Float::Class().IsA(LongDouble::Class()) == x2).Assert().Do();
				x2.Evaluate().Do();
				bool b2 = x2.GetBool();
				Assert::IsTrue(b2);

			}
			Cleanup();
		}

		TEST_METHOD(TestInheritDouble)
		{
			Initialize();
			{
				Manager manager("Test Inherit Double", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				Double::Class().IsA(LongDouble::Class()).Evaluate().Do();
				SN_DECLARE(x1);
				(Double::Class().IsA(LongDouble::Class()) == x1).Assert().Do();
				x1.Evaluate().Do();
				bool b1 = x1.GetBool();
				Assert::IsTrue(b1);

			}
			Cleanup();
		}

		TEST_METHOD(TestInheritChar)
		{
			Initialize();
			{
				Manager manager("Test Inherit Char", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				Char::Class().IsA(String::Class()).Evaluate().Do();
				SN_DECLARE(x1);
				(Char::Class().IsA(String::Class()) == x1).Assert().Do();
				x1.Evaluate().Do();
				bool b1 = x1.GetBool();
				Assert::IsTrue(b1);

			}
			Cleanup();
		}

		TEST_METHOD(TestInheritStringRef)
		{
			Initialize();
			{
				Manager manager("Test Inherit Char", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				StringRef::Class().IsA(String::Class()).Evaluate().Do();
				SN_DECLARE(x1);
				(StringRef::Class().IsA(String::Class()) == x1).Assert().Do();
				x1.Evaluate().Do();
				bool b1 = x1.GetBool();
				Assert::IsTrue(b1);

			}
			Cleanup();
		}

		TEST_METHOD(TestDerived)
		{
			Initialize();
			{
				Manager manager("Test Derived", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE_VALUE(fib, Derived());
				SN_DECLARE(N);
				(fib(Long(0)) == Long(1)).PartialAssert().Do();
				(fib(Long(1)) == Long(1)).PartialAssert().Do();
				(Define(fib)(N) == (N >Long(1)).If(fib(N - Long(1)) + fib(N - Long(2)))).PartialAssert().Do();
				
				fib.Fix();

				(fib(Long(0)) == Long(1)).Evaluate().Do();
				(fib(Long(1)) == Long(1)).Evaluate().Do();
				(fib(Long(2)) == Long(2)).Evaluate().Do();

				(fib(Long(3)) == Long(3)).Evaluate().Do();
				(fib(Long(4)) == Long(5)).Evaluate().Do();
				(fib(Long(5)) == Long(8)).Evaluate().Do();
				(fib(Long(6)) == Long(13)).Evaluate().Do();

				(fib(Long(0)) == Long(1)).Assert().Do();
				(fib(Long(1)) == Long(1)).Assert().Do();
				(fib(Long(2)) == Long(2)).Assert().Do();
				(fib(Long(3)) == Long(3)).Assert().Do();
				(fib(Long(4)) == Long(5)).Assert().Do();
				(fib(Long(5)) == Long(8)).Assert().Do();
				(fib(Long(6)) == Long(13)).Assert().Do();

				manager.DelayedCalls().Assert().Do();
				manager.DelayedCalls().Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestVirtual)
		{
			Initialize();
			{
				Manager manager("Test Virtual", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE_VALUE(fib, Virtual());
				SN_DECLARE(N);
				(fib(Long(0)) == Long(1)).PartialAssert().Do();
				(fib(Long(1)) == Long(1)).PartialAssert().Do();
				(Define(fib)(N) == fib(N - Long(1)) + fib(N - Long(2))).PartialAssert().Do();

				fib.Fix();

				(fib(Long(0)) == Long(1)).Evaluate().Do();
				(fib(Long(1)) == Long(1)).Evaluate().Do();
				(fib(Long(2)) == Long(2)).Evaluate().Do();
				(fib(Long(3)) == Long(3)).Evaluate().Do();
				(fib(Long(4)) == Long(5)).Evaluate().Do();
				(fib(Long(5)) == Long(8)).Evaluate().Do();
				(fib(Long(6)) == Long(13)).Evaluate().Do();

				(fib(Long(0)) == Long(1)).Assert().Do();
				(fib(Long(1)) == Long(1)).Assert().Do();
				(fib(Long(2)) == Long(2)).Assert().Do();
				(fib(Long(3)) == Long(3)).Assert().Do();
				(fib(Long(4)) == Long(5)).Assert().Do();
				(fib(Long(5)) == Long(8)).Assert().Do();
				(fib(Long(6)) == Long(13)).Assert().Do();

			}
			Cleanup();
		}

		TEST_METHOD(TestVirtualPolymorphic1)
		{
			Initialize();
			{
				Manager manager("Test Virtual Polymorphic2", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE_VALUE(typeChecker, Virtual());
				SN_DECLARE_VALUE(shortType, Short::Class());

				(typeChecker(shortType) == String("short")).PartialAssert().Do();

				std::string sTypeChecker_before = typeChecker.DisplayValueSN();
				typeChecker.Fix();
				std::string sTypeChecker_after = typeChecker.DisplayValueSN();

				// Polymorphic call.
				(typeChecker(Short(1)) == String("short")).Evaluate().Do();

				SN_DECLARE(A);

				(typeChecker(Short(1)) == A).Assert().Do();

				std::string A_text = A.GetString();

				Assert::IsTrue(A_text == "short");
			}
			Cleanup();
		}

		TEST_METHOD(TestVirtualPolymorphic2)
		{
			Initialize();
			{
				Manager manager("Test Virtual Polymorphic 2", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE_VALUE(typeChecker, Virtual());
				SN_DECLARE_VALUE(shortType, Short::Class());
				SN_DECLARE_VALUE(longType, Long::Class());

				(typeChecker(shortType) == String("short")).PartialAssert().Do();
				(typeChecker(longType) == String("long")).PartialAssert().Do();

				std::string sTypeChecker_before = typeChecker.DisplayValueSN();
				typeChecker.Fix();
				std::string sTypeChecker_after = typeChecker.DisplayValueSN();

				// Polymorphic call.
				(typeChecker(Short(1)) == String("short")).Evaluate().Do();
				(typeChecker(Long(1)) == String("long")).Evaluate().Do();

				(typeChecker(Short(1)) == String("short")).Assert().Do();
				(typeChecker(Long(1)) == String("long")).Assert().Do();

				SN_DECLARE(A);
				SN_DECLARE(B);

				(typeChecker(Short(1)) == A).Assert().Do();
				(typeChecker(Long(1)) == B).Assert().Do();

				std::string A_text = A.GetString();
				std::string B_text = B.GetString();

				Assert::IsTrue(A_text == "short");
				Assert::IsTrue(B_text == "long");
			}
			Cleanup();
		}

		TEST_METHOD(TestVirtualPolymorphic3)
		{
			Initialize();
			{
				Manager manager("Test Virtual Polymorphic3", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE_VALUE(typeChecker, Virtual());
				SN_DECLARE_VALUE(shortType, Short::Class());
				SN_DECLARE_VALUE(longType, Long::Class());
				SN_DECLARE_VALUE(longLongType, LongLong::Class());

				(typeChecker(shortType) == String("short")).PartialAssert().Do();
				(typeChecker(longType) == String("long")).PartialAssert().Do();
				(typeChecker(longLongType) == String("long long")).PartialAssert().Do();

				std::string sTypeChecker_before = typeChecker.DisplayValueSN();
				typeChecker.Fix();
				std::string sTypeChecker_after = typeChecker.DisplayValueSN();

				// Polymorphic call.
				(typeChecker(Short(1)) == String("short")).Evaluate().Do();
				(typeChecker(Long(1)) == String("long")).Evaluate().Do();
				(typeChecker(LongLong(1)) == String("long long")).Evaluate().Do();

				(typeChecker(Short(1)) == String("short")).Assert().Do();
				(typeChecker(Long(1)) == String("long")).Assert().Do();
				(typeChecker(LongLong(1)) == String("long long")).Assert().Do();

				SN_DECLARE(A);
				SN_DECLARE(B);
				SN_DECLARE(C);

				(typeChecker(Short(1)) == A).Assert().Do();
				(typeChecker(Long(1)) == B).Assert().Do();
				(typeChecker(LongLong(1)) == C).Assert().Do();

				std::string A_text = A.GetString();
				std::string B_text = B.GetString();
				std::string C_text = C.GetString();

				Assert::IsTrue(A_text == "short");
				Assert::IsTrue(B_text == "long");
				Assert::IsTrue(C_text == "long long");
			}
			Cleanup();
		}

		TEST_METHOD(TestVirtualPolymorphicReverse1)
		{
			Initialize();
			{
				Manager manager("Test Virtual Polymorphic Reverse 1", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE_VALUE(typeChecker, Virtual());
				SN_DECLARE_VALUE(shortType, Short::Class());

				(typeChecker(shortType) == String("short")).PartialAssert().Do();

				std::string sTypeChecker_before = typeChecker.DisplayValueSN();
				typeChecker.Fix();
				std::string sTypeChecker_after = typeChecker.DisplayValueSN();

				// Reverse Polymorphic call.
				SN_DECLARE_VALUE(numbers1, Short(1) || Long(2));
				std::string n1_text_before = numbers1.DoEvaluate().DisplayValueSN();

				(typeChecker(numbers1) == String("short")).Assert().Do();
				std::string n1_text = numbers1.DoEvaluate().DisplayValueSN();
				Assert::IsTrue(n1_text == "Short(1)");
				(numbers1 == Short(1)).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestVirtualPolymorphicReverse2)
		{
			Initialize();
			{
				Manager manager("Test Virtual Polymorphic Reverse 2", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE_VALUE(typeChecker, Virtual());
				SN_DECLARE_VALUE(shortType, Short::Class());
				SN_DECLARE_VALUE(longType, Long::Class());

				(typeChecker(shortType) == String("short")).PartialAssert().Do();
				(typeChecker(longType) == String("long")).PartialAssert().Do();

				std::string sTypeChecker_before = typeChecker.DisplayValueSN();
				typeChecker.Fix();
				std::string sTypeChecker_after = typeChecker.DisplayValueSN();

				// Reverse Polymorphic call.
				//SN_DECLARE_VALUE(numbers1, Short(1) || Long(2));
				SN_DECLARE(numbers1);
				(numbers1 == (Short(1) || Long(2))).PartialAssert().Do();

				std::string n1_text_before = numbers1.DoEvaluate().DisplayValueSN();
				(typeChecker(numbers1) == String("short")).Assert().Do();
				std::string n1_text = numbers1.DoEvaluate().DisplayValueSN();
				Assert::IsTrue(n1_text == "Short(1)");
				(numbers1 == Short(1)).Evaluate().Do();

				SN_DECLARE_VALUE(numbers2, Short(1) || Long(2));
				(typeChecker(numbers2) == String("long")).Assert().Do();
				std::string n2_text = numbers2.DoEvaluate().DisplayValueSN();
				Assert::IsTrue(n2_text == "Long(2)");
				(numbers2 == Long(2)).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestVirtualPolymorphicReverse3)
		{
			Initialize();
			{
				Manager manager("Test Virtual Polymorphic Reverse3", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE_VALUE(typeChecker, Virtual());
				SN_DECLARE_VALUE(shortType, Short::Class());
				SN_DECLARE_VALUE(longType, Long::Class());
				SN_DECLARE_VALUE(longLongType, LongLong::Class());

				(typeChecker(shortType) == String("short")).PartialAssert().Do();
				(typeChecker(longType) == String("long")).PartialAssert().Do();
				(typeChecker(longLongType) == String("long long")).PartialAssert().Do();

				std::string sTypeChecker_before = typeChecker.DisplayValueSN();
				typeChecker.Fix();
				std::string sTypeChecker_after = typeChecker.DisplayValueSN();

				// Reverse Polymorphic call.
				SN_DECLARE(numbers1);
				(numbers1 == (Short(1) || Long(2) || LongLong(3))).PartialAssert().Do();
				std::string numbers1_text = numbers1.DisplayValueSN();
				(typeChecker(numbers1) == String("short")).Assert().Do();
				std::string n1_text = numbers1.GetValue().DisplayValueSN();
				Assert::IsTrue(n1_text == "Short(1)");
				(numbers1 == Short(1)).Evaluate().Do();

				SN_DECLARE(numbers2);
				(numbers2 == (Short(1) || Long(2) || LongLong(3))).PartialAssert().Do();
				(typeChecker(numbers2) == String("long")).Assert().Do();
				std::string n2_text = numbers2.GetValue().DisplayValueSN();
				Assert::IsTrue(n2_text == "Long(2)");
				(numbers2 == Long(2)).Evaluate().Do();

				SN_DECLARE(numbers3);
				(numbers3 == (Short(1) || Long(2) || LongLong(3))).PartialAssert().Do();
				(typeChecker(numbers3) == String("long long")).Assert().Do();
				std::string n3_text = numbers3.GetValue().DisplayValueSN();
				Assert::IsTrue(n3_text == "LongLong(3)");
				(numbers3 == LongLong(3)).Evaluate().Do();
			}
			Cleanup();
		}
	};
}