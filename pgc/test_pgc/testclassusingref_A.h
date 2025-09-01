#pragma once

#include "pgc.h"

using namespace PGCX;

class TestClassUsingRef_B;

class TestClassUsingRef_A
    : public PGC::PGC_Base
{
    PGC_CLASS(TestClassUsingRef_A)

private:
    std::string                     m_Description = std::string();
    PGCX::Ref<TestClassUsingRef_A>  m_TestA;
    PGCX::Ref<TestClassUsingRef_B>  m_TestB;
    PGCX::Ref<TestClassUsingRef_A>  m_Next;

public:
    // no copying
    TestClassUsingRef_A(const TestClassUsingRef_A&) = delete;
    TestClassUsingRef_A& operator=(const TestClassUsingRef_A&) = delete;

    // yes moving
    TestClassUsingRef_A(TestClassUsingRef_A&& p_Other) noexcept
        : m_Description(std::move(p_Other.m_Description))
        , m_TestA(std::move(p_Other.m_TestA))
        , m_TestB(std::move(p_Other.m_TestB))
        , m_Next(std::move(p_Other.m_Next))
    {
        Initialize();
    };

    TestClassUsingRef_A& operator=(TestClassUsingRef_A&&) noexcept = default;

    void PromoteMembers() {}

    virtual PGC::PGC_Base* MoveTo(void* memory) override
    {
        return ::new (memory) TestClassUsingRef_A(std::move(*this));
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

    TestClassUsingRef_A* GetNext() {
        return m_Next.Get();
    }

    void SetNext(TestClassUsingRef_A* p_Ptr) {
        m_Next.Set(p_Ptr);
    }

    TestClassUsingRef_A();
    void Initialize();
    virtual ~TestClassUsingRef_A();

	std::string GetDescription() const;
	void SetDescription(const std::string& p_Description);

public:
	static long m_ActiveCount;
};

