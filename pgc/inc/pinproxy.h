#pragma once

#include <type_traits>
struct PGC_Transaction;

namespace PGC {

	// Internal template; generic over pointer type (T* or const T*)
	template <class Ptr>
	class PinProxy {
	public:
		using element_type = std::remove_pointer_t<Ptr>;

		explicit PinProxy(Ptr p) noexcept
			: m_pointer(p), m_transaction(const_cast<PGC::PGC_Transaction*>(p->GetTransaction()))
		{
			if (m_transaction) m_transaction->Pin();
		}
		~PinProxy() noexcept 
		{ 
			if (m_transaction) m_transaction->Unpin(); 
		}

		Ptr operator->() const noexcept 
		{
			return m_pointer; 
		}

		decltype(auto) operator*() const noexcept 
		{
			return *m_pointer; 
		}

		Ptr Get() const noexcept 
		{
			return m_pointer;
		}

		PinProxy(const PinProxy&) = delete;
		PinProxy& operator=(const PinProxy&) = delete;

		PinProxy(PinProxy&& o) noexcept
			: m_pointer(o.m_pointer)
			, m_transaction(o.m_transaction)
		{ 
			o.m_pointer = nullptr; 
			o.m_transaction = nullptr;
		}

		PinProxy& operator=(PinProxy&& o) noexcept
		{
			if (this != &o) 
			{
				if (m_transaction) m_transaction->Unpin();
				m_pointer = o.m_pointer; 
				m_transaction = o.m_transaction;
				o.m_pointer = nullptr;
				o.m_transaction = nullptr; 
			}
			return *this;
		}

	private:
		Ptr              m_pointer = nullptr;
		PGC_Transaction* m_transaction = nullptr;
	};

	// Public, nice names:
	template <class T> using Pin = PinProxy<T*>;
	template <class T> using ConstPin = PinProxy<const T*>;

} // namespace PGC
