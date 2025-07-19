#include "testpgc_b.h"
#include "test_pgc_pch.h"

/*static*/ long TestPGC_B::m_ActiveCount = 0;

TestPGC_B::TestPGC_B()
{
	++m_ActiveCount;
}

TestPGC_B::TestPGC_B(const TestPGC_B& other)
	: Base(other)
	, m_TestA(other.m_TestA)
{
	++m_ActiveCount;
	m_TestA.RequestPromotion(GetTransaction());
}

TestPGC_B::~TestPGC_B()
{
	--m_ActiveCount;
}

TestPGC_B* TestPGC_B::CloneTo(void* mem) const
{
//	if (g_ForceMemcpyFallback)
//		return nullptr;
	return new (mem) TestPGC_B(*this);
}
