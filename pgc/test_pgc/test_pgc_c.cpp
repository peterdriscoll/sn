#include "test_pgc_pch.h"
#include "test_pgc_c.h"

Test_PGC_C::Test_PGC_C()
{
}

Test_PGC_C::~Test_PGC_C()
{
}

long Test_PGC_C::GetLength()
{
	return m_Length;
}

void Test_PGC_C::SetLength(long p_Value)
{
	m_Length = p_Value;
}
