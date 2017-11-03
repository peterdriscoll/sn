#include "pgc_base.h"
#include "pgc_transaction.h"
#include "pgc_promotion.h"

#include <iostream>
#include <typeinfo>       // operator typeid
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

#include "pgc_pch.h"

namespace PGC
{
	// assert with message.
	void assertm(char* expr_str, bool expr, char* file, int line, string msg)
	{
		if (!expr)
		{
			cerr << "Assert failed:\t" << msg << "\n"
				<< "Expected:\t" << expr_str << "\n"
				<< "Source:\t\t" << file << ", line " << line << "\n";
			abort();
		}
	}

	typedef vector<size_t> MemberOffsetList;
	typedef vector<PGC_Base **> MemberList;

	unsigned HashCode(const type_info &info)
	{
		return (unsigned)&info;
	}

	unsigned Hasher(const unsigned &key)
	{
		return key;
	}

	typedef unordered_map <unsigned, MemberOffsetList *> MembersTable;

	MembersTable *g_MembersTable = NULL;
	MemberList *g_MemberList = NULL;

	MembersTable& GetMembersTable()
	{
		if (g_MembersTable == NULL)
		{
			g_MembersTable = new MembersTable();
		}
		return *g_MembersTable;
	}

	PGC_Base::PGC_Base()
		: m_Offset(0)
	{
		m_Transaction = PGC_Transaction::RegisterLastForDestruction(this);
		RecordMembers();
	}

	PGC_Base::PGC_Base(PGC_Transaction & p_Transaction)
		: m_Transaction(&p_Transaction)
		, m_Offset(0)
	{
		PGC_Transaction::RegisterLastForDestruction(this);
		RecordMembers();
	}

	PGC_Base::~PGC_Base()
	{

	}

	PGC_Base *PGC_Base::Clone(PGC_Transaction & /*p_Transaction*/)
	{
		return NULL;
	}

	void PGC_Base::RequestPromotion(PGC_Base **p_Base)
	{
		PGC_Base *base = *p_Base;
		if (base && (m_Transaction != base->GetTransaction()))
		{
			PGC_Promotion::RequestPromotion(p_Base, GetTransaction());
		}
	}

	PGC_Transaction * PGC_Base::GetTransaction()
	{
		if (0 <= m_Offset)
		{
			return (PGC_Transaction *)m_Transaction;
		}
		return 0;
	}

	void PGC_Base::SetTransaction(PGC_Transaction *p_Transaction)
	{
		m_Transaction = (void *)p_Transaction;
		if (m_Offset < 0)
		{
			m_Offset = (short)-m_Offset;
		}
	}

	PGC_Base *PGC_Base::GetNewCopyBase()
	{
		if (m_Offset < 0)
		{
			return (PGC_Base *)m_Transaction;
		}
		return 0;
	}

	void PGC_Base::SetNewCopyBase(PGC_Base *p_Base)
	{
		m_Transaction = (void *)p_Base;
		if (0 < m_Offset)
		{
			m_Offset = (short)-m_Offset;
		}
	}

	PGC_Base *PGC_Base::GetNext()
	{
		long offset = m_Offset;
		if (offset < 0)
		{
			offset = -offset;
		}
		if (offset)
		{
			return (PGC_Base *)((char *) this + offset);
		}
		return NULL;
	}

	void PGC_Base::SetNext(PGC_Base *p_Base)
	{
		if (p_Base)
		{
			m_Offset = (short)((char *)p_Base - (char *)this);
			ASSERTM(m_Offset > 0, "Offset from previous instance location must be positive");
		}
		else
		{
			m_Offset = 0;
		}
	}

	size_t PGC_Base::Size()
	{
		long l_size;
		char *l_pointer;
		RetrieveDescriptor(l_pointer, l_size);
		return l_size;
	}

	void PGC_Base::PromoteMembers()
	{
		MemberOffsetList *memberOffsets = GetMembersTable()[HashCode(typeid(*this))];
		if (memberOffsets)
		{
			for (size_t j = 0; j<memberOffsets->size(); j++)
			{
				size_t memberOffset = (*memberOffsets)[j];
				PGC_Base **member = (PGC_Base **)(((size_t) this) + memberOffset);
				RequestPromotion(member);
			}
		}
	}

	void PGC_Base::RecordMembers()
	{
		if (g_MemberList)
		{
			MemberOffsetList *memberOffsets = GetMembersTable()[HashCode(typeid(*this))];
			if (memberOffsets == NULL)
			{
				memberOffsets = new MemberOffsetList;
				for (size_t j = 0; j<g_MemberList->size(); j++)
				{
					PGC_Base **member = (*g_MemberList)[j];
					size_t memberOffset = ((size_t)member) - ((size_t) this);
					memberOffsets->push_back(memberOffset);
				}
				GetMembersTable()[HashCode(typeid(*this))] = memberOffsets;
			}
			g_MemberList = NULL;
		}
	}

	/*static*/ void PGC_Base::CaptureMember(PGC_Base **p_member)
	{
		if (g_MemberList)
		{
			g_MemberList->push_back(p_member);
		}
	}

	void * PGC_Base::operator new(size_t p_size, PGC_Transaction &p_Transaction)
	{
		g_MemberList = new MemberList;  // For capturing members for promotion.
		return p_Transaction.Allocate(p_size);
	}

	void * PGC_Base::operator new(size_t p_size)
	{
		g_MemberList = new MemberList;  // For capturing members for promotion.
		return PGC_Transaction::TopTransaction()->Allocate(p_size);
	}

	void PGC_Base::operator delete(void * /* p_Object */, PGC_Transaction & /* p_Transaction */)
	{
	}

	void PGC_Base::operator delete(void * /* p_Object */)
	{
	}
}
