#pragma once

#include "pgc.h"

using namespace PGCX;

class TestPGC_A;

#undef PGC_ACTION_OVER_VALUE_MEMBERS
#undef PGC_ACTION_OVER_MEMBER_REFS
#undef PGC_ACTION_OVER_MEMBER_CONTAINER_REFS

#define PGC_ACTION_OVER_VALUE_MEMBERS(ACTION) \
    ACTION(Description, std::string, std::string()) \

#define PGC_ACTION_OVER_MEMBER_REFS(ACTION) \
    ACTION(TestA, TestPGC_A) \
    ACTION(Next, TestClassUsingRef_B)

#define PGC_ACTION_OVER_MEMBER_CONTAINER_REFS(ACTION)

class TestClassUsingRef_B
    : public Base
{
    PGC_CLASS(TestClassUsingRef_B)

	//PGC_MEMBER_DEFINITIONS(TestClassUsingRef_B, Base, Initialize()
private:
    std::string                     m_Description = std::string();
    PGCX::Ref<TestPGC_A>            m_TestA;
    PGCX::Ref<TestClassUsingRef_B>  m_TestB;
    PGCX::Ref<TestClassUsingRef_B>  m_Next;

public:
    // no copying
    TestClassUsingRef_B(const TestClassUsingRef_B&) = delete;
    TestClassUsingRef_B& operator=(const TestClassUsingRef_B&) = delete;

    // yes moving
    TestClassUsingRef_B(TestClassUsingRef_B&&) noexcept = default;
    TestClassUsingRef_B& operator=(TestClassUsingRef_B&&) noexcept = default;

    void PromoteMembers() {
        m_TestA.PromoteNow();
        m_Next.PromoteNow();
    }

    virtual PGC::PGC_Base* MoveTo(void* memory) override
    {
        return ::new (memory) TestClassUsingRef_B(std::move(*this));
    }

    TestPGC_A* GetTestA() {
        return m_TestA.Get();
    }

    void SetTestA(TestPGC_A* p_Ptr) {
        m_TestA.Set(p_Ptr);
    }

    TestClassUsingRef_B* GetNext() {
        return m_Next.Get();
    }

    void SetNext(TestClassUsingRef_B* p_Ptr) {
        m_Next.Set(p_Ptr);
    }

public:
    TestClassUsingRef_B();
    void Initialize();
    virtual ~TestClassUsingRef_B();

    void SimulateLegacyPromotionOnA();

	std::string GetDescription() const;
	void SetDescription(const std::string& p_Description);

public:
	static long m_ActiveCount;
};