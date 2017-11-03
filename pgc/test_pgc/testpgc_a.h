#if !defined(TESTPGC_A_H_INCLUDED)
#define TESTPGC_A_H_INCLUDED

#pragma once

#include "pgc.h"
#include <string>

using namespace std;
using namespace PGCX;

class TestPgc_A : public Base
{
    PGC_CLASS(TestPgc_A);
public:
	TestPgc_A();
	virtual ~TestPgc_A();

	TestPgc_A *GetNext();
	void SetNext(TestPgc_A *p_next);

	string GetDescription();
	void SetDescription(const string &p_Description);

    virtual void PromoteMembers();

    long CountList();

	Base *Clone(Transaction & /*p_Transaction*/);
private:
	string m_Description;
	TestPgc_A *m_next;

public:
	static long m_ActiveCount;
};

#endif // !defined(TESTPGC_A_H_INCLUDED)
