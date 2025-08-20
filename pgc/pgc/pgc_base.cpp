#include "pgc_base.h"
#include "pgc_transaction.h"
#include "pgc_promotion.h"

#include <iostream>
#include <typeinfo>       // operator typeid
#include <string>
#include <vector>
#include <unordered_map>

#include "pgc_pch.h"

#if defined(_WIN32)
#include <Windows.h>
#endif

namespace PGC
{
	void logwarning(const std::string& msg)
	{
		std::string full = "PGC WARNING: " + msg + "\n";

#if defined(_WIN32)
		OutputDebugStringA(full.c_str());
#endif

		std::cerr << full;
	}

	// assert with message.
	void assertm(const char* expr_str, bool expr, const char* file, const int line, const std::string& msg)
	{
		PGC_User* user = PGC_User::GetCurrentPGC_User();
		if (!expr && !user->ShouldRaiseError())
		{
			std::string full = "PGC ASSERTION FAILED\n";
			full += "Expression: "; full += expr_str;
			full += "\nMessage   : "; full += msg;
			full += "\nFile      : "; full += file;
			full += ":" + std::to_string(line) + "\n";
			if (user)
			{
				user->GetErrorHandler()(!expr, full);
			}
		}
	}

	void DefaultErrorHandler(bool p_Err, const std::string& p_Description)
	{
#ifdef _WIN32
		::OutputDebugStringA(p_Description.c_str());
#endif
		std::cerr << p_Description << std::flush;
		if (p_Err)
			std::abort();
	};

	size_t HashCode(const type_info& info)
	{
		return info.hash_code();
	}

	unsigned Hasher(const unsigned& key)
	{
		return key;
	}

	PGC_Base::PGC_Base()
	{
		m_Transaction = PGC_Transaction::RegisterLastForDestruction(this);
	}

	PGC_Base::PGC_Base(PGC_Transaction& p_Transaction)
		: m_Transaction(&p_Transaction)
	{
		PGC_Transaction::RegisterLastForDestruction(this);
	}
	PGC_Base::PGC_Base(const PGC_Base& other)
		: m_Transaction(PGC_Transaction::RegisterLastForDestruction(this))
	{
	}
	PGC_Base::PGC_Base(PGC_Base&& other) noexcept
		: m_Transaction(PGC_Transaction::RegisterLastForDestruction(this))
	{
	}
	PGC_Base::~PGC_Base()
	{

	}

	PGC_Base* PGC_Base::Clone(PGC_Transaction& /*p_Transaction*/)
	{
		return NULL;
	}

	PGC_Base* PGC_Base::CloneTo(void* memory) const
	{
		return nullptr;
	}

	void PGC_Base::RequestPromotion(PGC_TypeCheck** p_Base)
	{
		PGC_TypeCheck* typeCheck = *p_Base;
		if (!typeCheck)
		{
			return;
		}
		ASSERTM(!typeCheck->IsPromotion(), "Backstabbing promotion attempted on a DoubleDipping object");

		if (typeCheck && !typeCheck->IsPromotion())
		{
			PGC_Promotion::CheckRequestPromotion(p_Base, typeCheck->GetTransaction(), GetTransaction(), PromotionStrategy::Backstabbing);
		}
		*p_Base = typeCheck;
	}

	void PGC_Base::PromoteNow(PGC_TypeCheck** p_Base)
	{
		if (!p_Base)
			return;

		PGC_TypeCheck* typeCheck = *p_Base;
		if (!typeCheck)
			return;

		// Backstabbing only; refuse DoubleDipping wrappers.
		ASSERTM(!typeCheck->IsPromotion(), "PromoteNow: backstabbing attempted on a DoubleDipping object");

		PGC_Transaction* source = typeCheck->GetTransaction();
		PGC_Transaction* destination = GetTransaction(); // owning object's transaction

		// Bail on no-op/unsafe cases.
		if (!source || !destination)
			return;
		if (source == destination)
			return;
		if (destination->Dieing())
			return;
		if (source->IsStatic())
			return;

		// Perform immediate promotion; backstabbing updates *p_Base in place.
		PGC_Promotion promotion;
		promotion.Create(
			p_Base,
			destination,
			PGC::PromotionStrategy::Backstabbing
		);
		promotion.Promote();
	}

	bool PGC_Base::IsPromotedMarker() const
	{
		return m_PromotedCopy != nullptr;
	}

	PGC_Transaction * PGC_Base::GetTransaction()
	{
		return m_Transaction;
	}

	void PGC_Base::SetTransaction(PGC_Transaction *p_Transaction)
	{
		m_Transaction = p_Transaction;
	}

	PGC_TypeCheck* PGC_Base::GetPromotedCopy()
	{
		return m_PromotedCopy;
	}

	void PGC_Base::SetPromotedCopy(PGC_TypeCheck*p_Base)
	{
		m_PromotedCopy = p_Base;
	}

	PGC_Base *PGC_Base::GetNext()
	{
		return m_Next;
	}

	void PGC_Base::SetNext(PGC_Base *p_Base)
	{
		m_Next = p_Base;
	}

	size_t PGC_Base::Size()
	{
		long l_size;
		char *l_pointer;
		RetrieveDescriptor(l_pointer, l_size);
		return l_size;
	}

	void PGC_Base::PromoteMembers()
	{
	}

	void PGC_Base::RegisterMember(PGC_Base* p_Base)
	{
	}


	void * PGC_Base::operator new(size_t p_size, PGC_Transaction &p_Transaction)
	{
		return p_Transaction.CreateNew(p_size);
	}

	void * PGC_Base::operator new(size_t p_size)
	{
		return PGC_Transaction::TopTransaction()->CreateNew(p_size);
	}

	void PGC_Base::operator delete(void * /* p_Object */, PGC_Transaction & /* p_Transaction */)
	{
	}

	void PGC_Base::operator delete(void * /* p_Object */)
	{
	}
}
