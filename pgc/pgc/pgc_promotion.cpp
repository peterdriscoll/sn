#include "pgc_promotion.h"
#include "pgc_base.h"
#include "pgc_transaction.h"

#include "pgc_pch.h"

// If DEBUG_FREE is defined there is a check when freeing a promotion to see if
// the promotion is already in the free list.  This shouldnt happen.  
// So the definition of DEBUG_FREE may be ommitted for speed.
#define DEBUG_FREE

namespace PGC
{
	/*static*/ PGC_Promotion *PGC_Promotion::m_FreeList = NULL;
	/*static*/ PGC_Promotion *PGC_Promotion::m_PromoteList = NULL;
	/*static*/ PGC_Promotion **PGC_Promotion::m_PromoteListLast = &m_PromoteList;

	PGC_Promotion::PGC_Promotion()
		: m_Base(NULL)
		, m_Next(NULL)
		, m_Destination(NULL)
	{
	}

	PGC_Promotion::~PGC_Promotion()
	{
	}

	/*static*/ void PGC_Promotion::RequestPromotion(PGC_Base **p_Base, PGC_Transaction *p_Destination)
	{
		PGC_Promotion *promotion = Allocate();
		promotion->Create(p_Base, p_Destination);
		promotion->AppendRequest();
	}

	//  Process the list
	//  * Promote items whos source is dieing transaction, and remove from the list.
	//  * Remove items from the list if the destination is a dieing transaction.
	//  Leave other items in the list for future promotion.
	/*static*/ void PGC_Promotion::PromoteRequests()
	{
		PGC_Promotion *promotion = m_PromoteList;
		PGC_Promotion **last = &m_PromoteList;
		while (promotion)
		{
			if (promotion->PromoteOrReject())
			{
				PGC_Promotion *next = promotion->m_Next;  // Must get next promotion after PromoteOrReject because it might change in the promotion.

														  // Remove promotion from this point in the list;
				*last = next;			                  // Change the next pointer on the previous promotion to point ot the next promotion.

				promotion->Free();                        // Returm  promotion to the free list for re-use.
				promotion = next;		                  // Set the promotion for the next loop
			}
			else
			{
				last = &(promotion->m_Next);              // Record the last element (not removed from the list.
				promotion = promotion->m_Next;            // Set the promotion for the next loop
			}
		}
		m_PromoteListLast = last;
	}

	void PGC_Promotion::Create(PGC_Base **p_Base, PGC_Transaction *p_Destination)
	{
		m_Base = p_Base;
		m_Destination = p_Destination;
	}

	void PGC_Promotion::AppendRequest()
	{
		*m_PromoteListLast = this;
		m_PromoteListLast = &(m_Next);
		m_Next = 0;
	}

	/*static*/ PGC_Promotion *PGC_Promotion::Allocate()
	{
		PGC_Promotion *result = m_FreeList;
		if (result)
		{
			m_FreeList = m_FreeList->GetNext();
		}
		else
		{
			PGC_Transaction::AddTotalGrossMemorySize(sizeof(PGC_Promotion));
			result = new PGC_Promotion();
		}
		result->m_Next = 0;
		return result;
	}

	void PGC_Promotion::Free()
	{
#ifdef DEBUG_FREE
		ASSERTM(!FindInFreeList(), "Item freed twice");
#endif

		m_Next = m_FreeList;
		m_FreeList = this;
	}

	// Used to check if an item already in the free list is being re-added.
	// This shouldnt happen.
	bool PGC_Promotion::FindInFreeList()
	{
		PGC_Promotion *promotion = m_FreeList;
		while (promotion)
		{
			if (promotion == this)
			{
				return true;
			}
			promotion = promotion->m_Next;
		}
		return false;
	}

	PGC_Base *PGC_Promotion::GetBase()
	{
		return *m_Base;
	}

	PGC_Promotion *PGC_Promotion::GetNext()
	{
		return m_Next;
	}

	void PGC_Promotion::SetNext(PGC_Promotion *p_Promotion)
	{
		m_Next = p_Promotion;
	}

	bool PGC_Promotion::PromoteOrReject()
	{
		if (m_Destination->Dieing())
		{
			return true;
		}
		PGC_Base *base = GetBase();
		if (base)
		{
			PGC_Transaction *source = base->GetTransaction();
			if (!source || source->Dieing())
			{
				Promote();
				return true;
			}
		}
		else
		{
			return true;
		}
		return false;
	}

	void PGC_Promotion::Promote()
	{
		PGC_Base *newBase = GetBase()->GetNewCopyBase();
		if (newBase)
		{
			*m_Base = newBase;
		}
		else
		{
			PGC_Base *copy = CopyMemory();
			m_Destination->RegisterForDestruction(copy);
			GetBase()->SetNewCopyBase(copy);
			copy->SetTransaction(m_Destination);
			copy->PromoteMembers();
			*m_Base = copy;
		}
	}

	PGC_Base *PGC_Promotion::CopyMemory()
	{
		char *l_pointer;
		long l_size;
		GetBase()->RetrieveDescriptor(l_pointer, l_size);
		long offset = l_pointer - (char *)GetBase();
		char *l_new_pointer = (char *)m_Destination->Allocate(l_size);
		PGC_Transaction::AddTotalNetMemorySize(-((long)(l_size - OVERHEAD))); // Allocating adds the net size, but this is a copy, so there should be no increase.
		memcpy(l_new_pointer, l_pointer, l_size);
		GetBase()->SetTransaction(NULL);
		return (PGC_Base *)(l_new_pointer + offset);
	}

	/*static*/ size_t PGC_Promotion::PromotionFreeMemory()
	{
		size_t memory = 0;
		PGC_Promotion *promotion = m_FreeList;
		while (promotion)
		{
			memory += sizeof(PGC_Promotion);
			promotion = promotion->m_Next;
		}
		return memory;
	}

	/*static*/ size_t PGC_Promotion::PromotionUsedMemory()
	{
		size_t memory = 0;
		PGC_Promotion *promotion = m_PromoteList;
		while (promotion)
		{
			memory += sizeof(PGC_Promotion);
			promotion = promotion->m_Next;
		}
		return memory;
	}
}
