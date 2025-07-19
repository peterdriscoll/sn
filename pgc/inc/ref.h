#pragma once

#include "pgc_base.h"
#include "pgc_transaction.h"
#include "memberref.h"

#include <iostream>

namespace PGC
{
	template <class T>
	class Ref
	{
	private:
		PGC_Transaction* m_Transaction = nullptr;
		MemberRef<T> m_Member;

	public:
		Ref()
			: m_Transaction(PGC_Transaction::TopTransaction())
		{
		}
		Ref(T* p_Pointer)
			: m_Transaction(PGC_Transaction::TopTransaction())
			, m_Member(p_Pointer, m_Transaction)
		{
		}
		Ref(T* p_Pointer, PGC_Transaction* p_Transaction)
			: m_Transaction(p_Transaction)
			, m_Member(p_Pointer, p_Transaction)
		{
		}
		Ref(const Ref<T>& other)
			: m_Transaction(other.m_Transaction)
			, m_Member(other.m_Member)
		{
			// Re-request promotion in case this copy is used in a different context
			m_Member.RequestPromotion(m_Transaction);
		}
		Ref(Ref<T>&& other) noexcept
			: m_Transaction(other.m_Transaction)
			, m_Member(std::move(other.m_Member))
		{
			other.m_Transaction = nullptr;
		}
		Ref<T>& operator=(const Ref<T>& other)
		{
			if (this != &other)
			{
				m_Transaction = other.m_Transaction;
				m_Member = other.m_Member;
				m_Member.RequestPromotion(m_Transaction);
			}
			return *this;
		}
		Ref<T>& operator=(Ref<T>&& other) noexcept
		{
			if (this != &other)
			{
				m_Transaction = other.m_Transaction;
				m_Member = std::move(other.m_Member);
				other.m_Transaction = nullptr;
			}
			return *this;
		}
		T* Get() const
		{
			return m_Member.Get();
		}
		void Set(T* p_Pointer)
		{
			m_Member.Set(p_Pointer, m_Transaction);
		}
		T * operator->() const
		{
			T* result = Get();
			ASSERTM(result, "Ref::operator-> - result is null");
			return result;
		}
	};
}
