#pragma once

#include "pgc.h"
#include "testclassusingref_A.h"

using namespace PGCX;

class TestClassUsingRef_A;

class TestClassUsingRef_B
    : public PGC::PGC_Base
{
    PGC_CLASS(TestClassUsingRef_B)

private:
    std::string                     m_Description = std::string();
    PGCX::Ref<TestClassUsingRef_A>  m_TestA;
    PGCX::Ref<TestClassUsingRef_B>  m_TestB;
    PGCX::Ref<TestClassUsingRef_B>  m_Next;

public:
    // no copying
    TestClassUsingRef_B(const TestClassUsingRef_B&) = delete;
    TestClassUsingRef_B& operator=(const TestClassUsingRef_B&) = delete;

    // yes moving
    TestClassUsingRef_B(TestClassUsingRef_B&& p_Other) noexcept
        : m_Description(p_Other.m_Description+"_copy")
        , m_TestA(std::move(p_Other.m_TestA))
        , m_TestB(std::move(p_Other.m_TestB))
        , m_Next(std::move(p_Other.m_Next))
    {
        Initialize();
    };
    TestClassUsingRef_B& operator=(TestClassUsingRef_B&&) noexcept = default;

    void PromoteMembers() {}

    virtual PGC::PGC_Base* MoveTo(void* memory) override
    {
        return ::new (memory) TestClassUsingRef_B(std::move(*this));
    }

public:
    TestClassUsingRef_A* GetTestA() {
        return m_TestA.Get();
    }

    void SetTestA(TestClassUsingRef_A* p_Ptr) {
        m_TestA.Set(p_Ptr);
    }

    TestClassUsingRef_B* GetTestB() {
        return m_TestB.Get();
    }

    void SetTestB(TestClassUsingRef_B* p_Ptr) {
        m_TestB.Set(p_Ptr);
    }

    TestClassUsingRef_B* GetNext() {
        return m_Next.Get();
    }

    void SetNext(TestClassUsingRef_B* p_Ptr) {
        m_Next.Set(p_Ptr);
    }

    TestClassUsingRef_B();
    void Initialize();
    virtual ~TestClassUsingRef_B();

	std::string GetDescription() const;
	void SetDescription(const std::string& p_Description);

public:
	static long m_ActiveCount;
};