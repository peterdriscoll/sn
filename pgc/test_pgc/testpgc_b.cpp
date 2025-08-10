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
	, m_Next(other.m_Next)
	, m_Description(other.m_Description)	
{
	++m_ActiveCount;
	//m_TestA.RequestPromotion(GetTransaction());
	//m_Next.RequestPromotion(GetTransaction());
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

void TestPGC_B::SimulateLegacyPromotionOnA()
{
	// Simulate double promotion
	m_TestA.RequestPromotion(GetTransaction());

	// Simulate legacy-style call on double promotion
	// RequestPromotion((PGC::PGC_TypeCheck**)&m_TestA);

}

std::string TestPGC_B::GetDescription() const
{
	return m_Description;
}	

void TestPGC_B::SetDescription(const std::string& p_Description)
{
	m_Description = p_Description;
}