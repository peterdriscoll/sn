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
	TEST_CLASS(test_sn_parser)
	{
	private:
		bool runWebServer = false;
		bool autoExpand = true;

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
		TEST_METHOD(TestCharactersets)
		{
			Initialize();
			{
				Manager manager("Test CharacterSets", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				manager.SetAutoExpandNull(autoExpand);

				CharacterSet characterSet;

				characterSet.Digit(String("0")).Assert().Do();
				characterSet.Digit(String("5")).Assert().Do();
				characterSet.Digit(String("9")).Assert().Do();
				(!characterSet.Digit(String("&"))).Assert().Do();

				characterSet.AlphaLower(String("a")).Assert().Do();
				characterSet.AlphaLower(String("z")).Assert().Do();
				(!characterSet.AlphaLower(String("Z"))).Assert().Do();

				characterSet.AlphaUpper(String("A")).Assert().Do();
				characterSet.AlphaUpper(String("Z")).Assert().Do();
				(!characterSet.AlphaUpper(String("q"))).Assert().Do();

				characterSet.Alpha(String("A")).Assert().Do();
				characterSet.Alpha(String("Z")).Assert().Do();
				characterSet.Alpha(String("a")).Assert().Do();
				characterSet.Alpha(String("z")).Assert().Do();
				(!characterSet.Alpha(String("_"))).Assert().Do();

				characterSet.AlphaUnder(String("A")).Assert().Do();
				characterSet.AlphaUnder(String("Z")).Assert().Do();
				characterSet.AlphaUnder(String("a")).Assert().Do();
				characterSet.AlphaUnder(String("z")).Assert().Do();
				characterSet.AlphaUnder(String("_")).Assert().Do();
				(!characterSet.AlphaUnder(String("9"))).Assert().Do();

				characterSet.AlphaNumeric(String("A")).Assert().Do();
				characterSet.AlphaNumeric(String("Z")).Assert().Do();
				characterSet.AlphaNumeric(String("a")).Assert().Do();
				characterSet.AlphaNumeric(String("z")).Assert().Do();
				characterSet.AlphaNumeric(String("0")).Assert().Do();
				characterSet.AlphaNumeric(String("5")).Assert().Do();
				characterSet.AlphaNumeric(String("9")).Assert().Do();
				(!characterSet.AlphaNumeric(String("_"))).Assert().Do();

				characterSet.AlphaUnderNumeric(String("A")).Assert().Do();
				characterSet.AlphaUnderNumeric(String("Z")).Assert().Do();
				characterSet.AlphaUnderNumeric(String("a")).Assert().Do();
				characterSet.AlphaUnderNumeric(String("z")).Assert().Do();
				characterSet.AlphaUnderNumeric(String("0")).Assert().Do();
				characterSet.AlphaUnderNumeric(String("5")).Assert().Do();
				characterSet.AlphaUnderNumeric(String("9")).Assert().Do();
				characterSet.AlphaUnderNumeric(String("_")).Assert().Do();
				(!characterSet.AlphaUnderNumeric(String(" "))).Assert().Do();

				characterSet.White(String(" ")).Assert().Do();
				characterSet.White(String("\t")).Assert().Do();
				characterSet.White(String("\n")).Assert().Do();
				characterSet.White(String("\r")).Assert().Do();
				(!characterSet.White(String("X"))).Assert().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestValidate_IsInteger)
		{
			Initialize();
			{
				Manager manager("Test Validate IsInteger", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				manager.SetAutoExpandNull(autoExpand);

				CharacterSet characterSet;
				Validate validate(characterSet);

				(!validate.IsInteger(String(""))).Assert().Do();
				validate.IsInteger(String("1")).Assert().Do();
				validate.IsInteger(String("45")).Assert().Do();
				validate.IsInteger(String("543")).Assert().Do();
				validate.IsInteger(String("-821")).Assert().Do();
				(!validate.IsInteger(String("543X"))).Assert().Do();
				(!validate.IsInteger(String("A543X"))).Assert().Do();

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s+t == String("")).Assert().Do();
					(!validate.IsInteger(s)).Assert().Do();
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("1 dog")).Assert().Do();
					validate.IsInteger(s).Assert().Do();
					(t == String(" dog")).Evaluate().Do();
					string t_string = t.GetString();
					Assert::IsTrue(t_string == " dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("45 dog")).Assert().Do();
					validate.IsInteger(s).Assert().Do();
					(t == String(" dog")).Evaluate().Do();
					string s_string = s.GetString();
					string t_string = t.GetString();
					Assert::IsTrue(s_string == "45");
					Assert::IsTrue(t_string == " dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("568 dog")).Assert().Do();
					validate.IsInteger(s).Assert().Do();
					(t == String(" dog")).Evaluate().Do();
					string s_string = s.GetString();
					string t_string = t.GetString();
					Assert::IsTrue(s_string == "568");
					Assert::IsTrue(t_string == " dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("9678 dog")).Assert().Do();
					validate.IsInteger(s).Assert().Do();
					(t == String(" dog")).Evaluate().Do();
					string s_string = s.GetString();
					string t_string = t.GetString();
					Assert::IsTrue(s_string == "9678");
					Assert::IsTrue(t_string == " dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("A543X dog")).Assert().Do();
					(!validate.IsInteger(s)).Assert().Do();
					string s_string = s.DisplayValueSN();
					string s_part = s_string.substr(0, 37 - 5);
					string s_comp = "StringRef(\"A543X dog\"[0.._split_";
					Assert::IsTrue(s_part == s_comp);
					string t_string = t.DisplayValueSN();
					string t_part = t_string.substr(0, 37 - 8);
					string t_comp = "StringRef(\"A543X dog\"[_split_";
					Assert::IsTrue(t_part == t_comp);
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestValidate_IsFloatingPoint)
		{
			Initialize();
			{
				Manager manager("Test Validate IsFloatingPoint", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				manager.SetAutoExpandNull(autoExpand);

				CharacterSet characterSet;
				Validate validate(characterSet);

				(!validate.IsFloatingPoint(String(""))).Assert().Do();
				validate.IsFloatingPoint(String("135")).Assert().Do();
				validate.IsFloatingPoint(String("45.9")).Assert().Do();
				validate.IsFloatingPoint(String("543.995")).Assert().Do();

				(!validate.IsFloatingPoint(String("8."))).Assert().Do();
				(!validate.IsFloatingPoint(String("543."))).Assert().Do();
				(!validate.IsFloatingPoint(String("543.89X"))).Assert().Do();
				(!validate.IsFloatingPoint(String("A543X"))).Assert().Do();

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("")).Assert().Do();
					(!validate.IsFloatingPoint(s)).Assert().Do();
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("536. dog")).Assert().Do();
					(validate.IsFloatingPoint(s)).Assert().Do();
					(t == String(". dog")).Evaluate().Do();
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("3.1415 dog")).Assert().Do();
					validate.IsFloatingPoint(s).Assert().Do();
					(t == String(" dog")).Evaluate().Do();
					string t_string = t.GetString();
					Assert::IsTrue(t_string == " dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("45 dog")).Assert().Do();
					validate.IsFloatingPoint(s).Assert().Do();
					(t == String(" dog")).Evaluate().Do();
					string s_string = s.GetString();
					string t_string = t.GetString();
					Assert::IsTrue(s_string == "45");
					Assert::IsTrue(t_string == " dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("3.141596 dog")).Assert().Do();
					validate.IsFloatingPoint(s).Assert().Do();
					(t == String(" dog")).Evaluate().Do();
					string s_string = s.GetString();
					string t_string = t.GetString();
					Assert::IsTrue(s_string == "3.141596");
					Assert::IsTrue(t_string == " dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("9678.345 dog")).Assert().Do();
					validate.IsFloatingPoint(s).Assert().Do();
					(t == String(" dog")).Evaluate().Do();
					string s_string = s.GetString();
					string t_string = t.GetString();
					Assert::IsTrue(s_string == "9678.345");
					Assert::IsTrue(t_string == " dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("A2.1X dog")).Assert().Do();
					(!validate.IsFloatingPoint(s)).Assert().Do();
					string s_string = s.DisplayValueSN();
					string s_part = s_string.substr(0, 37 - 5);
					string s_comp = "StringRef(\"A2.1X dog\"[0.._split_";
					Assert::IsTrue(s_part == s_comp);
					string t_string = t.DisplayValueSN();
					string t_part = t_string.substr(0, 37 - 8);
					string t_comp = "StringRef(\"A2.1X dog\"[_split_";
					Assert::IsTrue(t_part == t_comp);
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestValidate_IsExponential)
		{
			Initialize();
			{
				Manager manager("Test Validate IsExponential", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				manager.SetAutoExpandNull(autoExpand);

				CharacterSet characterSet;
				Validate validate(characterSet);

				(!validate.IsExponential(String(""))).Assert().Do();
				validate.IsExponential(String("135")).Assert().Do();
				validate.IsExponential(String("45.9")).Assert().Do();
				validate.IsExponential(String("543.995e19")).Assert().Do();
				validate.IsExponential(String("6.626e-34")).Assert().Do();
				validate.IsExponential(String("6e+12")).Assert().Do();      // Simple integer base
				validate.IsExponential(String("-0.5e2")).Assert().Do();     // Negative base, valid exponent
				validate.IsExponential(String("+12e+3")).Assert().Do();     // Signed integer base and exponent
				validate.IsExponential(String("0e0")).Assert().Do();        // Zero base and exponent
				validate.IsExponential(String("0.0e-0")).Assert().Do();     // Edge case, all zeros

				(!validate.IsExponential(String("dog"))).Assert().Do();		// Not an integer.
				(!validate.IsExponential(String("8."))).Assert().Do();
				(!validate.IsExponential(String("543."))).Assert().Do();
				(!validate.IsExponential(String("543.843E"))).Assert().Do();
				(!validate.IsExponential(String("543.89e13X"))).Assert().Do();
				(!validate.IsExponential(String("A53.12e12X"))).Assert().Do();

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("")).Assert().Do();
					(!validate.IsExponential(s)).Assert().Do();
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("536. dog")).Assert().Do();
					(validate.IsExponential(s)).Assert().Do();
					(t == String(". dog")).Assert().Do();
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("536.89e dog")).Assert().Do();
					(validate.IsExponential(s)).Assert().Do();
					(t == String("e dog")).Assert().Do();
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("536.e45 dog")).Assert().Do();
					(validate.IsExponential(s)).Assert().Do();
					(t == String(".e45 dog")).Assert().Do();
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("3.1415 dog")).Assert().Do();
					validate.IsExponential(s).Assert().Do();
					(s == String("3.1415")).Evaluate().Do();
					(t == String(" dog")).Evaluate().Do();
					string s_string = s.GetString();
					string t_string = t.GetString();
					Assert::IsTrue(s_string == "3.1415");
					Assert::IsTrue(t_string == " dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("45 dog")).Assert().Do();
					validate.IsExponential(s).Assert().Do();
					(t == String(" dog")).Evaluate().Do();
					string s_string = s.GetString();
					string t_string = t.GetString();
					Assert::IsTrue(s_string == "45");
					Assert::IsTrue(t_string == " dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("3.141596 dog")).Assert().Do();
					validate.IsExponential(s).Assert().Do();
					(t == String(" dog")).Evaluate().Do();
					string s_string = s.GetString();
					string t_string = t.GetString();
					Assert::IsTrue(s_string == "3.141596");
					Assert::IsTrue(t_string == " dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("9678.345e45 dog")).Assert().Do();
					string s_dislay = s.DisplayValueSN();
					string t_display = t.DisplayValueSN();
					validate.IsExponential(s).Assert().Do();
					string s_dislay1 = s.DisplayValueSN();
					string t_display1 = t.DisplayValueSN();
					string s_string = s.GetString();
					string t_string = t.GetString();
					Assert::IsTrue(s_string == "9678.345e45");
					Assert::IsTrue(t_string == " dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("A2.1X dog")).Assert().Do();
					(!validate.IsExponential(s)).Assert().Do();
					string s_string = s.DisplayValueSN();
					string s_part = s_string.substr(0, 37 - 5);
					string s_comp = "StringRef(\"A2.1X dog\"[0.._split_";
					Assert::IsTrue(s_part == s_comp);
					string t_string = t.DisplayValueSN();
					string t_part = t_string.substr(0, 37 - 8);
					string t_comp = "StringRef(\"A2.1X dog\"[_split_";
					Assert::IsTrue(t_part == t_comp);
				}
				manager.DelayedCalls().Evaluate().Do();
			}
			Cleanup();
		}

		TEST_METHOD(TestValidate_IsString)
		{
			Initialize();
			{
				Manager manager("Test Validate IsString", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				manager.SetAutoExpandNull(autoExpand);

				CharacterSet characterSet;
				Validate validate(characterSet);

				//manager.Breakpoint();
				(!validate.IsString(String("\"\"D"))).Assert().Do();

				(!validate.IsString(String("\"D"))).Assert().Do();
				(!validate.IsString(String("\"X\\"))).Assert().Do();

				(!validate.IsString(String(""))).Assert().Do();
				validate.IsString(String("\"\"")).Assert().Do();
				validate.IsString(String("\"Simple string\"")).Assert().Do();
				validate.IsString(String("\"C \\\"e\\\" q\"")).Assert().Do();
				validate.IsString(String("\"E \\ q\"")).Assert().Do();
				(!validate.IsString(String("\"Cat \" dog"))).Assert().Do();

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String(" dog")).Assert().Do();
					(!validate.IsString(s)).Assert().Do();
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("\"\" dog")).Assert().Do();
					validate.IsString(s).Assert().Do();
					(s == String("\"\"")).Evaluate().Do();
					(t == String(" dog")).Evaluate().Do();
					string s_string = s.GetString();
					string t_string = t.GetString();
					Assert::IsTrue(s_string == "\"\"");
					Assert::IsTrue(t_string == " dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("\"C \\\"e\\\" quotes\" dog")).Assert().Do();
					validate.IsString(s).Assert().Do();
					(s == String("\"C \\\"e\\\" quotes\"")).Evaluate().Do();
					(t == String(" dog")).Evaluate().Do();
					string s_string = s.GetString();
					string t_string = t.GetString();
					Assert::IsTrue(s_string == "\"C \\\"e\\\" quotes\"");
					Assert::IsTrue(t_string == " dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("\"E \\ quotes\" dog")).Assert().Do();
					validate.IsString(s).Assert().Do();
					(s == String("\"E \\ quotes\"")).Evaluate().Do();
					(t == String(" dog")).Evaluate().Do();
					string s_string = s.GetString();
					string t_string = t.GetString();
					Assert::IsTrue(s_string == "\"E \\ quotes\"");
					Assert::IsTrue(t_string == " dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("Not a dog")).Assert().Do();
					(!validate.IsString(s)).Assert().Do();
					string s_string = s.DisplayValueSN();
					string s_part = s_string.substr(0, 37 - 5);
					string s_comp = "StringRef(\"Not a dog\"[0.._split_";
					Assert::IsTrue(s_part == s_comp);
					string t_string = t.DisplayValueSN();
					string t_part = t_string.substr(0, 37 - 8);
					string t_comp = "StringRef(\"Not a dog\"[_split_";
					Assert::IsTrue(t_part == t_comp);
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestValidate_IsName)
		{
			Initialize();
			{
				Manager manager("Test Validate IsName", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				manager.SetAutoExpandNull(autoExpand);

				CharacterSet characterSet;
				Validate validate(characterSet);

				(!validate.IsName(String(""))).Assert().Do();
				validate.IsName(String("_MyVariable65")).Assert().Do();
				validate.IsName(String("My_Variable_65")).Assert().Do();
				(!validate.IsName(String("677_M"))).Assert().Do();

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String(" dog")).Assert().Do();
					(!validate.IsName(s)).Assert().Do();
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("_MyVariable65 dog")).Assert().Do();
					validate.IsName(s).Assert().Do();
					(s == String("_MyVariable65")).Evaluate().Do();
					(t == String(" dog")).Evaluate().Do();
					string s_string = s.GetString();
					string t_string = t.GetString();
					Assert::IsTrue(s_string == "_MyVariable65");
					Assert::IsTrue(t_string == " dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("My_Variable_65 dog")).Assert().Do();
					validate.IsName(s).Assert().Do();
					(s == String("My_Variable_65")).Evaluate().Do();
					(t == String(" dog")).Evaluate().Do();
					string s_string = s.GetString();
					string t_string = t.GetString();
					Assert::IsTrue(s_string == "My_Variable_65");
					Assert::IsTrue(t_string == " dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("677_M dog")).Assert().Do();
					(!validate.IsName(s)).Assert().Do();
					string s_string = s.DisplayValueSN();
					string s_part = s_string.substr(0, 37 - 5);
					string s_comp = "StringRef(\"677_M dog\"[0.._split_";
					Assert::IsTrue(s_part == s_comp);
					string t_string = t.DisplayValueSN();
					string t_part = t_string.substr(0, 37 - 8);
					string t_comp = "StringRef(\"677_M dog\"[_split_";
					Assert::IsTrue(t_part == t_comp);
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestValidate_IsWhiteSpaceOnly)
		{
			Initialize();
			{
				Manager manager("Test Validate IsWhiteSpaceOnly", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				manager.SetAutoExpandNull(autoExpand);

				CharacterSet characterSet;
				Validate validate(characterSet);

				(!validate.IsWhiteSpaceOnly(String(""))).Assert().Do();
				validate.IsWhiteSpaceOnly(String(" ")).Assert().Do();
				validate.IsWhiteSpaceOnly(String("\t\n")).Assert().Do();
				validate.IsWhiteSpaceOnly(String(" \t\n")).Assert().Do();
				(!validate.IsWhiteSpaceOnly(String(" \t \r\nXX"))).Assert().Do();
				(!validate.IsWhiteSpaceOnly(String("X   \tX"))).Assert().Do();

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("")).Assert().Do();
					(!validate.IsWhiteSpaceOnly(s)).Assert().Do();
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String(" dog")).Assert().Do();
					validate.IsWhiteSpaceOnly(s).Assert().Do();
					(t == String("dog")).Evaluate().Do();
					string t_string = t.GetString();
					Assert::IsTrue(t_string == "dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String(" \tdog")).Assert().Do();
					validate.IsWhiteSpaceOnly(s).Assert().Do();
					(t == String("dog")).Evaluate().Do();
					string s_string = s.GetString();
					string t_string = t.GetString();
					Assert::IsTrue(s_string == " \t");
					Assert::IsTrue(t_string == "dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("\t\n dog")).Assert().Do();
					validate.IsWhiteSpaceOnly(s).Assert().Do();
					(t == String("dog")).Evaluate().Do();
					string s_string = s.GetString();
					string t_string = t.GetString();
					Assert::IsTrue(s_string == "\t\n ");
					Assert::IsTrue(t_string == "dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("\t \r\ndog")).Assert().Do();
					validate.IsWhiteSpaceOnly(s).Assert().Do();
					(t == String("dog")).Evaluate().Do();
					string s_string = s.GetString();
					string t_string = t.GetString();
					Assert::IsTrue(s_string == "\t \r\n");
					Assert::IsTrue(t_string == "dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("A  \tX dog")).Assert().Do();
					(!validate.IsWhiteSpaceOnly(s)).Assert().Do();
					string s_string = s.DisplayValueSN();
					string s_part = s_string.substr(0, 37 - 4);
					string s_comp = "StringRef(\"A  \\tX dog\"[0.._split_";
					Assert::IsTrue(s_part == s_comp);
					string t_string = t.DisplayValueSN();
					string t_part = t_string.substr(0, 37 - 7);
					string t_comp = "StringRef(\"A  \\tX dog\"[_split_";
					Assert::IsTrue(t_part == t_comp);
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestValidate_IsSimpleComment)
		{
			Initialize();
			{
				Manager manager("Test Validate IsSimpleComment", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				manager.SetAutoExpandNull(autoExpand);

				CharacterSet characterSet;
				Validate validate(characterSet);

				// No closing line feed.
				(!validate.IsSimpleComment(String("/*D"))).Assert().Do();
				(!validate.IsSimpleComment(String("/*Escaped dog*"))).Assert().Do();

				(!validate.IsSimpleComment(String(""))).Assert().Do();
				validate.IsSimpleComment(String("/**/")).Assert().Do();
				validate.IsSimpleComment(String("/*Simple string*/")).Assert().Do();
				validate.IsSimpleComment(String("/*Containing /* */")).Assert().Do();
				validate.IsSimpleComment(String("/*Escaped backslash \\ quotes*/")).Assert().Do();
				(!validate.IsSimpleComment(String("/*Cat */ dog"))).Assert().Do();

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String(" dog")).Assert().Do();
					(!validate.IsSimpleComment(s)).Assert().Do();
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("/**/ dog")).Assert().Do();
					validate.IsSimpleComment(s).Assert().Do();
					(s == String("/**/")).Evaluate().Do();
					(t == String(" dog")).Evaluate().Do();
					string s_string = s.GetString();
					string t_string = t.GetString();
					Assert::IsTrue(s_string == "/**/");
					Assert::IsTrue(t_string == " dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("/*Containing included /* in text*/ dog")).Assert().Do();
					validate.IsSimpleComment(s).Assert().Do();
					(s == String("/*Containing included /* in text*/")).Evaluate().Do();
					(t == String(" dog")).Evaluate().Do();
					string s_string = s.GetString();
					string t_string = t.GetString();
					Assert::IsTrue(s_string == "/*Containing included /* in text*/");
					Assert::IsTrue(t_string == " dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("Not a dog")).Assert().Do();
					(!validate.IsSimpleComment(s)).Assert().Do();
					string s_string = s.DisplayValueSN();
					string s_part = s_string.substr(0, 37 - 5);
					string s_comp = "StringRef(\"Not a dog\"[0.._split_";
					Assert::IsTrue(s_part == s_comp);
					string t_string = t.DisplayValueSN();
					string t_part = t_string.substr(0, 37 - 8);
					string t_comp = "StringRef(\"Not a dog\"[_split_";
					Assert::IsTrue(t_part == t_comp);
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestValidate_IsLineComment)
		{
			Initialize();
			{
				Manager manager("Test Validate TestValidate_IsLineComment", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				manager.SetAutoExpandNull(autoExpand);

				CharacterSet characterSet;
				Validate validate(characterSet);

				// No closing line feed.
				(!validate.IsLineComment(String("//D"))).Assert().Do();
				(!validate.IsLineComment(String("//Escaped // quotes*/"))).Assert().Do();

				(!validate.IsLineComment(String(""))).Assert().Do();
				validate.IsLineComment(String("//\n")).Assert().Do();
				validate.IsLineComment(String("//Simple string*/\n")).Assert().Do();
				validate.IsLineComment(String("//Containing /* */ \n")).Assert().Do();
				validate.IsLineComment(String("//Escaped // quotes\n")).Assert().Do();
				(!validate.IsLineComment(String("//Cat \n dog"))).Assert().Do();

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String(" dog")).Assert().Do();
					(!validate.IsLineComment(s)).Assert().Do();
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("//\n dog")).Assert().Do();
					validate.IsLineComment(s).Assert().Do();
					(s == String("//\n")).Evaluate().Do();
					(t == String(" dog")).Evaluate().Do();
					string s_string = s.GetString();
					string t_string = t.GetString();
					Assert::IsTrue(s_string == "//\n");
					Assert::IsTrue(t_string == " dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("//Containing included /* in text\n dog")).Assert().Do();
					validate.IsLineComment(s).Assert().Do();
					(s == String("//Containing included /* in text\n")).Evaluate().Do();
					(t == String(" dog")).Evaluate().Do();
					string s_string = s.GetString();
					string t_string = t.GetString();
					Assert::IsTrue(s_string == "//Containing included /* in text\n");
					Assert::IsTrue(t_string == " dog");
				}

				{
					SN_LOCAL(s);
					SN_LOCAL(t);
					(s + t == String("Not a dog")).Assert().Do();
					(!validate.IsLineComment(s)).Assert().Do();
					string s_string = s.DisplayValueSN();
					string s_part = s_string.substr(0, 37 - 5);
					string s_comp = "StringRef(\"Not a dog\"[0.._split_";
					Assert::IsTrue(s_part == s_comp);
					string t_string = t.DisplayValueSN();
					string t_part = t_string.substr(0, 37 - 8);
					string t_comp = "StringRef(\"Not a dog\"[_split_";
					Assert::IsTrue(t_part == t_comp);
				}
			}
			Cleanup();
		}

		TEST_METHOD(TestParse_Value)
		{
			Initialize();
			{
				Manager manager("Test Parse Value", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, false);
				manager.SetAutoExpandNull(autoExpand);

				CharacterSet characterSet;
				Validate validate(characterSet);
				Parse parse(characterSet, validate);

				// Name test.
				SN_DOMAIN(MyDomain);
				SN_DECLARE(i1);
				(parse.AsValueRef(MyDomain)(String("Peter_1"))(i1)).Assert().Do();

				SN_DECLARE(j1);
				(j1 == Meta(1, MyDomain["Peter_1"])).Assert().Do();
				(i1 == j1).Evaluate().Do();

				string i1_string = i1.DisplayValueSN();
				string j1_string = j1.DisplayValueSN();
				Assert::IsTrue(i1_string == j1_string);

				// Integer test.
				SN_DECLARE(i2);
				(parse.AsValueRef(MyDomain)(String("543"))(i2)).Assert().Do();

				SN_DECLARE(j2);
				(j2 == Long(543)).Assert().Do();
				(i2 == j2).Evaluate().Do();

				string i2_string = i2.DisplayValueSN();
				string j2_string = j2.DisplayValueSN();
				Assert::IsTrue(i2_string == j2_string);

				// Floating point test.
				SN_DECLARE(i4);
				(parse.AsValueRef(MyDomain)(String("3.1415"))(i4)).Assert().Do();

				SN_DECLARE(j4);
				(j4 == Double(3.1415)).Assert().Do();
				(i4 == j4).Evaluate().Do();

				string i4_string = i4.DisplayValueSN();
				string j4_string = j4.DisplayValueSN();
				Assert::IsTrue(i4_string == j4_string);

				// String
				SN_DECLARE(i3);
				(parse.AsValueRef(MyDomain)(String("\"My test string\""))(i3)).Assert().Do();

				SN_DECLARE(j3);
				(j3 == String("My test string")).Assert().Do();
				(i3 == j3).Evaluate().Do();

				string i3_string = i3.DisplayValueSN();
				string j3_string = j3.DisplayValueSN();
				Assert::IsTrue(i3_string == j3_string);


				// String sub string
				SN_DECLARE(s1);
				(parse.AsValueRef(MyDomain)(String("\"My test \\\"string\\\"\""))(s1)).Assert().Do();

				SN_DECLARE(s2);
				(s2 == String("My test \"string\"")).Assert().Do();
				(s1 == s2).Evaluate().Do();

				string s1_string = s1.DisplayValueSN();
				string s2_string = s2.DisplayValueSN();
				Assert::IsTrue(s1_string == s2_string);
			}
		}

		TEST_METHOD(TestParse_Value_Exponentials)
		{
			return; // to_string does not use scientific notation.

			Initialize();
			{
				Manager manager("Test Parse Value Exponentials", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, false);
				manager.SetAutoExpandNull(autoExpand);

				CharacterSet characterSet;
				Validate validate(characterSet);
				Parse parse(characterSet, validate);

				// Exponential test.
				SN_DOMAIN(MyDomain);
				SN_DECLARE(k5);
				(parse.AsNumber(String("6.626176e-34"))(k5)).Assert().Do();

				SN_DECLARE(i5);
				(parse.AsValueRef(MyDomain)(String("-6.626176e-34"))(i5)).Assert().Do();

				SN_DECLARE(j5);
				(j5 == Double(6.626176e-34)).Assert().Do();

				string i5_string = i5.DisplayValueSN();
				string k5_string = k5.DisplayValueSN();
				string j5_string = j5.DisplayValueSN();
				Assert::IsTrue(i5_string == j5_string);
				Assert::IsTrue(k5_string == "-" + j5_string);

				(i5 == j5).Evaluate().Do();
				(k5 == -j5).Evaluate().Do();
			}
		}

		TEST_METHOD(TestParse_SimpleExpression1)
		{
			Initialize();
			{
				Manager manager("Test Parse Simple Expression1", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, runWebServer);
				manager.SetAutoExpandNull(autoExpand);

				CharacterSet characterSet;
				Validate validate(characterSet);
				Parse parse(characterSet, validate);

				SN_DOMAIN(MyDomain);

				// Arithmetic Expression.
				SN_DECLARE(i1);

				(parse.AsArithmeticExpression(MyDomain)(String("13+21"))(i1)).Assert().Do();

				SN_DECLARE(j1);
				(j1 == Meta(1, Meta(-1, Long(13)) + Meta(-1, Long(21)))).Assert().Do();

				string i1_string = i1.DoEvaluate(0).DisplayValueSN();
				string i1_value = i1.DoEvaluate(-1).DisplayValueSN();

				string j1_string = j1.DoEvaluate(0).DisplayValueSN();
				string j1_value = j1.DoEvaluate(-1).DisplayValueSN();

				Assert::IsTrue(i1_string == j1_string);
				Assert::IsTrue(i1_value == j1_value);

				(i1 == j1).Evaluate().Do();
			}
		}

		TEST_METHOD(TestParse_SimpleExpression2)
		{
			Initialize();
			{
				Manager manager("Test Parse Simple Expression2", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, false);
				manager.SetAutoExpandNull(autoExpand);

				CharacterSet characterSet;
				Validate validate(characterSet);
				Parse parse(characterSet, validate);

				SN_DOMAIN(MyDomain);

				// Boolean Expression.
				SN_DECLARE(i2);
				(parse.AsBooleanExpression(MyDomain)(String("13+21"))(i2)).Assert().Do();

				SN_DECLARE(j2);
				(j2 == Meta(1, Meta(-1, Long(13)) + Meta(-1, Long(21)))).Assert().Do();

				string i2_string = i2.DoEvaluate(0).DisplayValueSN();
				string i2_value = i2.DoEvaluate(-1).DisplayValueSN();

				string j2_string = j2.DoEvaluate(0).DisplayValueSN();
				string j2_value = j2.DoEvaluate(-1).DisplayValueSN();

				Assert::IsTrue(i2_string == j2_string);
				Assert::IsTrue(i2_value == j2_value);

				(i2 == j2).Evaluate().Do();

				if (false)
				{
					// Expression.
					SN_DECLARE(i3);
					(parse.AsExpression(MyDomain)(String("13+21"))(i2)).Assert().Do();

					SN_DECLARE(j3);
					(j3 == Meta(1, Meta(-1, Long(13)) + Meta(-1, Long(21)))).Assert().Do();

					string i3_string = i3.DoEvaluate(0).DisplayValueSN();
					string i3_value = i3.DoEvaluate(-1).DisplayValueSN();

					string j3_string = j3.DoEvaluate(0).DisplayValueSN();
					string j3_value = j3.DoEvaluate(-1).DisplayValueSN();

					Assert::IsTrue(i3_string == j3_string);
					Assert::IsTrue(i3_value == j3_value);

					(i3 == j3).Evaluate().Do();
				}
			}
		}

		TEST_METHOD(TestParse_SimpleExpression3)
		{
			Initialize();
			{
				Manager manager("Test Parse Simple Expression3", AssertErrorHandler);
				manager.StartWebServer(skynet::StepInto, "0.0.0.0", "80", doc_root, false);
				manager.SetAutoExpandNull(autoExpand);

				CharacterSet characterSet;
				Validate validate(characterSet);
				Parse parse(characterSet, validate);

				SN_DOMAIN(MyDomain);

				// Expression.
				SN_DECLARE(i3);
				(parse.AsExpression(MyDomain)(String("13+21"))(i3)).Assert().Do();

				SN_DECLARE(j3);
				(j3 == Meta(1, Meta(-1, Long(13)) + Meta(-1, Long(21)))).Assert().Do();

				string i3_string = i3.DoEvaluate(0).DisplayValueSN();
				string i3_value = i3.DoEvaluate(-1).DisplayValueSN();

				string j3_string = j3.DoEvaluate(0).DisplayValueSN();
				string j3_value = j3.DoEvaluate(-1).DisplayValueSN();

				Assert::IsTrue(i3_string == j3_string);
				Assert::IsTrue(i3_value == j3_value);

				(i3 == j3).Evaluate().Do();
			}
		}
	};
}