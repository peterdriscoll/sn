#if !defined(TESTPGC_B_H_INCLUDED)
#define TESTPGC_B_H_INCLUDED

#pragma once

#include "pgc.h"

using namespace PGCX;

class TestPGC_A;

#undef PGC_ACTION_OVER_MEMBERS
#undef PGC_ACTION_OVER_CONTAINERS

#define PGC_ACTION_OVER_MEMBERS(ACTION) \
    ACTION(TestA, TestPGC_A) \
    ACTION(Next, TestPGC_B)

#define PGC_ACTION_OVER_CONTAINERS(ACTION)

class TestPGC_B : public Base
{
    PGC_CLASS(TestPGC_B)

//	PGC_MEMBER_DEFINITIONS(TestPGC_B)
/*
private:
    PGC_DEFINE_MEMBERS(TestPGC_B)
public:
    PGC_MEMBER_ACCESSORS(TestPGC_B)
        PGC_PROMOTION_LOGIC(TestPGC_B)
        PGC_REGISTRATION_LOGIC(TestPGC_B)
        //PGC_CLONE_TO(TestPGC_B)
*/

        virtual TestPGC_B* CloneTo(void* mem) const override;

private:
    // PGC_DEFINE_MEMBERS(TestPGC_B)
    PGCX::MemberRef<TestPGC_A> m_TestA;
    PGCX::MemberRef<TestPGC_B> m_Next;

public:
    // PGC_MEMBER_ACCESSORS(TestPGC_B)
    TestPGC_A* GetTestA() { return m_TestA.Get(); }
    void SetTestA(TestPGC_A* p_Pointer) { m_TestA.Set(p_Pointer, GetTransaction()); }

    TestPGC_B* GetNext() { return m_Next.Get(); }
    void SetNext(TestPGC_B* p_Pointer) { m_Next.Set(p_Pointer, GetTransaction()); }

    // PGC_PROMOTION_LOGIC(TestPGC_B)
    void PromoteMembers() {
        m_TestA.RequestPromotion(GetTransaction());
        m_Next.RequestPromotion(GetTransaction());
    }

public:
	TestPGC_B();
	TestPGC_B(const TestPGC_B& other);
	virtual ~TestPGC_B();

    void SimulateLegacyPromotionOnA();

	std::string GetDescription() const;
	void SetDescription(const std::string& p_Description);

public:
	static long m_ActiveCount;

private:
	std::string m_Description;
};

#endif // !defined(TESTPGC_B_H_INCLUDED)
