#ifndef TestPGC_H
#define TestPGC_H

#include "cppunit/extensions/HelperMacros.h"
#include "testpgc_a.h"
#include "testpgc_b.h"

class TestPGC : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(TestPGC);
		CPPUNIT_TEST(TestBasicPGC);
		CPPUNIT_TEST(TestRecursivePGC);
        CPPUNIT_TEST(TestRecursiveKeepPGC);
        CPPUNIT_TEST(TestReferenceCyclePGC);
	CPPUNIT_TEST_SUITE_END();

protected:
	void TestBasicPGC();
    void TestRecursivePGC();
    void TestRecursiveKeepPGC();
    void TestReferenceCyclePGC();

    void RecursivelyDoThis(long p_Depth);
    void RecursivelyDoThat(long p_Depth);
    void RecursivelyDoKeep(TestPgc_A *keep, long p_Depth);

public:
	virtual void setUp();
	virtual void tearDown();
};

#endif // TestPGC_H
