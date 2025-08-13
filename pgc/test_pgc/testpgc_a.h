#if !defined(TestPGC_A_H_INCLUDED)
#define TestPGC_A_H_INCLUDED

#pragma once

#include "pgc.h"
#include <string>


using namespace PGCX;

class TestPGC_A : public Base
{
    PGC_CLASS(TestPGC_A);
public:
	TestPGC_A();
	TestPGC_A(const TestPGC_A& other);
	virtual ~TestPGC_A();

	TestPGC_A *GetNext();
	void SetNext(TestPGC_A *p_next);

	std::string GetDescription();
	void SetDescription(const std::string &p_Description);

	virtual TestPGC_A* CloneTo(void* memory) const override;
	virtual void PromoteMembers();
	virtual void RegisterMembers() {};

    long CountList();

	long MagicValue;
private:
	std::string m_Description;
	TestPGC_A *m_next;

public:
	static long m_ActiveCount;
};

#endif // !defined(TestPGC_A_H_INCLUDED)
