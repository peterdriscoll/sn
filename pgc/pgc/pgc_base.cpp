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
		if (!expr)
		{
			PGC_User* user = PGC_User::GetCurrentPGC_User();
			if (user)
			{
				if (!user->ShouldRaiseError())
				{
					std::string full = "PGC ASSERTION FAILED\n";
					full += "Expression: "; full += expr_str;
					full += "\nMessage   : "; full += msg;
					full += "\nFile      : "; full += file;
					full += ":" + std::to_string(line) + "\n";
					user->GetErrorHandler()(!expr, full);
				}
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
		SetTransaction(PGC_Transaction::RegisterLastForDestruction(this));
	}

	PGC_Base::PGC_Base(PGC_Transaction& p_Transaction)
	{
		SetTransaction(&p_Transaction);
		PGC_Transaction::RegisterLastForDestruction(this);
	}
	PGC_Base::PGC_Base(const PGC_Base& other)
	{
		SetTransaction(PGC_Transaction::RegisterLastForDestruction(this));
	}
	PGC_Base::PGC_Base(PGC_Base&& other) noexcept
	{
		SetTransaction(PGC_Transaction::RegisterLastForDestruction(this));
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
			PromotionStrategy::Backstabbing
		);
		promotion.Promote();
	}

	bool PGC_Base::IsPromotedMarker() const
	{
		return HoldsPromotedCopy();
	}

	bool PGC_Base::GetCallDestructor() const noexcept
	{
		return m_CallDestructor;
	}
	void PGC_Base::SetCallDestructor(bool p_CallDestructor) noexcept
	{
		m_CallDestructor = p_CallDestructor;
	}

	// Query
	bool PGC_Base::HoldsPromotedCopy() const noexcept
	{
		return is_tagged(m_Link);
	}

	// Accessors (return nullptr if not holding that variant)
	PGC_Transaction* PGC_Base::GetTransaction() noexcept
	{
		return const_cast<const PGC_Base*>(this)->GetTransaction();
	}

	PGC_Transaction* PGC_Base::GetTransaction() const noexcept
	{
		PGC_Transaction* result = HoldsPromotedCopy() 
			? nullptr
			: reinterpret_cast<PGC_Transaction*>(m_Link);
	#ifdef PGC_DEBUG_UNION
		ASSERTM(result == m_LogicalTransaction, "PGC_Base::GetTransaction(): m_LogicalTransaction out of sync");
	#endif
		return result;
	}
	PGC_TypeCheck* PGC_Base::GetPromotedCopy() const noexcept
	{
		PGC_TypeCheck* result = HoldsPromotedCopy() 
			? reinterpret_cast<PGC_TypeCheck*>(rm_tag(m_Link))
			: nullptr;
	#ifdef PGC_DEBUG_UNION
		ASSERTM(result == m_LogicalPromotedCopy, "PGC_Base::GetPromotedCopy(): m_LogicalPromotedCopy out of sync");
	#endif
		return result;
	}

	// Mutators
	void PGC_Base::SetTransaction(PGC_Transaction* t) noexcept
	{
		// Keep logical mirrors
		m_LogicalTransaction = t;
		m_LogicalPromotedCopy = nullptr;

		// Store untagged pointer (or 0)
		m_Link = reinterpret_cast<uintptr_t>(t);
	}

	void PGC_Base::SetPromotedCopy(PGC_TypeCheck* p) noexcept {
		m_LogicalTransaction = nullptr;
		m_LogicalPromotedCopy = p;

		// Null stays null; otherwise tag bit marks “promoted copy”
		m_Link = p ? add_tag(p) : 0;
	}

	// Clear to empty
	void PGC_Base::ResetLink() noexcept {
		m_Link = 0;
		m_LogicalTransaction = nullptr;
		m_LogicalPromotedCopy = nullptr;
	}


	static constexpr size_t kAllocAlign = alignof(std::max_align_t);
	static inline size_t align_up(size_t n, size_t a = kAllocAlign) {
		return (n + a - 1) & ~(a - 1);
	}

	PGC_Base* PGC_Base::GetNext(const void* p_end) const noexcept
	{
		const std::byte* self = reinterpret_cast<const std::byte*>(this);
		const size_t span = align_up(Size());            // ensure Size() is total, unshared span
		const std::byte* nextp = self + span;

		PGC_Base* next = nullptr;
		if (!p_end || nextp < reinterpret_cast<const std::byte*>(p_end)) {
			// launder is the standards-correct way when objects are created via placement new
			next = std::launder(reinterpret_cast<PGC_Base*>(const_cast<std::byte*>(nextp)));
		}

#ifdef PGC_DEBUG_NEXT_CALC
		// If you also maintain m_Next in debug, sanity-check agreement.
		ASSERTM(m_Next == next, "Layout-computed next disagrees with linked next");
#endif
		return next;
	}

	void PGC_Base::SetNext(PGC_Base *p_Base)
	{
	#ifdef PGC_DEBUG_NEXT_CALC
		m_Next = p_Base;
	#endif
	}

	size_t PGC::PGC_Base::Size() const
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
