#include "testpgc_a.h"
#include "pgc_transaction.h"
#include <string>

#include "test_pgc_pch.h"

/*static*/ long TestPGC_A::m_ActiveCount = 0;

TestPGC_A::TestPGC_A()
	: Base()
	, m_next(nullptr)
{
	++m_ActiveCount;
}

TestPGC_A::TestPGC_A(const TestPGC_A& other)
	: Base(other)
	, m_next(other.m_next)
	, m_Description(other.m_Description)
{
	++m_ActiveCount;
}

TestPGC_A::~TestPGC_A()
{
	--m_ActiveCount;
}

string TestPGC_A::GetDescription()
{
	return m_Description;
}

void TestPGC_A::SetDescription(const string &p_Description)
{
	m_Description = p_Description;
}

TestPGC_A *TestPGC_A::GetNext()
{
	return m_next;
}

void TestPGC_A::SetNext(TestPGC_A *p_next)
{
	m_next = p_next;
	REQUESTPROMOTION(m_next);
}

TestPGC_A* TestPGC_A::CloneTo(void* memory) const
{
	if (g_ForceMemcpyFallback)
		return nullptr;
	return new (memory) TestPGC_A(*this);
}


void TestPGC_A::PromoteMembers()
{
	REQUESTPROMOTION(m_next);
}

long TestPGC_A::CountList()
{
    if (m_next)
    {
        return m_next->CountList() + 1;
    }
    return 0;
}

