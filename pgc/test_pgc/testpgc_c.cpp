#include "test_pgc_pch.h"
#include "TestPGC_C.h"

TestPGC_C::TestPGC_C()
{
}

TestPGC_C::~TestPGC_C()
{
}

long TestPGC_C::GetLength()
{
	return m_Length;
}

void TestPGC_C::SetLength(long p_Value)
{
	m_Length = p_Value;
}
