#pragma once

#if !defined(TESTPGC_C_H_INCLUDED)
#define TESTPGC_C_H_INCLUDED

#include "pgc.h"
#include <string>
#include <vector>

using namespace std;

class TestPGC_A;
#undef ACTION_OVER_MEMBERS
#undef ACTION_OVER_CONTAINERS

#define ACTION_OVER_MEMBERS(ACTION) \
    ACTION(AMember, TestPGC_C) \
    ACTION(AMember, TestPGC_A)

#define ACTION_OVER_CONTAINERS(ACTION) \
    ACTION(ACollection, std::vector, TestPGC_A)

class Test_PGC_C : public Base
{
	PGC_CLASS(Test_PGC_C);

public:
	Test_PGC_C();
	virtual ~Test_PGC_C();

//	MEMBER_DEFINITIONS
private:
    Ref<TestPGC_C> m_CMember;
    Ref<TestPGC_A> m_AMember;
    std::vector<Ref<TestPGC_A>> m_ACollection;

public:
    TestPGC_C* GetCMember() {
        return m_CMember.Get();
    }

    void SetCMember(TestPGC_C* p_Pointer) {
        m_CMember.Set(p_Pointer, GetTransaction());
    }

    TestPGC_A* GetAMember() {
        return m_AMember.Get();
    }

    void SetAMember(TestPGC_A* p_Pointer) {
        m_AMember.Set(p_Pointer, GetTransaction());
    }

    void RequestPromotionACollection(TestPGC_A* p_Pointer) {
        for (auto& _ref : m_ACollection)
            _ref.RequestPromotion(GetTransaction());
    }

    void PromoteMembers() {
        m_CMember.RequestPromotion(GetTransaction());
        m_AMember.RequestPromotion(GetTransaction());

        for (auto& _ref : m_ACollection)
            _ref.RequestPromotion(GetTransaction());
    }

    void RegisterMembers() {
        RegisterMember((PGC::PGC_Base*)m_CMember.Get());
        RegisterMember((PGC::PGC_Base*)m_AMember.Get());
    }

    long GetLength();
    void SetLength(long p_Value);
private:
	long m_Length;
};

#endif // !defined(TESTPGC_C_H_INCLUDED)
