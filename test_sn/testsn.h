#ifndef TestSN::SN_H
#define TestSN::SN_H

#include "cppunit/extensions/HelperMacros.h"

class TestSN : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(TestSN);
/*
		CPPUNIT_TEST(TestStringEquivalent);

		CPPUNIT_TEST(TestStringEvaluate);

	    CPPUNIT_TEST(TestStringPartialEvaluate);
		CPPUNIT_TEST(TestStringAssert);
	//	CPPUNIT_TEST(TestStringPartialAssert);

		CPPUNIT_TEST(TestBoolEquivalent);
		CPPUNIT_TEST(TestBoolEvaluate);
		CPPUNIT_TEST(TestBoolPartialEvaluate);
		CPPUNIT_TEST(TestBoolAssert);
	//	CPPUNIT_TEST(TestBoolPartialAssert);

		CPPUNIT_TEST(TestDoubleEquivalent);
		CPPUNIT_TEST(TestDoubleEvaluate);
		CPPUNIT_TEST(TestDoublePartialEvaluate);
		CPPUNIT_TEST(TestDoubleAssert);

		CPPUNIT_TEST(TestLongEvaluate);
		CPPUNIT_TEST(TestLongAssert);

		CPPUNIT_TEST(TestFunctionDefinition);
*/
        CPPUNIT_TEST(TestRecursiveFunctionDefinition);
/*
		CPPUNIT_TEST(TestStringRefDefinition);

        CPPUNIT_TEST(TestParsingExamples);
        CPPUNIT_TEST(TestParser);

        CPPUNIT_TEST(TestMetaEvaluate);
		CPPUNIT_TEST(TestMetaPartialEvaluateOnValues);
		CPPUNIT_TEST(TestMetaPartialEvaluate);

		CPPUNIT_TEST(TestValueSetEvaluate);
		CPPUNIT_TEST(TestValueSetEquivalencyEvaluate);

		CPPUNIT_TEST(TestSetEvaluate);
		CPPUNIT_TEST(TestSetAssert);
*/
    CPPUNIT_TEST_SUITE_END();

protected:
	void TestStringEquivalent();
	void TestStringEvaluate();
	void TestStringPartialEvaluate();
	void TestStringAssert();
	void TestStringPartialAssert();

	void TestBoolEquivalent();
	void TestBoolEvaluate();
	void TestBoolPartialEvaluate();
	void TestBoolAssert();
	void TestBoolPartialAssert();
	
	void TestDoubleEquivalent();
	void TestDoubleEvaluate();
	void TestDoublePartialEvaluate();
	void TestDoubleAssert();

	void TestLongEvaluate();
	void TestLongAssert();

	void TestFunctionDefinition();
	void TestRecursiveFunctionDefinition();
	void TestStringRefDefinition();

	void TestParsingExamples();
    void TestParser();

	void TestMetaEvaluate();
	void TestMetaPartialEvaluateOnValues();
	void TestMetaPartialEvaluate();

	void TestValueSetEvaluate();
	void TestValueSetEquivalencyEvaluate();

	void TestSetEvaluate();
	void TestSetAssert();

public:
	virtual void setUp();
	virtual void tearDown();
};

#endif // TestSN::SN_H
