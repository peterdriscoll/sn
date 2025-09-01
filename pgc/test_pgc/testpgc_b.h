#pragma once

#include "pgc.h"
#include "pgc_base.h"

using namespace PGCX;

class TestPGC_A;

#undef PGC_ACTION_OVER_VALUE_MEMBERS
#undef PGC_ACTION_OVER_MEMBER_REFS
#undef PGC_ACTION_OVER_MEMBER_CONTAINER_REFS

#define PGC_ACTION_OVER_VALUE_MEMBERS(ACTION) \
    ACTION(Description, std::string, std::string()) \

#define PGC_ACTION_OVER_MEMBER_REFS(ACTION) \
    ACTION(TestA, TestPGC_A) \
    ACTION(Next, TestPGC_B)

#define PGC_ACTION_OVER_MEMBER_CONTAINER_REFS(ACTION)

class TestPGC_B : public PGC::PGC_Base
{
    PGC_CLASS(TestPGC_B)

	//PGC_MEMBER_DEFINITIONS(TestPGC_B, Base, Initialize()
private:
    std::string                 m_Description = std::string();
    PGCX::MemberRef<TestPGC_A>  m_TestA;
    PGCX::MemberRef<TestPGC_B>  m_Next;

public:
    TestPGC_A* GetTestA() {
        return m_TestA.Get();
    }

    void SetTestA(TestPGC_A* p_Ptr) {
        m_TestA.Set(p_Ptr, GetTransaction());
    }

    TestPGC_B* GetNext() {
        return m_Next.Get();
    }

    void SetNext(TestPGC_B* p_Ptr) {
        m_Next.Set(p_Ptr, GetTransaction());
    }

    void PromoteMembers() {
        m_TestA.PromoteNow(GetTransaction());
        m_Next.PromoteNow(GetTransaction());
    }

    void RegisterMembers() {
        RegisterMember(reinterpret_cast<PGC::PGC_Base*>(m_TestA.Get()));
        RegisterMember(reinterpret_cast<PGC::PGC_Base*>(m_Next.Get()));
    }

    TestPGC_B(const TestPGC_B& other)
        : Base(other)
        , m_Description(other.m_Description)
        , m_TestA(other.m_TestA)
        , m_Next(other.m_Next)
    {
        Initialize();
    }

    TestPGC_B(TestPGC_B&& other) noexcept
        : Base(std::move(other))
        , m_Description(std::move(other.m_Description))
        , m_TestA(std::move(other.m_TestA))
        , m_Next(std::move(other.m_Next))
    {
        Initialize();
    }

    virtual PGC::PGC_Base* CloneTo(void* memory) const override {
        return ::new (memory) TestPGC_B(*this);
    }

    virtual PGC::PGC_Base* MoveTo(void* memory) override
    {
        return ::new (memory) TestPGC_B(std::move(*this));
    }
    //End of PGC_MEMBER_DEFINITIONS(TestPGC_B, Base, Initialize())
public:
    TestPGC_B();
    void Initialize();
    virtual ~TestPGC_B();

    void SimulateLegacyPromotionOnA();

	std::string GetDescription() const;
	void SetDescription(const std::string& p_Description);

public:
	static long m_ActiveCount;
};