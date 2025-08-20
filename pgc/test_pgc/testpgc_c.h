#pragma once

#if !defined(TESTPGC_C_H_INCLUDED)
#define TESTPGC_C_H_INCLUDED

#include "pgc.h"
#include <string>
#include <vector>


using namespace PGCX;

class TestPGC_A;
class TestPGC_C;

#undef PGC_ACTION_OVER_VALUE_MEMBERS
#undef PGC_ACTION_OVER_MEMBER_REFS
#undef PGC_ACTION_OVER_MEMBER_CONTAINER_REFS

#define PGC_ACTION_OVER_VALUE_MEMBERS(ACTION) \
    ACTION(Length, long, 0)

#define PGC_ACTION_OVER_MEMBER_REFS(ACTION) \
    ACTION(CMember, TestPGC_C) \
    ACTION(AMember, TestPGC_A)

#define PGC_ACTION_OVER_MEMBER_CONTAINER_REFS(ACTION) \
    ACTION(ACollection, PGC::vector_ref, TestPGC_A)

class TestPGC_C : public Base
{
	PGC_CLASS(TestPGC_C);

    //	PGC_MEMBER_DEFINITIONS(TestPGC_C, Base)
private:
    long                              m_Length{ 0 };
    PGCX::MemberRef<TestPGC_C>        m_CMember;
    PGCX::MemberRef<TestPGC_A>        m_AMember;
    PGCX::vector_ref<TestPGC_A>       m_ACollection;

public:
    TestPGC_C* GetCMember() {
        return m_CMember.Get();
    }

    void SetCMember(TestPGC_C* p_Ptr) {
        m_CMember.Set(p_Ptr, GetTransaction());
    }

    TestPGC_A* GetAMember() {
        return m_AMember.Get();
    }

    void SetAMember(TestPGC_A* p_Ptr) {
        m_AMember.Set(p_Ptr, GetTransaction());
    }

    void PromoteMembers() {
        m_CMember.PromoteNow(GetTransaction());
        m_AMember.PromoteNow(GetTransaction());
        m_ACollection.PromoteAll();
    }

    void RegisterMembers() {
        RegisterMember(reinterpret_cast<PGC::PGC_Base*>(m_CMember.Get()));
        RegisterMember(reinterpret_cast<PGC::PGC_Base*>(m_AMember.Get()));
    }

    TestPGC_C(const TestPGC_C& other)
        : Base(other)
        , m_Length(other.m_Length)
        , m_CMember(other.m_CMember)
        , m_AMember(other.m_AMember)
        , m_ACollection(other.m_ACollection)
    {
        m_CMember.RequestPromotion(GetTransaction());
        m_AMember.RequestPromotion(GetTransaction());
        m_ACollection.PromoteAll();
    }

    TestPGC_C(TestPGC_C&& other) noexcept
        : Base(std::move(other))
        , m_Length(other.m_Length)
        , m_CMember(std::move(other.m_CMember))
        , m_AMember(std::move(other.m_AMember))
        , m_ACollection(std::move(other.m_ACollection))
    {
        m_CMember.RequestPromotion(GetTransaction());
        m_AMember.RequestPromotion(GetTransaction());
        m_ACollection.PromoteAll();
    }

    PGC::PGC_Base* CloneTo(void* memory) const override {
        return ::new (memory) TestPGC_C(*this);
    }

    PGC::PGC_Base* MoveTo(void* memory) override {
        return ::new (memory) TestPGC_C(std::move(*this));
    }
    //	End of PGC_MEMBER_DEFINITIONS(TestPGC_C, Base)

public:
    TestPGC_C();
    virtual ~TestPGC_C();

    long GetLength();
    void SetLength(long p_Value);
};

#endif // !defined(TESTPGC_C_H_INCLUDED)
