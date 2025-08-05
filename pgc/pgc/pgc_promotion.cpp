#include "pgc_promotion.h"
#include "pgc_base.h"
#include "pgc_transaction.h"

#include "pgc_pch.h"

namespace PGC
{
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

	PGC_TypeCheck* PGC_Promotion::RequestPromotion(PGC_TypeCheck** p_Base, PGC_Transaction* p_Destination, PromotionStrategy p_Strategy)
	{
		PGC_User* user = p_Destination->GetUser();
		PGC_Promotion* promotion = user->Allocate();
		promotion->Create(p_Base, p_Destination, p_Strategy);
		user->AppendRequest(promotion);
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
		PGC_TypeCheck* typeCheck = p_Base;
		if (p_Base->IsPromotion())
		{
			typeCheck = static_cast<PGC_Promotion*>(p_Base)->GetFinalCopy();
		}
		PGC_Base* base = static_cast<PGC_Base*>(typeCheck);

		char* dataStart;
		long size;
		base->RetrieveDescriptor(dataStart, size);

		void* newMemory = p_Destination->CreateNew(size);

		// Clone the original object into the new memory block via placement new
		PGC_Base* newBase = base->CloneTo(newMemory); // TO DO: Constructor not being called on PGC_Base.

		if (!newBase)
		{
			p_Destination->GetUser()->AddTotalNetMemorySize(-static_cast<long>(size - PGC_OVERHEAD));
			// Fallback to memcpy if CloneTo isn't implemented
			memcpy(newMemory, dataStart, size);
			// Note: no constructor, no destuctor.
			// Recalculate newBase using original offset
			char* oldPointer = reinterpret_cast<char*>(base);
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
	void PGC_Promotion::Create(PGC_TypeCheck** p_Base, PGC_Transaction* p_Destination, PromotionStrategy p_Strategy)
	{
		m_Base = p_Base;
		m_Destination = p_Destination;
		m_Strategy = p_Strategy;
		m_FinalCopy = *p_Base;  // <- capture the original before any overwrite happens
	}

	PGC_User* PGC_Promotion::GetUser() const
	{
		return m_Destination->GetUser();
	}

	void PGC_Promotion::Free()
	{
		GetUser()->FreePromotion(this);
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
		PGC_TypeCheck* base = *m_Base;

		// Resolve if base is itself a promotion
		while (base->IsPromotion()) {
			base = static_cast<PGC_Promotion*>(base)->GetFinalCopy();
		}

		return base->GetTransaction();
	};

	PGC_Transaction* PGC_Promotion::GetDestination()
	{
		return m_Destination;
	}

	PromotionStrategy PGC_Promotion::GetStrategy()
	{
		return m_Strategy;
	}
}
