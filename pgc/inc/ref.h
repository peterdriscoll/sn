#if !defined(REF_INCLUDED)
#define REF_INCLUDED

#pragma once

#include "pgc_base.h"

#include <iostream>

namespace PGC
{
	class RefBase
	{
	protected:
		void *m_Object;
	public:
		RefBase()
			: m_Object(0)
		{
		}
		RefBase(void *p_Object)
			: m_Object(p_Object)
		{
		}

		void * Ptr() const
		{
			return m_Object;
		}
	};

	template <class T>
	class SRef : public RefBase
	{
	private:
		bool *m_LiveTransaction;
	public:
		SRef()
			: m_LiveTransaction(0)
		{
		}
		SRef(const RefBase &p_Other)
			: RefBase(p_Other.Ptr())
		{
			SetLiveTransaction();
		}
		SRef(T *p_Object)
			: RefBase(p_Object)
		{
			SetLiveTransaction();
		}
		SRef<T> operator ==(const RefBase &p_Other)
		{
			return m_Object == p_Other.m_Object;
		}
		SRef<T> operator =(T *p_Object)
		{
			m_Object = p_Object;
			SetLiveTransaction();
			return *this;
		}
		SRef<T> operator =(const RefBase &p_Object)
		{
			m_Object = dynamic_cast<T>(p_Other.Ptr());
			SetLiveTransaction();
			return *this;
		}
		T * operator->() const
		{
			ASSERTM(m_Object, "Ref::operator-> - pointer is NULL");
			ASSERTM(*m_LiveTransaction, "SRef::operator-> - Transaction is ended.");
			return static_cast<T *>(m_Object);
		}

		operator T*()
		{
			ASSERTM(m_Object, "Ref::operator-> - pointer is NULL");
			ASSERTM(*m_LiveTransaction, "SRef::operator-> - Transaction is ended.");
			return (T *)m_Object;
		}

		T * Ptr() const
		{
			ASSERTM(*m_LiveTransaction, "SRef::operator-> - Transaction is ended.");
			return (T *)m_Object;
		}
	private:
		void SetLiveTransaction()
		{
			m_LiveTransaction = 0;
			if (m_Object)
			{
				PGC_Transaction *transaction = static_cast<T *>(m_Object)->GetTransaction();
				if (transaction)
				{
					m_LiveTransaction = transaction->GetLiveTransactionPointer();
				}
			}
		}
	};

	enum VariableClass
	{
		Unknown,
		Member,
		Stack,
		Static
	};

	template <class T, enum VariableClass V = Unknown>
	class Ref
	{
	private:
		T                 *m_Pointer;
		PGC_Transaction   *m_Transaction;

		void RequestPromotion()
		{
			if (m_Pointer)
			{
				PGC_Base *owner = m_Pointer->GetTransaction()->FindOwner((void *)&m_Pointer);
				if (owner)
				{
					owner->REQUESTPROMOTION(m_Pointer);
				}
			}
		}

		void CaptureTransaction()
		{
			if ((V == Unknown) || (V == Member))
			{
				m_Transaction = PGC_Des::FindTransaction((void *) this);
				if (m_Transaction == NULL)
				{
					return;
				}
			}
			if ((V == Unknown) || (V == Stack))
			{
				m_Transaction = TopTransaction();
				char * transaction = (char *)m_Transaction;
				//            if (((char *)m_Transaction < 

			}
			if ((V == Unknown) || (V == Static))
			{
				m_Transaction = BaseTransaction();
			}
		}

	public:
		Ref()
		{
		}
		Ref(T *p_Pointer)
			: m_Pointer(p_Pointer)
		{
			PGC_Base::CaptureMember((PGC_Base **)&m_Pointer);
			RequestPromotion();
		}
		Ref<T> operator =(T *p_Pointer)
		{
			m_Pointer = p_Pointer;
			RequestPromotion();
			return *this;
		}
		void Set(T *p_Pointer)
		{
			m_Pointer = p_Pointer;
		}
		T * operator->() const
		{
			ASSERTM(m_Pointer, "Ref::operator-> - pointer is NULL");
			return m_Pointer;
		}

		operator T*() const
		{
			ASSERTM(m_Pointer, "Ref::operator T* - pointer is NULL");
			return m_Pointer;
		}
	};

	template <class T>
	class MRef
	{
	private:
		T *m_Pointer;

		void RequestPromotion()
		{
			if (m_Pointer)
			{
				PGC_Base *owner = m_Pointer->GetTransaction()->FindOwner((void *)&m_Pointer);
				if (owner)
				{
					owner->REQUESTPROMOTION(m_Pointer);
				}
			}
		}

	public:
		MRef()
		{
		}
		MRef(T *p_Pointer)
			: m_Pointer(p_Pointer)
		{
			PGC_Base::CaptureMember((PGC_Base **)&m_Pointer);
			RequestPromotion();
		}
		Ref<T> operator =(T *p_Pointer)
		{
			m_Pointer = p_Pointer;
			RequestPromotion();
			return *this;
		}
		void Set(T *p_Pointer)
		{
			m_Pointer = p_Pointer;
		}
		T * operator->() const
		{
			ASSERTM(m_Pointer, "Ref::operator-> - pointer is NULL");
			return m_Pointer;
		}

		operator T*() const
		{
			ASSERTM(m_Pointer, "Ref::operator T* - pointer is NULL");
			return m_Pointer;
		}
	};
}

#endif // !defined(REF_INCLUDED)
