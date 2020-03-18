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
	TEST_CLASS(test_sn_valueset)
	{
	private:
		const string doc_root = "C:/Users/peter_driscoll/Documents/Source/Repos/skynet2/html";

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
		TEST_METHOD(TestValueSetEvaluate)
		{
			Initialize();
			{
				Manager manager("Test Value Set Evaluate", AssertErrorHandler);

				// || operator applied to values other than bool, creates a value set.
				// ValueSet has Constructors for up to 5 parameters.  This will be mainly for testing. 
				Assert::IsTrue((Long(5) || Long(7)).DoEvaluate().DoRemove(Long(5)).DoRemove(Long(7)).DoIsEmpty());

				Assert::IsTrue((String("dog") || String("cat")).DoEvaluate().DoRemove(String("dog")).DoRemove(String("cat")).DoIsEmpty());

				Assert::IsTrue((Char('d') || Char('c')).DoEvaluate().DoRemove(Char('d')).DoRemove(Char('c')).DoIsEmpty());

				Assert::IsTrue((((String("catdograt") || String("pigdograt")) - (String("dograt") || String("rat")))).DoEvaluate().DoRemove(String("cat")).DoRemove(String("pig")).DoRemove(String("catdog")).DoRemove(String("pigdog")).DoIsEmpty());


				Assert::IsTrue((((String("cat") || String("dog")) + (String("food") || String("awesomefood")))).DoEvaluate().DoRemove(String("catfood")).DoRemove(String("catawesomefood")).DoRemove(String("dogfood")).DoRemove(String("dogawesomefood")).DoIsEmpty());

				Assert::IsTrue((Double(5) || Double(7)).DoEvaluate().DoRemove(Double(5)).DoRemove(Double(7)).DoIsEmpty());

				Assert::IsTrue((((Double(5) || Double(7)) + (Double(8) || Double(9)))).DoEvaluate().DoRemove(Double(13)).DoRemove(Double(14)).DoRemove(Double(15)).DoRemove(Double(16)).DoIsEmpty());
				// The order does not matter
				Assert::IsTrue((Long(-4) || Long(4)).DoEvaluate().DoRemove(Long(4)).DoRemove(Long(-4)).DoIsEmpty());

				// Value sets behave like values.  an operator or function applied to value set, generates the cardesian product of the values, with the function applied.
				Assert::IsTrue((((Long(5) || Long(7)) + (Long(8) || Long(9)))).DoEvaluate().DoRemove(Long(13)).DoRemove(Long(14)).DoRemove(Long(15)).DoRemove(Long(16)).DoIsEmpty());

				//added for subtraction
				Assert::IsTrue((((Long(9) || Long(11)) - (Long(3) || Long(4)))).DoEvaluate().DoRemove(Long(6)).DoRemove(Long(5)).DoRemove(Long(8)).DoRemove(Long(7)).DoIsEmpty());

				Assert::IsTrue((((Long(1) || Long(2)) * (Long(3) || Long(4)))).DoEvaluate().DoRemove(Long(3)).DoRemove(Long(4)).DoRemove(Long(6)).DoRemove(Long(8)).DoIsEmpty());

				Assert::IsTrue((((Long(5) || Long(10)) / (Long(1) || Long(5)))).DoEvaluate().DoRemove(Double(5)).DoRemove(Double(1)).DoRemove(Double(10)).DoRemove(Double(2)).DoIsEmpty());

				// If the values are tagged with worlds from different world sets then all the values from the cardesian product are created. 
				// first    2 || -2 = [2:A, -2:B]
				// second   2 || -2 = [2:C, -2:D]
				// first+second = [2+2:AC, 0:AD, 0:BC, -2:BD]
				Assert::IsTrue((((Long(2) || Long(-2)) + (Long(2) || Long(-2)))).DoEvaluate().DoRemove(Long(4)).DoRemove(Long(0)).DoRemove(Long(-4)).DoIsEmpty());

				// If using the same value set twice then must use the same value each time.
				// This is because the intersection of different worlds from the same value set is always empty.
				// More explictly, each value in a vale set is tagged with a world from a world set.
				//     2 || -2 = [2:A, -2:B]
				// where A and B are worlds. Worlds are sets.
				// A intersection B is empty.
				SN_DECLARE_VALUE(two_or_minus_two, (Long(2) || Long(-2)).DoEvaluate());

				Assert::IsTrue((two_or_minus_two + two_or_minus_two).DoEvaluate().DoRemove(Long(4)).DoRemove(Long(-4)).DoIsEmpty());

				Assert::IsTrue(((Long(2) || Long(-2)) + (Long(2) || Long(-2))).DoEvaluate().DoRemove(Long(4)).DoRemove(Long(0)).DoRemove(Long(-4)).DoIsEmpty());

				(two_or_minus_two + two_or_minus_two).BuildSet().HasMember(Long(-4)).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestValueSetEquivalencyEvaluate)
		{
			Initialize();
			{
				Manager manager("Test Value Set Equivalency Evaluate", AssertErrorHandler);

				(((String("Dog") || String("Cat")) + String("Food")).BuildSet() == ((String("DogFood") || String("CatFood")).BuildSet())).Evaluate().Do();

				(((String("Dog") || String("Cat")) + String("Food")).BuildSet() == ((String("CatFood") || String("DogFood")).BuildSet())).Evaluate().Do();
				(((String("Dog") || String("Pig")) + String("Food")).BuildSet() == ((String("DogFood") || String("PigFood")).BuildSet())).Evaluate().Do();

				(((Long(10) || Long(9)) + Long(1)).BuildSet() == (Long(11) || Long(10)).BuildSet()).Evaluate().Do();

				(((Double(10) || Double(9)) + Double(1)).BuildSet() == (Double(11) || Double(10)).BuildSet()).Evaluate().Do();


				// mixed value sets
				Assert::IsTrue((String("dog") || Long(9)).DoEvaluate().DoRemove(String("dog")).DoRemove(Long(9)).DoIsEmpty());

				//NESTED OR CASE
				Assert::IsTrue(((String("dog") || Long(9)) || String("Cat")).DoEvaluate().DoRemove(String("dog")).DoRemove(Long(9)).DoRemove(String("Cat")).DoIsEmpty());
			}
			Cleanup();
		}

		TEST_METHOD(TestValueSetOfLambdaFunctions)
		{
			Initialize();
			{
				Manager manager("Test Value Set Of Lambda Functions", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(plus);
				SN_DECLARE(times);

				{
					SN_DECLARE(a);
					SN_DECLARE(b);

					(Define(plus)(a)(b) == a + b).PartialAssert().Do();

					SN_DECLARE(c);
					SN_DECLARE(d);

					(Define(times)(c)(d) == c * d).PartialAssert().Do();
				}
				string s_plus = plus.DisplayValueSN();
				string s_times = times.DisplayValueSN();
				string s_plus_times = (plus || times).DoEvaluate().DisplaySN();
				string s_plus_times2 = (plus || times).DoEvaluate().DisplaySN();

				SN_DECLARE(f);
				(f == (plus || times)).Assert().Do();
				string f_string = f.DisplaySN();

				SN_DECLARE(x);
				(x == f(long(5))(long(8))).Assert().Do();
				string x_string = x.DisplaySN();
				(x < long(20)).Assert().Do();
				(x == long(13)).Assert().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestValueSetOfStandardFunctions)
		{
			Initialize();
			{
				Manager manager("Test Value Set Of Standard Functions", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(f);
				(f == (skynet::Add || skynet::Multiply)).Assert().Do();
				string f_string = f.DisplaySN();

				SN_DECLARE(x);
				(x == f(long(8))(long(13))).Assert().Do();
				string x_string = x.BuildSet().DoEvaluate().DisplaySN();
				Assert::IsTrue(x_string == "{Long(21), Long(104)}");
				(x < long(30)).Assert().Do();
				string x_string2 = x.DisplaySN();
				long x_num = Long(x).GetNumber();
				Assert::IsTrue(x_num == 21);
				(x == Long(21)).Assert().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestValueSetOfStringFunctions)
		{
			Initialize();
			{
				Manager manager("Test Value Set Of String Functions", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(f);

				SN_DECLARE(wrap);
				SN_DECLARE(map);
				{
					//Transaction transaction;
					SN_DECLARE(a);

					(Define(wrap)(a) == String("{") + (f(a) + String("}"))).PartialAssert().Do();

					SN_DECLARE(c);
					SN_DECLARE(d);

					(Define(map)(a) == a).PartialAssert().Do();
				}
				string f_before_string = f.DisplaySN();

				(Define(f) == (wrap || map)).PartialAssert().Do();
				string f_string = f.DisplaySN();

				SN_DECLARE(x);
				(String("{{expression}}") == f(x)).Assert().Do();
				string x_display = x.DisplaySN();
				string x_string = x.BuildSet().DoEvaluate().DisplaySN();
				string x_compare_string = "{String(\"expression\"), String(\"{expression}\"), String(\"{{expression}}\")}";

				Assert::IsTrue(x_string == x_compare_string);
			}
			Cleanup();
		}
	};
}