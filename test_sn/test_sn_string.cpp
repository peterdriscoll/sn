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
	TEST_CLASS(test_sn_string)
	{
	private:
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
		//	TestStringValue
		//		Test operator equals on program model string value gives the program model function call.
		//
		//		In the program model, operations dont return values.  Instead they return program model objects that represent the calculation of the values.
		//		The function Equivalent determines if the project model produced by == is the application of Equals as a function call on the two parameters.
		//
		//		Equals is a FunctionDef object that defines the function.
		TEST_METHOD(TestStringEquivalent)
		{
			Initialize();
			Assert::IsTrue(Transaction::TotalNetMemoryUsed() == 0);
			{
				Manager manager("Test String Equivalent", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				Assert::IsTrue((String("dog") == String("dog")).Equivalent(Function(Function(FunctionCall, Function(Function(FunctionCall, Equals), String("dog"))), String("dog"))));
				Assert::IsTrue(!(String("dog") == String("dog")).Equivalent(Function(Function(FunctionCall, Function(Function(FunctionCall, Equals), String("dog"))), String("cat"))));
				Assert::IsTrue((String("dog") + String("cat")).Equivalent(Function(Function(FunctionCall, Function(Function(FunctionCall, Add), String("dog"))), String("cat"))));
				Assert::IsTrue((String("dog") == String("dog")).Equivalent(Equals(String("dog"))(String("dog"))));
				Assert::IsTrue(!(String("dog") == String("dog")).Equivalent(Equals(String("dog"))(String("cat"))));
				Assert::IsTrue((String("dog") + String("cat")).Equivalent(Add(String("dog"))(String("cat"))));
			}
			Cleanup();
		}

		TEST_METHOD(TestStringEvaluate)
		{
			Initialize();
			{
				Manager manager("Test String Evaluate", AssertErrorHandler);

				(String("dog") == String("dog")).Evaluate().Do();
				(!(String("dog") == String("cat"))).Evaluate().Do();

				SN_DECLARE_VALUE(z, String("dog"));
				(String("dog") == z).Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestStringPartialEvaluate)
		{
			Initialize();
			{
				Manager manager("Test String Partial Evaluate", AssertErrorHandler);

				SN_DECLARE(y);
				SN_DECLARE(x);
				SN_DECLARE_VALUE(z, String("dog"));

				Assert::IsTrue((String("dog") == y).DoPartialEvaluate().Equivalent(Function(Function(Equals, String("dog")), y)));
				Assert::IsTrue(!(String("dog") == y).DoPartialEvaluate().Equivalent(Function(Function(Equals, String("dog")), x)));
				Assert::IsTrue((String("dog") == String("dog")).DoPartialEvaluate().Equivalent(Bool(true)));
				(String("dog") == z).DoPartialEvaluate().Do();

				Assert::IsTrue((String("dog") + y == String("dogcat")).DoPartialEvaluate().Equivalent(Function(Function(Equals, Function(Function(Add, String("dog")), y)), String("dogcat"))));
				(String("dog") + z == String("dogdog")).DoPartialEvaluate().Do();

				(String("dog") + z == "dogdog").DoPartialEvaluate().Do();
				(String("dog") + "dog" == "dogdog").DoPartialEvaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestStringAssert)
		{
			Initialize();
			{
				Manager manager("Test String Assert", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				{
					Transaction transaction;

					SN_DECLARE(y);

					(String("dog") == y).Assert().Do();
					(String("dog") == y).Evaluate().Do();

					SN_DECLARE(z);

					(String("dog") + z == "dogcat").Assert().Do();
					Value cat_value = z.GetValue();
					string s_cat_value = cat_value.GetString();
					Assert::IsTrue(s_cat_value == "cat");
					(String("cat") == z).Evaluate().Do();
				}
				{
					Transaction transaction;

					SN_DECLARE(a);
					(String("dog") + a == String("dogcat")).Assert().Do();
					(a == String("cat")).Assert().Do();
					SN_DECLARE(b);
					(b + String("cat") == String("dogcat")).Assert().Do();
					(b == String("dog")).Assert().Do();
					SN_DECLARE(c);
					(String("dog") + String("cat") == c).Assert().Do();
					(c == String("dogcat")).Assert().Do();
				}
				{
					Transaction transaction;

					SN_DECLARE(a);
					(String("dogcat").SubtractRight(String("cat")) == a).Assert().Do();
					(a == String("dog")).Assert().Do();
					SN_DECLARE(b);
					(String("dogcat").SubtractRight(b) == String("dog")).Assert().Do();
					(b == String("cat")).Assert().Do();
					SN_DECLARE(c);
					(c.SubtractRight(String("cat")) == String("dog")).Assert().Do();
					(c == String("dogcat")).Assert().Do();
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestStringRefDefinition)
		{
			Initialize();
			{
				Manager manager("Test String Ref Definition", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, false);

				SN_DECLARE(a);
				SN_DECLARE(b);
				SN_DECLARE(c);
				SN_DECLARE(d);
				(String("ratdogcat") == a + b).Assert().Do();
				(a == d + c).Assert().Do();

				(d == String("rat")).Assert().Do();
				string d_string = d.GetString();
				Assert::IsTrue(d_string == "rat");

				(c == String("dog")).Assert().Do();
				string c_string = c.GetString();
				Assert::IsTrue(c_string == "dog");

				Assert::IsTrue(b.DoEvaluate().Equivalent(String("cat")));
				string b_string = b.GetString();
				Assert::IsTrue(b_string == "cat");
			}
			Cleanup();
		}

		TEST_METHOD(TestSubStrings)
		{
			Initialize();
			{
				Manager manager("Test Sub Strings", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				(String("catdog").SubtractRight(String("catdog").SubtractLeft(String("cat"))) == String("cat")).Assert().Do();
				(String("catdog").SubtractLeft(String("catdog").SubtractRight(String("dog"))) == String("dog")).Assert().Do();

				SN_DECLARE_VALUE(x, String("catdog"));
				SN_DECLARE_VALUE(y, String("cat"));
				SN_DECLARE_VALUE(z, String("dog"));

				(x.SubtractRight(x.SubtractLeft(y)) == y).Assert().Do();
				(x.SubtractLeft(x.SubtractRight(z)) == z).Assert().Do();

				(String("GamesX").SubtractRightChar() + String("GamesX").SelectRightChar() == String("GamesX")).Assert().Do();
				SN_DECLARE_VALUE(m, String("GamesX"));
				(m.SubtractRightChar() + m.SelectRightChar() == m).Assert().Do();

				(String("XGames").SelectLeftChar() + String("XGames").SubtractLeftChar() == String("XGames")).Assert().Do();
				SN_DECLARE_VALUE(n, String("XGames"));
				(n.SelectLeftChar() + n.SubtractLeftChar() == n).Assert().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestFileAccess)
		{
			Initialize();
			{
				string s_name_1 = "c:/temp/sn_test_testfileaccess_1.txt";
				string s_name_2 = "c:/temp/sn_test_testfileaccess_2.txt";
				string s_contents_1 = "Tora! Tora! Tora!";
				string s_contents_2 = "Two dogs barking!";

				remove(s_name_1.data());
				remove(s_name_2.data());
				{
					ofstream out(s_name_1);
					out << s_contents_1;
					out.close();
				}

				Manager manager("Test File Access", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				{
					Transaction transaction;

					SN_DECLARE_VALUE(name_1, String(s_name_1));
					SN_DECLARE_VALUE(contents_1, String(s_contents_1));

					(name_1.File() == contents_1).Assert().Do();

					{
						ofstream out(s_name_1);
						out << s_contents_1;
						out.close();
					}

					(name_1.File() == contents_1).Assert().Do();

					SN_DECLARE_VALUE(name_2, String(s_name_2));
					SN_DECLARE_VALUE(contents_2, String(s_contents_2));

					(name_2.File() == contents_2).Assert().Do();

					ifstream inFile;
					inFile.open(s_name_2); // open the input file
					Assert::IsTrue(inFile.is_open());

					stringstream strStream;
					strStream << inFile.rdbuf(); // read the file
					Assert::IsTrue(strStream.str() == s_contents_2);

					{
						std::ofstream out(s_name_2);
						out << s_contents_1;
						out.close();
					}

					(name_2.File() == contents_2).Assert().Do(); // The assert still hold. For consistency, SN caches the value for the file name.
				}

				// Outside the transaction, the cache is cleared and the value is re-read.
				{
					Transaction transaction;

					SN_DECLARE_VALUE(name_1, String(s_name_1));
					SN_DECLARE_VALUE(name_2, String(s_name_2));
					SN_DECLARE_VALUE(contents_1, String(s_contents_1));
					(name_1.File() == name_2.File()).Assert().Do();
					(name_2.File() == contents_1).Assert().Do();
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestStringSearch)
		{
			Initialize();
			{
				Manager manager("Test String Search", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				{
					Transaction transaction;

					SN_DECLARE(money);
					SN_DECLARE(evil);
					(money + (String(" is the root of all ") + evil) == String("Money is the root of all evil")).Assert().Do();
					string money_string = money.GetString();
					string evil_string = evil.GetString();
					Assert::IsTrue(money_string == "Money");
					Assert::IsTrue(evil_string == "evil");
				}
				{
					Transaction transaction;

					SN_DECLARE(money);
					SN_DECLARE(evil);
					(money + String(" is the root of all ") + evil == String("Money is the root of all evil")).Assert().Do();
					string money_string = money.GetString();
					string evil_string = evil.GetString();
					Assert::IsTrue(money_string == "Money");
					Assert::IsTrue(evil_string == "evil");
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestStringSearchExample1)
		{
			Initialize();
			{
				Manager manager("Test String Search Example 1", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(firstname1);
				SN_DECLARE(surname1);
				(String("My first name is ") + (firstname1 + String(" and my surname is Fischer.")) == String("My first name is Bobby and my surname is Fischer.")).Assert().Do();
				string firstname1_string = firstname1.GetString();
				Assert::IsTrue(firstname1_string == "Bobby");

				SN_DECLARE(firstname2);
				SN_DECLARE(surname2);
				(String("My first name is ") + (firstname2 + (String(" and my surname is ") + surname2)) == String("My first name is Bobby and my surname is Fischer")).Assert().Do();
				string firstname2_string = firstname2.GetString();
				string surname2_string = surname2.GetString();
				Assert::IsTrue(firstname2_string == "Bobby");
				Assert::IsTrue(surname2_string == "Fischer");
			}
			Cleanup();
		}

		TEST_METHOD(TestStringSearchExample2)
		{
			Initialize();
			{
				Manager manager("Test String Search Example 2", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				{
					Transaction transaction;

					SN_DECLARE(firstname);
					SN_DECLARE(surname);

					(String("My first name is ") + (firstname + (String(" and my surname is ") + (surname + String(".")))) == String("My first name is Bobby and my surname is Fischer.")).Assert().Do();

					string firstname_string = firstname.GetString();
					string surname_string = surname.GetString();
					Assert::IsTrue(firstname_string == "Bobby");
					Assert::IsTrue(surname_string == "Fischer");

					(firstname == String("Bobby")).Assert().Do();
					(surname == String("Fischer")).Assert().Do();
				}
				{
					Transaction transaction;

					SN_DECLARE(firstname);
					SN_DECLARE(surname);

					(String("My first name is ") + firstname + String(" and my surname is ") + (surname + String(".")) == String("My first name is Bobby and my surname is Fischer.")).Assert().Do();
					(firstname == String("Bobby")).Assert().Do();
					(surname == String("Fischer")).Assert().Do();
					string firstname_string = firstname.GetString();
					string surname_string = surname.GetString();
					Assert::IsTrue(firstname_string == "Bobby");
					Assert::IsTrue(surname_string == "Fischer");
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestStringAmbiguity)
		{
			Initialize();
			{
				Manager manager("Test String Ambiguity", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);

				SN_DECLARE(firstClause);
				SN_DECLARE(secondClause);
				SN_DECLARE(sentence);
				(firstClause + String(" and ") + secondClause == String("I love dogs and cats and dogs love me.")).Assert().Do();
				string firstClause_string_vs = firstClause.DisplaySN();
				string secondClause_string_vs = secondClause.DisplaySN();
				
				string firstClause_string = firstClause.BuildSet().DoEvaluate().DisplaySN();
				string secondClause_string = secondClause.BuildSet().DoEvaluate().DisplaySN();
				Assert::IsTrue(firstClause_string == "{String(\"I love dogs\"), String(\"I love dogs and cats\")}");
				Assert::IsTrue(secondClause_string == "{String(\"cats and dogs love me.\"), String(\"dogs love me.\")}");
				
				(sentence == firstClause + (String(" and ") + secondClause)).Assert().Do();
				string sentence_string_vs = sentence.DisplaySN();
				string sentence_string = sentence.GetString();
				Assert::IsTrue(sentence_string == "I love dogs and cats and dogs love me.");
			}
			Cleanup();
		}		
	};
}