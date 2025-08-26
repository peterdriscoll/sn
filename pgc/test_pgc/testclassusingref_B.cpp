#include "testclassusingref_B.h"
#include "test_pgc_pch.h"

/*static*/ long TestClassUsingRef_B::m_ActiveCount = 0;

TestClassUsingRef_B::TestClassUsingRef_B()
{
	Initialize();
}

void TestClassUsingRef_B::Initialize()
{
	++m_ActiveCount;
}

TestClassUsingRef_B::~TestClassUsingRef_B()
{
	--m_ActiveCount;
}

std::string TestClassUsingRef_B::GetDescription() const
{
	return m_Description;
}	

void TestClassUsingRef_B::SetDescription(const std::string& p_Description)
{
	m_Description = p_Description;
}