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
	template <class T>
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
			: m_Pointer(other.m_Pointer)
		{
			// No promotion here; context-dependent
		}
		MemberRef(MemberRef<T>&& other) noexcept
			: m_Pointer(other.m_Pointer)
		{
			other.m_Pointer = nullptr;
		}
		T* Get() const
		{
			if (!m_Pointer) return nullptr;

			if (m_Pointer->IsPromotion())
			{
				auto* promotion = static_cast<PGC_Promotion*>(m_Pointer);
				ASSERTM(promotion, "IsPromotion returned true but cast to PGC_Promotion failed");
				m_Pointer = promotion->GetFinalCopy();
				promotion->Free();  // Put the promotion object back on the free list for reuse.
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
				static_cast<PGC_Promotion*>(m_Pointer)->Free();
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
		void RequestPromotion(PGC_Transaction* p_Destination)
		{
			if (!m_Pointer || m_Pointer->IsPromotion())
				return;

			PGC_Transaction* source = m_Pointer->GetTransaction();
			PromotionStrategy strategy = source->GetPromotionStrategy();
			if (!source) return;

			switch (strategy)
			{
			case PromotionStrategy::Backstabbing:
				// modifies *p_Pointer (back-reference) via side effect
				PGC_Promotion::CheckRequestPromotion(&m_Pointer, source, p_Destination, strategy);
				break;

			case PromotionStrategy::DoubleDipping:
				PGC_TypeCheck* typeCheck = PGC_Promotion::CheckRequestPromotion(&m_Pointer, source, p_Destination, strategy);
				if (typeCheck != m_Pointer)
				{
					m_Pointer = typeCheck;
				}
				break;
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