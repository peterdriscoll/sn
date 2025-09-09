#include "pgc_block.h"
#include "pgc_base.h"
#include "pgc_transaction.h"

#include "pgc_pch.h"

namespace PGC
{
	PGC_Block::PGC_Block(PGC_Transaction *p_Transaction, PGC_Block *p_LastBlock)
		: m_Transaction(p_Transaction)
		, m_NextBlock(NULL)
		, m_end(NULL)
		, m_current(NULL)
	{
		if (p_LastBlock)
		{
			p_LastBlock->m_NextBlock = this;
		}
		m_current = m_Data;
		m_end = m_Data + BlockSize;
		m_DestuctionList = 0;
		m_DestuctionListLast = 0;

		m_Transaction->GetUser()->AddTotalGrossMemorySize(sizeof(PGC_Block));
	}

	void PGC_Block::FinalizeUncopied()
	{
		PGC_Base* next = m_DestuctionList;
		while (next)
		{
			PGC_Base* temp = next->GetNext((void*)m_current);
			if (next->GetCallDestructor())
			{
				next->Finalize();
			}
			next = temp;
		}
	}

	void PGC_Block::DestroyUncopied()
	{
		PGC_Base *next = m_DestuctionList;
		while (next)
		{
			PGC_Base* temp = next->GetNext((void *)m_current);
			if (next->GetCallDestructor())
			{
				m_Transaction->GetUser()->AddTotalNetMemorySize(-((long)(next->Size() - PGC_OVERHEAD)));
				next->~PGC_Base();
			}
			next = temp;
		}
	}

	PGC_Base *PGC_Block::FindOwner(void *p_member)
	{
		char * member = (char *)p_member;
		if ((m_Data <= member) && (member < m_end))
		{
			PGC_Base *next = m_DestuctionList;
			while (next)
			{
				PGC_Base *temp = next->GetNext(m_current);
				if (member < (char *)temp)
				{
					return next;
				}
				next = temp;
			}
		}
		return NULL;
	}

	PGC_Block::~PGC_Block()
	{
		m_Transaction->GetUser()->AddTotalGrossMemorySize(-((long) sizeof(PGC_Block)));
	}

	PGC_Block *PGC_Block::GetNextBlock()
	{
		return m_NextBlock;
	}

	void* PGC::PGC_Block::Allocate(size_t p_size, std::size_t p_align)
	{
		if (p_align < alignof(std::max_align_t))
		{
			p_align = alignof(std::max_align_t);
		}

		uintptr_t cur = reinterpret_cast<uintptr_t>(m_current);
		uintptr_t aligned = (cur + (p_align - 1)) & ~(uintptr_t)(p_align - 1);
		std::size_t pad = static_cast<std::size_t>(aligned - cur);

		if (aligned + p_size > reinterpret_cast<uintptr_t>(m_end)) {
			return nullptr; // not enough space in this block
		}

		void* mem = reinterpret_cast<void*>(aligned);
		m_current = reinterpret_cast<char*>(aligned + p_size);
		m_Transaction->GetUser()->AddTotalNetMemorySize(static_cast<long>(p_size - PGC_OVERHEAD));
		return mem;
	}

	void PGC_Block::RegisterForDestruction(PGC_Base *p_Base)
	{
		if (m_DestuctionListLast)
		{
			m_DestuctionListLast->SetNext(p_Base);
			m_DestuctionListLast = p_Base;
		}
		else
		{
			m_DestuctionList = p_Base;
			m_DestuctionListLast = p_Base;
		}
		p_Base->SetNext(NULL);
	}

	size_t PGC_Block::NetMemoryUsed()
	{
		size_t baseClassSize = 0;
		PGC_Base *next = m_DestuctionList;
		while (next)
		{
			baseClassSize += PGC_OVERHEAD;
			next = next->GetNext(m_current);
		}
		return (m_current - m_Data) - baseClassSize;
	}
}
