#pragma once

#include "pgc_base.h"
#include "pgc_transaction.h"

#include <iostream>

namespace PGC
{
	class RefBase
	{
	protected:
		void* m_Object;
	public:
		RefBase()
			: m_Object(0)
		{
		}
		RefBase(void* p_Object)
			: m_Object(p_Object)
		{
		}

		void* Ptr() const
		{
			return m_Object;
		}
	};

	template <class T>
	class SRef : public RefBase
	{
	private:
		std::shared_ptr<bool> m_LiveTransaction;
	public:
		SRef()
			: m_LiveTransaction(nullptr)
		{
		}
		SRef(const RefBase& p_Other)
			: RefBase(p_Other.Ptr())
		{
			SetLiveTransaction();
		}
		SRef(T* p_Object)
			: RefBase(p_Object)
		{
			SetLiveTransaction();
		}
		bool operator ==(const RefBase& p_Other)
		{
			return m_Object == p_Other.m_Object;
		}
		SRef<T> operator =(T* p_Object)
		{
			m_Object = p_Object;
			SetLiveTransaction();
			return *this;
		}
		SRef<T> operator =(const RefBase& p_Object)
		{
			m_Object = dynamic_cast<T*>(p_Object.Ptr());
			SetLiveTransaction();
			return *this;
		}
		T* operator->() const
		{
			ASSERTM(m_Object, "Ref::operator-> - pointer is NULL");
			ASSERTM(*m_LiveTransaction, "SRef::operator-> - Transaction is ended.");
			return static_cast<T*>(m_Object);
		}

		operator T* ()
		{
			ASSERTM(m_Object, "Ref::operator-> - pointer is NULL");
			ASSERTM(*m_LiveTransaction, "SRef::operator-> - Transaction is ended.");
			return (T*)m_Object;
		}

		T* Ptr() const
		{
			ASSERTM(*m_LiveTransaction, "SRef::operator-> - Transaction is ended.");
			return (T*)m_Object;
		}
	private:
		void SetLiveTransaction()
		{
			m_LiveTransaction = nullptr;
			if (m_Object)
			{
				PGC_Transaction* transaction = static_cast<T*>(m_Object)->GetTransaction();
				if (transaction)
				{
					m_LiveTransaction = transaction->GetLiveTransactionPointer();
				}
			}
		}
	};
}