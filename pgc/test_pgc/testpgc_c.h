#pragma once

#if !defined(TESTPGC_C_H_INCLUDED)
#define TESTPGC_C_H_INCLUDED

#include "pgc.h"
#include <string>
#include <vector>


using namespace PGCX;

class TestPGC_A;
class TestPGC_C;

#undef PGC_ACTION_OVER_MEMBERS
#undef PGC_ACTION_OVER_CONTAINERS

#define PGC_ACTION_OVER_MEMBERS(ACTION) \
    ACTION(CMember, TestPGC_C) \
    ACTION(AMember, TestPGC_A)

#define PGC_ACTION_OVER_CONTAINERS(ACTION) \
    ACTION(ACollection, std::vector, TestPGC_A)

class TestPGC_C : public Base
{
	PGC_CLASS(TestPGC_C);

public:
	TestPGC_C();
	virtual ~TestPGC_C();

    long GetLength();
    void SetLength(long p_Value);

//	PGC_MEMBER_DEFINITIONS(TestPGC_C)
// Member and container definitions (private)
private:
    MemberRef<TestPGC_C> m_CMember;
    MemberRef<TestPGC_A> m_AMember;
    std::vector<MemberRef<TestPGC_A>> m_ACollection;

    // Public accessors and helpers
public:
    TestPGC_C* GetCMember()
    {
        return m_CMember.Get();
    }
    void SetCMember(TestPGC_C* p_Pointer)
    {
        m_CMember.Set(p_Pointer, GetTransaction());
    }

    TestPGC_A* GetAMember()
    {
        return m_AMember.Get();
    }
    void SetAMember(TestPGC_A* p_Pointer)
    {
        m_AMember.Set(p_Pointer, GetTransaction());
    }

    void RequestPromotionACollection()
    {
        for (auto& _ref : m_ACollection)
            _ref.RequestPromotion(GetTransaction());
    }

    void PromoteMembers()
    {
        m_CMember.RequestPromotion(GetTransaction());
        m_AMember.RequestPromotion(GetTransaction());
        for (auto& _ref : m_ACollection)
            _ref.RequestPromotion(GetTransaction());
    }

    void RegisterMembers()
    {
        RegisterMember((PGC::PGC_Base*)m_CMember.Get());
        RegisterMember((PGC::PGC_Base*)m_AMember.Get());
    }

    TestPGC_C(const TestPGC_C& other)
        : Base(other),
        m_CMember(other.m_CMember),
        m_AMember(other.m_AMember),
        m_ACollection(other.m_ACollection),
        m_Length(other.m_Length)
    {
        m_CMember.RequestPromotion(GetTransaction());
        m_AMember.RequestPromotion(GetTransaction());
        for (auto& _ref : m_ACollection)
            _ref.RequestPromotion(GetTransaction());
    }

    virtual PGC_Base* CloneTo(void* memory) const override {
        return new (memory) TestPGC_C(*this);
    }

private:
	long m_Length;
};

#endif // !defined(TESTPGC_C_H_INCLUDED)
