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
		: m_Base(nullptr)
		, m_Next(nullptr)
		, m_Destination(nullptr)
		, m_FinalCopy(nullptr)
	{
	}

	PGC_Promotion::~PGC_Promotion()
	{
	}

	/*static*/ PGC_TypeCheck* PGC_Promotion::CheckRequestPromotion(PGC_TypeCheck** p_Base, PGC_Transaction* p_Source, PGC_Transaction* p_Destination, PromotionStrategy p_CheckType)
	{
		PGC_TypeCheck* return_value = *p_Base;
		if (p_Base && p_Source && p_Source != p_Destination && !p_Source->IsStatic())
		{
			return_value = RequestPromotion(p_Base, p_Destination, p_CheckType);
		}
		return return_value;
	}

	/*static*/ PGC_TypeCheck* PGC_Promotion::RequestPromotion(PGC_TypeCheck** p_Base, PGC_Transaction* p_Destination, PromotionStrategy p_Strategy)
	{
		PGC_Promotion* promotion = PGC_Promotion::Allocate();
		promotion->Create(p_Base, p_Destination, p_Strategy);
		promotion->AppendRequest();
		return static_cast<PGC_TypeCheck*>(promotion);
	}

	bool PGC_Promotion::PromoteOrReject()
	{
		if (!m_Destination || m_Destination->Dieing())
		{
			// Invalid destination, drop promotion
			// ASSERTM(false, "Promotion from an outer to an inner transaction is not allowed.");
			return true;
		}

		PGC_Transaction* source = GetSource();

		if (!source)
		{
			// Invalid source, drop promotion
			return true;
		}

		if (source == m_Destination)
		{
			return true;
		}

		if (source->Dieing())
		{
			// Valid promotion condition
			Promote();
			return true;
		}

		// Still valid but source is alive — keep for later
		return false;
	}

	void PGC_Promotion::Promote()
	{
		PGC_Base* newBase = static_cast<PGC_Base*>((*m_Base)->GetPromotedCopy());
		PGC_TypeCheck* copy;
		if (newBase)
		{
			copy = newBase;
		}
		else
		{
			copy = CopyMemory(*m_Base, m_Destination);
			(*m_Base)->SetPromotedCopy(copy);
			newBase = static_cast<PGC_Base*>(copy);
			newBase->SetTransaction(m_Destination);
			newBase->PromoteMembers();
		}
		switch (m_Strategy)
		{
		case PromotionStrategy::Backstabbing:
			*m_Base = copy;
			break;
		case  PromotionStrategy::DoubleDipping:
			m_FinalCopy = copy;
		}
	}
	/*static*/PGC_TypeCheck* PGC_Promotion::CopyMemory(PGC_TypeCheck* p_Base, PGC_Transaction* p_Destination)
	{
		PGC_Base* base = static_cast<PGC_Base*>(p_Base);
		long size;
		char* dataStart;
		base->RetrieveDescriptor(dataStart, size);

		void* newMemory = p_Destination->CreateNew(size);

		// Clone the original object into the new memory block via placement new
		PGC_Base* newBase = base->CloneTo(newMemory); // TO DO: Constructor not being called on PGC_Base.

		if (!newBase)
		{
			p_Destination->AddTotalNetMemorySize(-static_cast<long>(size - PGC_OVERHEAD));
			// Fallback to memcpy if CloneTo isn't implemented
			memcpy(newMemory, dataStart, size);
			// Note: no constructor, no destuctor.
			// Recalculate newBase using original offset
			char* oldPointer = reinterpret_cast<char*>(p_Base);
			ptrdiff_t offset = dataStart - oldPointer;
			newBase = reinterpret_cast<PGC_Base*>(
				static_cast<char*>(newMemory) + offset);
			base->SetTransaction(nullptr); // This will stop the destructor being called.
			p_Destination->RegisterLastForDestruction(newBase);
		}
		ASSERTM(newBase != nullptr, "Did not expect CloneTo to return a nullptr");

		return newBase;
	}

	bool PGC_Promotion::IsPromotion()
	{
		return true;
	}
	PGC_TypeCheck* PGC_Promotion::GetFinalCopy() const
	{
		return m_FinalCopy;
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
				// I think here is where the promotion is freed where it maybe shouldnt be.
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

	void PGC_Promotion::Create(PGC_TypeCheck **p_Base, PGC_Transaction *p_Destination, PromotionStrategy p_Strategy)
	{
		m_Base = p_Base;
		m_Destination = p_Destination;
		m_Strategy = p_Strategy;
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

	/*static*/ void PGC_Promotion::ClearAllPromotions()
	{
		// Walk through the free list and delete everything
		while (m_FreeList)
		{
			PGC_Promotion* next = m_FreeList->GetNext();

			PGC_Transaction::AddTotalGrossMemorySize(-static_cast<long>(sizeof(PGC_Promotion)));

			delete m_FreeList;

			m_FreeList = next;
		}
	}

	PGC_TypeCheck* PGC_Promotion::GetBase()
	{
		return *m_Base;
	}

	PGC_TypeCheck** PGC_Promotion::GetBaseRef()
	{
		return m_Base;
	}

	PGC_Transaction* PGC_Promotion::GetSource()
	{
		return (*m_Base)->GetTransaction();
	};

	PGC_Transaction* PGC_Promotion::GetDestination()
	{
		return m_Destination;
	};

	PGC_Promotion *PGC_Promotion::GetNext()
	{
		return m_Next;
	}

	void PGC_Promotion::SetNext(PGC_Promotion *p_Promotion)
	{
		m_Next = p_Promotion;
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
