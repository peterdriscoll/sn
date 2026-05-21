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
				Manager manager("Test Inherit Instance", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", port, doc_root, runWebServer);

				skynet::True.In(Bool::Class()).Assert().Do();
				String("woof").In(String::Class()).Assert().Do();
				Char('X').In(Char::Class()).Assert().Do();
				(String("DogFood") || String("CatFood")).BuildSet().In(Set::Class()).Assert().Do();

				Meta(1, String("Vincent ") + String("Emma ")).In(Meta::Class()).Assert().Do();

				Short(5).In(Short::Class()).Assert().Do();
				Int(5).In(Int::Class()).Assert().Do();
				Long(5).In(Long::Class()).Assert().Do();
				LongLong(5).In(LongLong::Class()).Assert().Do();
				Float(5).In(Float::Class()).Assert().Do();
				Double(5).In(Double::Class()).Assert().Do();
				LongDouble(5).In(LongDouble::Class()).Assert().Do();
				Mapping().In(Mapping::Class()).Assert().Do();

				SN_DECLARE(a);
				SN_DECLARE(b);
				(String("dogcat") == a + b).Assert().Do();

				a.In(StringRef::Class()).Assert().Do();

				skynet::True.Assert().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestInheritShort)
		{
			Initialize();
			{
				Manager manager("Test Inherit Short", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", port, doc_root, runWebServer);

				Short::Class().IsA(Long::Class()).Evaluate().Do().CheckValue();
				SN_DECLARE(x1);
				(Short::Class().IsA(Long::Class()) == x1).Assert().Do();
				x1.Evaluate().Do().CheckValue();
				bool b1 = x1.GetBool();
				Assert::IsTrue(b1);

				Short::Class().IsA(LongLong::Class()).Evaluate().Do().CheckValue();
				SN_DECLARE(x2);
				(Short::Class().IsA(LongLong::Class()) == x2).Assert().Do();
				x2.Evaluate().Do().CheckValue();
				bool b2 = x2.GetBool();
				Assert::IsTrue(b2);

				Short::Class().IsA(Float::Class()).Evaluate().Do().CheckValue();
				SN_DECLARE(x3);
				(Short::Class().IsA(Float::Class()) == x3).Assert().Do();
				x3.Evaluate().Do().CheckValue();
				bool b3 = x3.GetBool();
				Assert::IsTrue(b3);

				Short::Class().IsA(Double::Class()).Evaluate().Do().CheckValue();
				SN_DECLARE(x4);
				(Short::Class().IsA(Double::Class()) == x4).Assert().Do();
				x4.Evaluate().Do().CheckValue();
				bool b4 = x4.GetBool();
				Assert::IsTrue(b4);

				Short::Class().IsA(LongDouble::Class()).Evaluate().Do().CheckValue();
				SN_DECLARE(x5);
				(Short::Class().IsA(LongDouble::Class()) == x5).Assert().Do();
				x5.Evaluate().Do().CheckValue();
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
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", port, doc_root, runWebServer);

				Long::Class().IsA(LongLong::Class()).Evaluate().Do().CheckValue();
				SN_DECLARE(x1);
				(Long::Class().IsA(LongLong::Class()) == x1).Assert().Do();
				x1.Evaluate().Do().CheckValue();
				bool b1 = x1.GetBool();
				Assert::IsTrue(b1);

				Long::Class().IsA(Double::Class()).Evaluate().Do().CheckValue();
				SN_DECLARE(x2);
				(Long::Class().IsA(Double::Class()) == x2).Assert().Do();
				x2.Evaluate().Do().CheckValue();
				bool b2 = x2.GetBool();
				Assert::IsTrue(b2);

				Long::Class().IsA(LongDouble::Class()).Evaluate().Do().CheckValue();
				SN_DECLARE(x3);
				(Long::Class().IsA(LongDouble::Class()) == x3).Assert().Do();
				x3.Evaluate().Do().CheckValue();
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
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", port, doc_root, runWebServer);

				LongLong::Class().IsA(LongDouble::Class()).Evaluate().Do().CheckValue();
				SN_DECLARE(x1);
				(LongLong::Class().IsA(LongDouble::Class()) == x1).Assert().Do();
				x1.Evaluate().Do().CheckValue();
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
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", port, doc_root, runWebServer);

				Float::Class().IsA(Double::Class()).Evaluate().Do().CheckValue();
				SN_DECLARE(x1);
				(Float::Class().IsA(Double::Class()) == x1).Assert().Do();
				x1.Evaluate().Do().CheckValue();
				bool b1 = x1.GetBool();
				Assert::IsTrue(b1);

				Float::Class().IsA(LongDouble::Class()).Evaluate().Do().CheckValue();
				SN_DECLARE(x2);
				(Float::Class().IsA(LongDouble::Class()) == x2).Assert().Do();
				x2.Evaluate().Do().CheckValue();
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
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", port, doc_root, runWebServer);

				Double::Class().IsA(LongDouble::Class()).Evaluate().Do().CheckValue();
				SN_DECLARE(x1);
				(Double::Class().IsA(LongDouble::Class()) == x1).Assert().Do();
				x1.Evaluate().Do().CheckValue();
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
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", port, doc_root, runWebServer);

				Char::Class().IsA(String::Class()).Evaluate().Do().CheckValue();
				SN_DECLARE(x1);
				(Char::Class().IsA(String::Class()) == x1).Assert().Do();
				x1.Evaluate().Do().CheckValue();
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
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", port, doc_root, runWebServer);

				StringRef::Class().IsA(String::Class()).Evaluate().Do().CheckValue();
				SN_DECLARE(x1);
				(StringRef::Class().IsA(String::Class()) == x1).Assert().Do();
				x1.Evaluate().Do().CheckValue();
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
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", port, doc_root, runWebServer);

				SN_DECLARE_VALUE(fib, Derived());
				SN_DECLARE(N);
				(fib(Long(0)) == Long(1)).PartialAssert().Do();
				(fib(Long(1)) == Long(1)).PartialAssert().Do();
				(Define(fib)(N) == (N >Long(1)).If(fib(N - Long(1)) + fib(N - Long(2)))).PartialAssert().Do();
				
				std::string sfib_before = fib.DisplayValueSN();
				fib.Fix();
				std::string sfib_after = fib.DisplayValueSN();

				(fib(Long(0)) == Long(1)).Evaluate().Do().CheckValue();
				(fib(Long(1)) == Long(1)).Evaluate().Do().CheckValue();
				(fib(Long(2)) == Long(2)).Evaluate().Do().CheckValue();

				(fib(Long(3)) == Long(3)).Evaluate().Do().CheckValue();
				(fib(Long(4)) == Long(5)).Evaluate().Do().CheckValue();
				(fib(Long(5)) == Long(8)).Evaluate().Do().CheckValue();
				(fib(Long(6)) == Long(13)).Evaluate().Do().CheckValue();

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
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", port, doc_root, runWebServer);

				SN_DECLARE_VALUE(fib, Virtual());
				SN_DECLARE(N);
				(fib(Long(0)) == Long(1)).PartialAssert().Do();
				(fib(Long(1)) == Long(1)).PartialAssert().Do();
				(Define(fib)(N) == fib(N - Long(1)) + fib(N - Long(2))).PartialAssert().Do();

				std::string sfib_before = fib.DisplayValueSN();
				fib.Fix();
				std::string sfib_after = fib.DisplayValueSN();

				(fib(Long(0)) == Long(1)).Evaluate().Do().CheckValue();
				(fib(Long(1)) == Long(1)).Evaluate().Do().CheckValue();
				(fib(Long(2)) == Long(2)).Evaluate().Do().CheckValue();
				(fib(Long(3)) == Long(3)).Evaluate().Do().CheckValue();
				(fib(Long(4)) == Long(5)).Evaluate().Do().CheckValue();
				(fib(Long(5)) == Long(8)).Evaluate().Do().CheckValue();
				(fib(Long(6)) == Long(13)).Evaluate().Do().CheckValue();

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
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", port, doc_root, runWebServer);

				SN_DECLARE_VALUE(typeChecker, Virtual());
				SN_DECLARE_TYPE(shortType, Short::Class());

				(typeChecker(shortType) == String("short")).PartialAssert().Do();

				std::string sTypeChecker_before = typeChecker.DisplayValueSN();
				typeChecker.Fix();
				std::string sTypeChecker_after = typeChecker.DisplayValueSN();

				// Polymorphic call.
				(typeChecker(Short(1)) == String("short")).Evaluate().Do().CheckValue();

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
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", port, doc_root, runWebServer);

				SN_DECLARE_VALUE(typeChecker, Virtual());
				SN_DECLARE_TYPE(shortType, Short::Class());
				SN_DECLARE_TYPE(longType, Long::Class());
				SN_DECLARE(v);

				(typeChecker(longType) == String("long")).PartialAssert().Do();
				(typeChecker(shortType) == String("short")).PartialAssert().Do();
				(typeChecker(v) == String("unknown")).PartialAssert().Do();

				std::string sTypeChecker_before = typeChecker.DisplayValueSN();
				typeChecker.Fix();
				std::string sTypeChecker_after = typeChecker.DisplayValueSN();

				SN_DECLARE_VALUE(u, skynet::False);

				SN_DECLARE(A);
				SN_DECLARE(B);
				SN_DECLARE(C);

				(typeChecker(Short(1)) == A).Assert().Do();
				(typeChecker(Long(1)) == B).Assert().Do();
				(typeChecker(u) == C).Assert().Do();

				std::string A_text = A.GetString();
				std::string B_text = B.GetString();
				std::string C_text = C.GetString();

				Assert::IsTrue(A_text == "short");
				Assert::IsTrue(B_text == "long");
				Assert::IsTrue(C_text == "unknown");
				(typeChecker(Short(1)) == String("short")).Evaluate().Do().CheckValue();
				(typeChecker(Long(1)) == String("long")).Evaluate().Do().CheckValue();
				(typeChecker(u) == String("unknown")).Evaluate().Do().CheckValue();
				(typeChecker(Short(1)) == String("short")).Assert().Do();
				(typeChecker(Long(1)) == String("long")).Assert().Do();
				(typeChecker(u) == String("unknown")).Assert().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestVirtualPolymorphic3)
		{
			Initialize();
			{
				Manager manager("Test Virtual Polymorphic3", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", port, doc_root, runWebServer);

				SN_DECLARE_VALUE(typeChecker, Virtual());
				SN_DECLARE_TYPE(shortType, Short::Class());
				SN_DECLARE_TYPE(longType, Long::Class());
				SN_DECLARE_TYPE(longLongType, LongLong::Class());

				(typeChecker(shortType) == String("short")).PartialAssert().Do();
				(typeChecker(longType) == String("long")).PartialAssert().Do();
				(typeChecker(longLongType) == String("long long")).PartialAssert().Do();

				std::string sTypeChecker_before = typeChecker.DisplayValueSN();
				typeChecker.Fix();
				std::string sTypeChecker_after = typeChecker.DisplayValueSN();

				// Polymorphic call.
				(typeChecker(Short(1)) == String("short")).Evaluate().Do().CheckValue();
				(typeChecker(Long(1)) == String("long")).Evaluate().Do().CheckValue();
				(typeChecker(LongLong(1)) == String("long long")).Evaluate().Do().CheckValue();

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
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", port, doc_root, runWebServer);

				SN_DECLARE_VALUE(typeChecker, Virtual());
				SN_DECLARE_TYPE(shortType, Short::Class());

				(typeChecker(shortType) == String("short")).PartialAssert().Do();

				std::string sTypeChecker_before = typeChecker.DisplayValueSN();
				typeChecker.Fix();
				std::string sTypeChecker_after = typeChecker.DisplayValueSN();

				// Reverse Polymorphic call.
				SN_DECLARE_VALUE(numbers1, Short(1) || Long(2));
				std::string n1_text_before = numbers1.DoEvaluate().DisplayValueSN();

				(typeChecker(numbers1) == String("short")).Assert().Do();
				std::string n1_text = numbers1.GetValue().DisplayValueSN();
				Assert::IsTrue(n1_text == "Short(1)");
				(numbers1 == Short(1)).Evaluate().Do().CheckValue();
			}
			Cleanup();
		}

		TEST_METHOD(TestVirtualPolymorphicReverse2)
		{
			Initialize();
			{
				Manager manager("Test Virtual Polymorphic Reverse 2", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", port, doc_root, runWebServer);

				SN_DECLARE_VALUE(typeChecker, Virtual());
				SN_DECLARE_TYPE(shortType, Short::Class());
				SN_DECLARE_TYPE(longType, Long::Class());

				(typeChecker(shortType) == String("short")).PartialAssert().Do();
				(typeChecker(longType) == String("long")).PartialAssert().Do();

				std::string sTypeChecker_before = typeChecker.DisplayValueSN();
				typeChecker.Fix();
				std::string sTypeChecker_after = typeChecker.DisplayValueSN();

				// Reverse Polymorphic call.
				//SN_DECLARE_VALUE(numbers1, Short(1) || Long(2));
				SN_DECLARE(numbers1);
				(numbers1 == (Short(1) || Long(2))).PartialAssert().Do();

				std::string n1_text_before = numbers1.GetValue().DisplayValueSN();
				(typeChecker(numbers1) == String("short")).Assert().Do();
				std::string n1_text = numbers1.GetValue().DisplayValueSN();
				Assert::IsTrue(n1_text == "Short(1)");
				(numbers1 == Short(1)).Evaluate().Do().CheckValue();

				SN_DECLARE_VALUE(numbers2, (Short(1) || Long(2)).DoEvaluate());
				std::string n2_text_before = numbers2.GetValue().DisplayValueSN();
				(typeChecker(numbers2) == String("long")).Assert().Do();
				std::string n2_text = numbers2.GetValue().DisplayValueSN();
				Assert::IsTrue(n2_text == "Long(2)");
				(numbers2 == Long(2)).Evaluate().Do().CheckValue();
			}
			Cleanup();
		}

		TEST_METHOD(TestVirtualPolymorphicReverse3)
		{
			Initialize();
			{
				Manager manager("Test Virtual Polymorphic Reverse3", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", port, doc_root, runWebServer);

				SN_DECLARE_VALUE(typeChecker, Virtual());
				SN_DECLARE_TYPE(shortType, Short::Class());
				SN_DECLARE_TYPE(longType, Long::Class());
				SN_DECLARE_TYPE(longLongType, LongLong::Class());

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
				(numbers1 == Short(1)).Evaluate().Do().CheckValue();

				SN_DECLARE(numbers2);
				(numbers2 == (Short(1) || Long(2) || LongLong(3))).PartialAssert().Do();
				std::string numbers2_text = numbers2.DisplayValueSN();
				(typeChecker(numbers2) == String("long")).Assert().Do();
				std::string n2_text = numbers2.GetValue().DisplayValueSN();
				Assert::IsTrue(n2_text == "Long(2)");
				(numbers2 == Long(2)).Evaluate().Do().CheckValue();

				SN_DECLARE(numbers3);
				std::string numbers3_text = numbers3.DisplayValueSN();
				(numbers3 == (Short(1) || Long(2) || LongLong(3))).PartialAssert().Do();
				(typeChecker(numbers3) == String("long long")).Assert().Do();
				std::string n3_text = numbers3.GetValue().DisplayValueSN();
				Assert::IsTrue(n3_text == "LongLong(3)");
				(numbers3 == LongLong(3)).Evaluate().Do().CheckValue();
			}
			Cleanup();
		}
		
		TEST_METHOD(TestSimpleClass)
		{
			Initialize();
			{
				Manager manager("Test Simple Class", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", port, doc_root, runWebServer);

				SN_DECLARE(j);
				j.In(LongLong::Class()).Assert().Do();
				std::string j_class_text = j.DisplayValueSN();
				Assert::IsTrue(j_class_text == "j:LongLong");


				SN_DECLARE_TYPE(n, Long::Class());
				std::string n_class_text = n.DisplayValueSN();
				Assert::IsTrue(n_class_text == "n:Long");
				
                (n == Long(5)).Assert().Do();
				std::string n_text = n.DisplayValueSN();
				Assert::IsTrue(n_text == "n:=5:Long");

				SN_DECLARE_TYPE(q, Long::Class());
				std::string q_class_text = q.DisplayValueSN();
				Assert::IsTrue(q_class_text == "q:Long");
				size_t q_card_before = q.Cardinality();
				Assert::IsTrue(q_card_before == CARDINALITY_MAX);
                (q == Short(5)).Assert().Do();
				size_t q_card_after = q.Cardinality();
				Assert::IsTrue(q_card_after == 1);
				std::string q_text = q.DisplayValueSN();
				Assert::IsTrue(q_text == "q:=Short(5):Long");
                (q == Long(5)).Assert().Do();
				q_text = q.DisplayValueSN();
				Assert::IsTrue(q_text == "q:=Short(5):Long");

				SN_DECLARE_TYPE(m, Long::Class());
				std::string m_class_text = m.DisplayValueSN();
				Assert::IsTrue(m_class_text == "m:Long");
				size_t m_card_before = m.Cardinality();
				Assert::IsTrue(m_card_before == CARDINALITY_MAX);
				try
				{
                    (m == String("5")).Assert().Throw();
					Assert::IsTrue(false, L"Expected a type conflict");
				}
				catch (Error e)
				{
					std::string description = e.GetDescription();
					Assert::IsTrue(description == "Type conflict: String(\"5\") is not a Long", std::wstring(description.begin(), description.end()).c_str());
				}
				std::string m_text = m.DisplayValueSN();
				Assert::IsTrue(m_text != "String(\"5\")");
				Assert::IsTrue(m_text == "m:Long");
				size_t m_card_after = m.Cardinality();
				Assert::IsTrue(m_card_after == CARDINALITY_MAX);

				(m == Long(7)).Assert().Do();
				m_text = m.DisplayValueSN();
				Assert::IsTrue(m_text == "m:=7:Long");
				Assert::IsTrue(m.Cardinality() == 1);

				SN_DECLARE_TYPE(x, Long::Class());
				(x == Short(5)).Assert().Do();
				try
				{
					(x == Short(6)).Assert().Throw();
					Assert::IsTrue(false, L"Expected an assertion failure");
				}
				catch (Error e)
				{
					std::string description = e.GetDescription();
					Assert::IsTrue(description == "Assertion failure: Expected true for 5==6", std::wstring(description.begin(), description.end()).c_str());
				}

				SN_DECLARE_TYPE(y, Short::Class());
				try
				{
					(y == Long(5)).Assert().Throw();
					Assert::IsTrue(false, L"Expected a type conflict");
				}
				catch (Error e)
				{
					std::string description = e.GetDescription();
					Assert::IsTrue(description == "Type conflict: Long(5) is not a Short", std::wstring(description.begin(), description.end()).c_str());
				}
			}
		}
		TEST_METHOD(TestVirtualPolymorphicAnimal2)
		{
			Initialize();
			{
				Manager manager("Test Virtual Polymorphic Animal2", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE_CLASS(animal);
				SN_DECLARE_CLASS(cat);

                cat.IsA(animal).Assert().Do();
				cat.Fix();
				animal.Fix();

				SN_DECLARE_VALUE(typeChecker, Virtual());
				SN_DECLARE(v);
				SN_DECLARE_TYPE(vcat, cat);
				SN_DECLARE_TYPE(vanimal, animal);

				(typeChecker(vcat) == String("cat")).PartialAssert().Do();
				(typeChecker(vanimal) == String("animal")).PartialAssert().Do();
				(typeChecker(v) == String("other")).PartialAssert().Do();

				std::string sTypeChecker_before = typeChecker.DisplayValueSN();
				typeChecker.Fix();
				std::string sTypeChecker_after = typeChecker.DisplayValueSN();

				SN_DECLARE_VALUE(u, skynet::False);

				SN_DECLARE(A);
				SN_DECLARE(B);
				SN_DECLARE(C);

				SN_DECLARE_INSTANCE_CLASS(emma, cat);
				SN_DECLARE_INSTANCE_CLASS(waggy, animal);
				
                Expression emma_cat = emma.In(cat).Evaluate().Do();
                std::string emma_cat_text = emma_cat.DisplayValueSN();
				Assert::IsTrue(emma_cat_text == "Bool(true)");
				
				(typeChecker(emma) == A).Assert().Do();
				std::string A_text = A.GetString();
				Assert::IsTrue(A_text == "cat");

				waggy.In(cat).Evaluate().Do().CheckValue(skynet::False);
                Expression waggy_cat = waggy.In(cat).Evaluate().Do();
                std::string waggy_cat_text = waggy_cat.DisplayValueSN();

				waggy.In(animal).Evaluate().Do().CheckValue(skynet::True);
				Expression waggy_animal = waggy.In(animal).Evaluate().Do();
                std::string waggy_animal_text = waggy_animal.DisplayValueSN(); 

 				manager.Breakpoint();
                (typeChecker(waggy) == B).Assert().Do();
				std::string B_text = B.GetString();
				Assert::IsTrue(B_text == "animal");

                (typeChecker(u) == C).Assert().Do();
				std::string C_text = C.GetString();
				Assert::IsTrue(C_text == "other");

				// Polymorphic call.
				(typeChecker(emma) == String("cat")).Evaluate().Do().CheckValue();
				(typeChecker(waggy) == String("animal")).Evaluate().Do().CheckValue();
				(typeChecker(u) == String("other")).Evaluate().Do().CheckValue();

				(typeChecker(emma) == String("cat")).Assert().Do();
				(typeChecker(waggy) == String("animal")).Assert().Do();
				(typeChecker(u) == String("other")).Assert().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestInstanceClass_ParentClass)
		{
			Initialize();
			{
				Manager manager("Test Virtual Polymorphic Animal2", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE_CLASS(Dog);
                SN_DECLARE_CLASS_CLASS(Samoyed, Dog);

				// 1. Exact direct class
				SN_DECLARE_INSTANCE_CLASS(Katara, Samoyed);

				(Katara.In(Dog)).Evaluate().Do().CheckValue();

				// ParentClass not yet implemented. Need to implement.
				// (Katara.ParentClass() == Samoyed).Evaluate().Do().CheckValue();
                std::string Katara_text = Katara.DisplaySN();
                Assert::IsTrue(Katara_text == "User.Katara:User.Samoyed");

				SN_DECLARE_CLASS(Duck);
				SN_DECLARE_CLASS(Mallard);
				SN_DECLARE_INSTANCE(Quacker1);
				SN_DECLARE_INSTANCE(Quacker2);
				Quacker1.In(Mallard).Assert().Do();
				Quacker1.In(Duck).Assert().Do();
				Quacker2.In(Duck).Assert().Do();
				Quacker2.In(Mallard).Assert().Do();

				std::string Quacker1_text = Quacker1.DisplaySN();
                Assert::IsTrue(Quacker1_text == "User.Quacker1:synthetic_1");
                std::string Quacker2_text = Quacker2.DisplaySN();
                Assert::IsTrue(Quacker2_text == "User.Quacker2:synthetic_2");
			}
			Cleanup();
		}

	};
}