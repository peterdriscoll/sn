#if !defined(MEMBERREF_INCLUDED)
#define MEMBERREF_INCLUDED

#pragma once

#include "pgc_base.h"
#include "pgc_transaction.h"
#include "pgc_typecheck.h"
#include "pgc_promotion.h"
#include "pgc_promotionstrategy.h"

#include <iostream>

namespace PGC
{
	template <typename T>
	class MemberRef
	{
	private:
		mutable PGC_TypeCheck* m_Pointer;

	public:
		MemberRef()
			: m_Pointer(nullptr)
		{
		}

		MemberRef(T* p_Pointer, PGC_Transaction* p_Transaction)
			: m_Pointer(dynamic_cast<PGC_TypeCheck*>(p_Pointer))
		{
			RequestPromotion(p_Transaction);
		}

		MemberRef(const MemberRef<T>& other)
			: m_Pointer(dynamic_cast<PGC_TypeCheck*>(other.Get()))
		{
			// Debug safety: if someone ever changes Get(), catch it.
			#if defined(DEBUG) || defined(_DEBUG)
			ASSERTM(!m_Pointer || !m_Pointer->IsPromotion(),
				"Copy ctor must not copy a promotion pointer");
			#endif
		}

		MemberRef(MemberRef<T>&& other) noexcept
			: m_Pointer(other.m_Pointer)
		{
			other.m_Pointer = nullptr;
			if (m_Pointer && m_Pointer->IsPromotion())
			{
				// The promotion is no longer used at the old location so repurpose it for here.
				// Could also free it and do a RequestPromotion, as an alternate approach.
				PGC_Promotion *promotion = static_cast<PGC_Promotion*>(m_Pointer);
				if (promotion && promotion->IsPromotedOrDropped())
				{
					m_Pointer = promotion->GetFinalCopy();
					promotion->FreeFromRefAttached();  // Put the promotion object back on the free list for reuse.
				}
				else
				{
					promotion->SetBase(&m_Pointer); // Fix the promotion to refer to the location moved to.
				}
			}
		}
		~MemberRef() noexcept
		{
			if (m_Pointer && m_Pointer->IsPromotion())
			{
				PGC_Promotion* promotion = static_cast<PGC_Promotion*>(m_Pointer);

				// In debug, catch rebinding bugs early.
				#if defined(DEBUG) || defined(_DEBUG)
				ASSERTM(promotion->GetStrategy() == PromotionStrategy::DoubleDipping,
					"~MemberRef: promotion pointer present under Backstabbing");
				ASSERTM(promotion->GetBaseAddress() == &m_Pointer,
					"~MemberRef: promotion base not bound to this address");
				#endif

				promotion->FreeFromRefAttached();
			}
		}
		T* Get() const
		{
			if (!m_Pointer) return nullptr;

			if (m_Pointer->IsPromotion())
			{
				auto* promotion = static_cast<PGC_Promotion*>(m_Pointer);
				ASSERTM(promotion, "IsPromotion returned true but cast to PGC_Promotion failed");
				if (promotion->IsPromotedOrDropped())
				{
					m_Pointer = promotion->GetFinalCopy();
					promotion->FreeFromRefAttached();  // Put the promotion object back on the free list for reuse.
				}
				else
				{
					return dynamic_cast<T*>(promotion->GetFinalCopy());
				}
			}
			return dynamic_cast<T*>(m_Pointer);
		}
		
		void Set(T* p_Pointer, PGC_Transaction* p_Destination)
		{
			ASSERTM(p_Destination, "Always need a destination.");

			std::lock_guard<std::mutex> lock(p_Destination->m_Mutex);

			// Clean up old promotion if present
			if (m_Pointer && m_Pointer->IsPromotion())
			{
				static_cast<PGC_Promotion*>(m_Pointer)->FreeFromRefAttached();
			}

			// Handle null pointer case
			if (!p_Pointer)
			{
				m_Pointer = nullptr;
				return;
			}

			// Set new pointer and promote if needed
			m_Pointer = dynamic_cast<PGC_TypeCheck*>(p_Pointer);
			ASSERTM(m_Pointer, "dynamic_cast returned nullptr");

			RequestPromotion(p_Destination);
		}
		
		// Implicit conversion to T*. Use with care: does not trigger promotion, does not check validity.
		// Only safe if you're confident in the lifetime and transaction context of the pointed object.
		operator T* () const
		{
			return Get();
		}
		
		MemberRef<T>& operator=(T*) = delete; 
		
		void RequestPromotion(PGC_Transaction* p_Destination)
		{
			if (!m_Pointer || m_Pointer->IsPromotion())
				return;

			PGC_Transaction* source = m_Pointer->GetTransaction();
			if (!source) return;
			PromotionStrategy strategy = source->GetPromotionStrategy();

			switch (strategy)
			{
			case PromotionStrategy::Backstabbing:
				// modifies *p_Pointer (back-reference) via side effect
				PGC_Promotion::CheckRequestPromotion(&m_Pointer, source, p_Destination, strategy);
				break;

			case PromotionStrategy::DoubleDipping:
				{
					PGC_TypeCheck* typeCheck = PGC_Promotion::CheckRequestPromotion(&m_Pointer, source, p_Destination, strategy);
					if (typeCheck != m_Pointer)
					{
						m_Pointer = typeCheck;
					}
				}
				break;
			default:
				ASSERTM(false, "Invalid promotion strategy");
				break;
			}
		}
		
		void PromoteNow(PGC_Transaction* destination)
		{
			if (!m_Pointer)
				return;
			PGC_Transaction* source = m_Pointer->GetTransaction();
			if (source && destination &&
				source != destination &&
				!destination->Dieing() &&
				!source->IsStatic())
			{
				PGC_Promotion promotion;
				promotion.Create(
					reinterpret_cast<PGC_TypeCheck**>(&m_Pointer),
					destination,
					PromotionStrategy::Backstabbing
				);

				promotion.Promote();  // Directly promotes and updates pointer via backstabbing
			}
		}
		
		MemberRef<T>& operator=(const MemberRef<T>& other)
		{
			if (this != &other)
			{
				m_Pointer = other.m_Pointer;
				// No auto-promotion; optional
			}
			return *this;
		}
		
		MemberRef<T>& operator=(MemberRef<T>&& other) noexcept
		{
			if (this != &other)
			{
				m_Pointer = other.m_Pointer;
				other.m_Pointer = nullptr;
			}
			return *this;
		}
		
		T& operator*() const
		{
			T* ptr = Get();
			ASSERTM(ptr, "MemberRef::operator* - dereferenced null pointer");
			return *ptr;
		}
		
		T* operator->() const
		{
			T* result = Get();
			ASSERTM(result, "MemberRef::operator-> - result is NULL after promotion");
			return result;
		}
	};
}

#endif // !defined(MEMBERREF_INCLUDED)