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
		, m_Promoted(false)
		, m_Dropped(false)
		, m_InProcessingList(true)
		, m_RefAttached(true)
	{
	}

	PGC_Promotion::~PGC_Promotion()
	{
	}

	/*static*/ PGC_TypeCheck* PGC_Promotion::CheckRequestPromotion(PGC_TypeCheck** p_Base, PGC_Transaction* p_Source, PGC_Transaction* p_Destination, PromotionStrategy p_CheckType)
	{
		PGC_TypeCheck* return_value = *p_Base;
		if (p_Base && p_Source && p_Source != p_Destination && p_Source->IsDescendantOf(p_Destination) && !p_Source->IsStatic())
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

	PromotionResult  PGC_Promotion::PromoteOrReject()
	{
		if (!m_RefAttached)
		{
			return PromotionResult::Dropped;
		}

		if (!m_Destination || m_Destination->Dieing())
		{
			return PromotionResult::Dropped;
		}

		PGC_Transaction* source = GetSource();
		if (!source)
		{
			return PromotionResult::Dropped;
		}

		if (source == m_Destination)
		{
			return PromotionResult::Dropped;
		}

		if (source->Dieing())
		{
			// Perform promotion
			Promote();
			return (m_Strategy == PromotionStrategy::DoubleDipping)
				? PromotionResult::PromotedKeep
				: PromotionResult::PromotedDone;
		}

		return PromotionResult::Keep;
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
		}
		switch (m_Strategy)
		{
		case PromotionStrategy::Backstabbing:
			*m_Base = copy;
			break;
		case  PromotionStrategy::DoubleDipping:
			m_FinalCopy = copy;
			break;
		default:
			ASSERTM(false, "Invalid promotion strategy");
			break;
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
		PGC_Base* newBase = nullptr;
		                                                                                                                                                                                                                                                                                         
		FuncVector captureVector;
		{
			PromotionCaptureScope scope; // Capture any promotable members constructed during the clone
 			newBase = base->MoveTo(newMemory);   // uses T(T&&) via placement-new
			captureVector = scope.GetCaptures();
		}

		if (newBase)
		{
			newBase->SetTransaction(p_Destination);
			if (captureVector.size() == 0)
			{
				newBase->PromoteMembers();
			}

			// Request promotion for captured promotables.
			for (auto f : captureVector)
			{
				f(p_Destination);
			}	
		}
		else
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
			newBase->SetTransaction(p_Destination);
			newBase->PromoteMembers();
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
		if (p_Strategy == PromotionStrategy::DoubleDipping)
		{
			long dog = 10;
		}
		m_Base = p_Base;
		m_Destination = p_Destination;
		m_Strategy = p_Strategy;
		m_Promoted = false;
		m_Dropped = false;
		m_FinalCopy = *p_Base;  // <- capture the original before any overwrite happens
		m_RefAttached = true;
		m_InProcessingList = true;
	}

	PGC_User* PGC_Promotion::GetUser() const
	{
		return m_Destination->GetUser();
	}

	bool PGC_Promotion::IsPromotedOrDropped() const
	{
		return m_Promoted || m_Dropped;
	}

	void PGC_Promotion::MarkPromoted()
	{
		m_Promoted = true;
		m_InProcessingList = false;
		GetUser()->AddProcessedDoubleDippingMemory(sizeof(PGC_Promotion));
	}
	void PGC_Promotion::FreeFromRefAttached()
	{
		m_RefAttached = false;
		TryFree();
	}
	void PGC_Promotion::FreeFromProcessingList()
	{
		m_InProcessingList = false;
		if (m_Strategy == PromotionStrategy::DoubleDipping && m_RefAttached)
		{
			m_Dropped = true;
			GetUser()->AddProcessedDoubleDippingMemory(sizeof(PGC_Promotion));
		}
		TryFree();
	}
	void PGC_Promotion::TryFree()
	{
		switch (m_Strategy) {
		case PromotionStrategy::Backstabbing:
			GetUser()->FreePromotion(this);
			break;
		case  PromotionStrategy::DoubleDipping:
			if (!m_RefAttached && !m_InProcessingList)
			{
				GetUser()->FreePromotion(this);
			}
			break;
		default:
			ASSERTM(false, "Invalid promotion strategy");
			break;
		}
	}

	PGC_TypeCheck* PGC_Promotion::GetBase()
	{
		return *m_Base;
	}

	PGC_TypeCheck** PGC_Promotion::GetBaseAddress()
	{
		#if defined(DEBUG) || defined(_DEBUG)
		ASSERTM(m_Base != nullptr,
			"No pointer to memory address.");
		#endif
		return m_Base;
	}

	void PGC_Promotion::SetBase(PGC_TypeCheck** p_Base)
	{
		m_Base = p_Base;
	}

	void PGC_Promotion::Rebind(PGC_TypeCheck** p_Base, PGC_Transaction* p_DestinationTransaction)
	{
		m_Base = p_Base;
		m_Destination = p_DestinationTransaction;
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

	PGC_TypeCheck* PGC_Promotion::GetLogicalPointer()
	{
		return GetFinalCopy();
	}

	PGC_Transaction* PGC_Promotion::GetLogicalOwnerTransaction()
	{
		return GetDestination();
	}

	PGC_Promotion* PGC_Promotion::GetLogicalPromotion()
	{
		return this;
	}
}
