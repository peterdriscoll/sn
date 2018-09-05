#if !defined(TESTPGC_B_H_INCLUDED)
#define TESTPGC_B_H_INCLUDED

#pragma once

#include "pgc.h"
#include "ref.h"
namespace PGCX
{
	template<typename T>
	using SRef = PGC::SRef<T>;

	template<typename T>
	using Ref = PGC::Ref<T>;
}

using namespace PGCX;

class TestPgc_A;

class TestPgc_B : public Base
{
    PGC_CLASS(TestPgc_B);
public:
	TestPgc_B();
	virtual ~TestPgc_B();

	Ref<TestPgc_A> GetTestA();
	void SetTestA(Ref<TestPgc_A> newVal);

    virtual void PromoteMembers();
	Base *Clone(PGC::PGC_Transaction & /*p_Transaction*/);
private:
	TestPgc_A* m_TestA;

public:
	static long m_ActiveCount;
};

#endif // !defined(TESTPGC_B_H_INCLUDED)
