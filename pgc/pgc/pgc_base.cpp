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

	unsigned HashCode(const type_info &info)
	{
		return (unsigned)&info;
	}

	unsigned Hasher(const unsigned &key)
	{
		return key;
	}

	PGC_Base::PGC_Base()
		: m_Offset(0)
	{
		m_Transaction = PGC_Transaction::RegisterLastForDestruction(this);
	}

	PGC_Base::PGC_Base(PGC_Transaction & p_Transaction)
		: m_Transaction(&p_Transaction)
		, m_Offset(0)
	{
		PGC_Transaction::RegisterLastForDestruction(this);
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
		if (base)
		{
			PGC_Transaction *transaction = base->GetTransaction();
			if (base && transaction && m_Transaction != transaction && !transaction->IsStatic())
			{
				PGC_Promotion::RequestPromotion(p_Base, GetTransaction());
			}
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
	}

	void * PGC_Base::operator new(size_t p_size, PGC_Transaction &p_Transaction)
	{
		return p_Transaction.CreateNew(p_size);
	}

	void * PGC_Base::operator new(size_t p_size)
	{
		return PGC_Transaction::TopTransaction()->CreateNew(p_size);
	}

	void PGC_Base::operator delete(void * /* p_Object */, PGC_Transaction & /* p_Transaction */)
	{
	}

	void PGC_Base::operator delete(void * /* p_Object */)
	{
	}
}
