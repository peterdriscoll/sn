#include "testpgc_b.h"
#include "testpgc_a.h"

#include "test_pgc_pch.h"

/*static*/ long TestPgc_B::m_ActiveCount = 0;

TestPgc_B::TestPgc_B()
: m_TestA(0)
{
	++m_ActiveCount;
}

TestPgc_B::~TestPgc_B()
{
	--m_ActiveCount;
}

Ref<TestPgc_A> TestPgc_B::GetTestA()
{
	return m_TestA;
}

void TestPgc_B::SetTestA(Ref<TestPgc_A> newVal)
{
	m_TestA = newVal;
	REQUESTPROMOTION(m_TestA);
}

void TestPgc_B::PromoteMembers()
{
	REQUESTPROMOTION(m_TestA);
}

Base *TestPgc_B::Clone(Transaction & /*p_Transaction*/)
{
	return NULL;
}

