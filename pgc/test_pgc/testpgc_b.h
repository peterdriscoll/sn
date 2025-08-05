#if !defined(TESTPGC_B_H_INCLUDED)
#define TESTPGC_B_H_INCLUDED

#pragma once

#include "pgc.h"

using namespace PGCX;

class TestPGC_A;

#undef PGC_ACTION_OVER_MEMBERS
#undef PGC_ACTION_OVER_CONTAINERS

#define PGC_ACTION_OVER_MEMBERS(ACTION) \
    ACTION(TestA, TestPGC_A)

#define PGC_ACTION_OVER_CONTAINERS(ACTION)

class TestPGC_B : public Base
{
    PGC_CLASS(TestPGC_B)

//	PGC_MEMBER_DEFINITIONS(TestPGC_B)
private:
    PGC_DEFINE_MEMBERS(TestPGC_B)
public:
    PGC_MEMBER_ACCESSORS(TestPGC_B)
        PGC_PROMOTION_LOGIC(TestPGC_B)
        PGC_REGISTRATION_LOGIC(TestPGC_B)
        //PGC_CLONE_TO(TestPGC_B)
        virtual TestPGC_B* CloneTo(void* mem) const override;

/*
private:
    MemberRef<TestPGC_A> m_TestA;

public:
    TestPGC_A* GetTestA()
    {
        return m_TestA.Get();
    }

    void SetTestA(TestPGC_A* p_Pointer)
    {
        m_TestA.Set(p_Pointer, GetTransaction());
    }

    void PromoteMembers()
    {
        m_TestA.RequestPromotion(GetTransaction());
    }

    void RegisterMembers()
    {
        RegisterMember((PGC::PGC_Base*)m_TestA.Get());
    }

    TestPGC_B(const TestPGC_B& other)
        : Base(other)
        , m_TestA(other.m_TestA)
    {
        m_TestA.RequestPromotion(GetTransaction());
    }

    virtual PGC_Base* CloneTo(void* memory) const override
    {
        return new (memory) TestPGC_B(*this);
    }
*/

public:
	TestPGC_B();
	TestPGC_B(const TestPGC_B& other);
	virtual ~TestPGC_B();

    void SimulateLegacyPromotionOnA();

public:
	static long m_ActiveCount;
};

#endif // !defined(TESTPGC_B_H_INCLUDED)
