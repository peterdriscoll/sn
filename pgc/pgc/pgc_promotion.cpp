#include "pgc_promotion.h"
#include "pgc_base.h"
#include "pgc_transaction.h"

#include "pgc_pch.h"

namespace PGC
{
	long g_IDCounter = 0;

	PGC_Promotion::PGC_Promotion()
		: m_Base(nullptr)
		, m_Next(nullptr)
		, m_Destination(nullptr)
		, m_Source(nullptr)
		, m_FinalCopy(nullptr)
		, m_Promoted(false)
		, m_Dropped(false)
		, m_InProcessingList(true)
		, m_RefAttached(true)
		, m_Id(++g_IDCounter)
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

		if (!m_Destination || m_Destination->IsDying())
		{
			return PromotionResult::Dropped;
		}

 		ASSERTM(Verify(), "Invalid promotion.");
		PGC_Transaction* source = GetSource();
		if (!source)
		{
			return PromotionResult::Dropped;
		}

		if (source == m_Destination)
		{
			return PromotionResult::Dropped;
		}

		if (source->IsDying())
		{
			// Perform promotion
			Promote();
			return (m_Strategy == PromotionStrategy::DoubleDipping)
				? PromotionResult::PromotedKeep
				: PromotionResult::PromotedDone;
		}

		return PromotionResult::Keep;
	}

	bool PGC_Promotion::Verify()
	{
		return m_Base && *m_Base && m_Destination && m_Source && m_FinalCopy;
	}

	void PGC_Promotion::Promote()
	{
		auto* user = PGC_User::GetCurrentPGC_User();
		user->with_lock(WithLock, [&] {
			ASSERTM(Verify(), "Invalid promotion.");
			PGC_TypeCheck* base = (*m_Base)->GetLogicalPointer();
			ASSERTM(base, "Base pointer should not be null during promotion");
			PGC_TypeCheck* promotedCopy = base->GetPromotedCopy();
			PGC_TypeCheck* copy;
			if (promotedCopy)
			{
				copy = promotedCopy;
			}
			else
			{
				copy = CopyMemory(base, m_Destination);
				base->SetPromotedCopy(copy);
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
		});
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
			newBase->SetCallDestructor(true);   // Just a precaution. Prevent carrying over a false from source
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
			base->SetCallDestructor(false); // This will stop the destructor being called.
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
		ASSERTM(p_Base, "No address for promotion");
		ASSERTM(*p_Base, "No instance for promotion");
		m_Base = p_Base;
		ASSERTM(p_Destination, "No destination transaction for promotion");
		m_Destination = p_Destination;
		m_Source = (*m_Base)->GetTransaction();
		ASSERTM(m_Source, "No source transaction for promotion");
		m_Strategy = p_Strategy;
		m_Promoted = false;
		m_Dropped = false;
		m_FinalCopy = *p_Base;  // <- capture the original before any overwrite happens
		m_RefAttached = true;
		m_InProcessingList = true;
		ASSERTM(Verify(), "Invalid promotion.");
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
		ASSERTM(!m_Promoted, "Promotion should not be marked twice.");
		ASSERTM(m_InProcessingList, "Must be in processing list to be removed from it.");
		m_Promoted = true;
		m_InProcessingList = false;
		GetUser()->AddProcessedRefAttachedMemory(sizeof(PGC_Promotion));
	}
	void PGC_Promotion::FreeFromRefAttached()
	{
		if (m_RefAttached)
		{
			m_RefAttached = false;
			TryFree();
		}
	}
	void PGC_Promotion::FreeFromProcessingList()
	{
		ASSERTM(m_InProcessingList, "Must be in the processing list to be freed from it.");
		m_InProcessingList = false;
		if (m_Strategy == PromotionStrategy::DoubleDipping && m_RefAttached)
		{
			m_Dropped = true;
			GetUser()->AddProcessedRefAttachedMemory(sizeof(PGC_Promotion));
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
		ASSERTM(Verify(), "Invalid promotion.");
	}

	void PGC_Promotion::Rebind(PGC_TypeCheck** p_Base, PGC_Transaction* p_DestinationTransaction)
	{
		m_Base = p_Base;
		m_Destination = p_DestinationTransaction;
		ASSERTM(Verify(), "Invalid promotion.");
	}

	PGC_Transaction* PGC_Promotion::GetSource()
	{
		return m_Source;
	};

	PGC_Transaction* PGC_Promotion::GetDestination()
	{
		return m_Destination;
	}

	bool PGC_Promotion::GetPromoted()
	{
		return m_Promoted;
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
