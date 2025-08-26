#include "testclassusingref_A.h"
#include "test_pgc_pch.h"

/*static*/ long TestClassUsingRef_A::m_ActiveCount = 0;

TestClassUsingRef_A::TestClassUsingRef_A()
{
	Initialize();
}

void TestClassUsingRef_A::Initialize()
{
	++m_ActiveCount;
}

TestClassUsingRef_A::~TestClassUsingRef_A()
{
	--m_ActiveCount;
}

std::string TestClassUsingRef_A::GetDescription() const
{
	return m_Description;
}	

void TestClassUsingRef_A::SetDescription(const std::string& p_Description)
{
	m_Description = p_Description;
}