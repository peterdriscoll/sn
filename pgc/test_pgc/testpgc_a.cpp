#include "testpgc_a.h"
#include "pgc_transaction.h"
#include <string>

#include "test_pgc_pch.h"

/*static*/ long TestPgc_A::m_ActiveCount = 0;

TestPgc_A::TestPgc_A()
: m_next(0)
{
	++m_ActiveCount;
}

TestPgc_A::~TestPgc_A()
{
	--m_ActiveCount;
}

string TestPgc_A::GetDescription()
{
	return m_Description;
}

void TestPgc_A::SetDescription(const string &p_Description)
{
	m_Description = p_Description;
}

TestPgc_A *TestPgc_A::GetNext()
{
	return m_next;
}

void TestPgc_A::SetNext(TestPgc_A *p_next)
{
	m_next = p_next;
	REQUESTPROMOTION(m_next);
}

void TestPgc_A::PromoteMembers()
{
	REQUESTPROMOTION(m_next);
}

long TestPgc_A::CountList()
{
    if (m_next)
    {
        return m_next->CountList() + 1;
    }
    return 0;
}

Base *TestPgc_A::Clone(Transaction & /*p_Transaction*/)
{
	return NULL;
}

