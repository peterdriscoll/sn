//  pgc_base.h: interface for the PGC_Base class.
//
//  Description:
//      A base class for classes that will use the Promotional Garbage Collector (PGC).
//      The PGC manages the allocation and deallocation of memory so that,
//      * Memory is automatically recovered when it is inaccessible.
//      * Memory is allocated contiguously without fragmentation.
//      * Memory may be quickly recovered at the end of a transaction.
//      * Memory is recovered at predictable times.
//
//  Usage:
//      The requirments for using the PGC are,
//      * Inherit from PGC_Base
// to be completed
//      * Call the macro at the start of the body of the class, giving the class name.
//      * Implement PromoteMembers
//      ** Call REQUESTPROMOTION on each member.
//      * Call REQUESTPROMOTION on each member afte the value is set with a new pointer.
//
//  Example:
//      #include "pgc_base.h"
//
//      class ExampleClass : public PGC_Base
//      {
//          PGC_CLASS(ExampleClass);  // Implements the RetrieveDescriptor method,
//
//          ExampleClass1 *m_Member1;
//          ExampleClass1 *m_Member2;
//          ExampleClass2 *m_Member3;
//          ...
//      private:
//          void PromoteMembers()
//          {
//	            REQUESTPROMOTION(m_Member1);
//	            REQUESTPROMOTION(m_Member2);
//	            REQUESTPROMOTION(m_Member3);
//          }
//
//          void SetMember1(ExampleClass1 *p_value)
//          {
//          	m_Member1 = p_value;
//          	REQUESTPROMOTION(m_Member1);
//          }
//          void SetMember2(ExampleClass1 *p_value)
//          {
//          	m_Member2 = p_value;
//          	REQUESTPROMOTION(m_Member2);
//          }
//          void SetMember3(ExampleClass2 *p_value)
//          {
//          	m_Member3 = p_value;
//          	REQUESTPROMOTION(m_Member3);
//          }
//          ...
//    }
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "exp_ctrl_pgc.h"
#include <string>
#include "pgc_typecheck.h"

#ifndef NDEBUG
#define ASSERTM(Expr, Msg) \
    ::PGC::assertm(#Expr, Expr, __FILE__, __LINE__, Msg)
#else
#define ASSERTM(Expr, Msg) \
    do { if (!(Expr)) PGC::logwarning("ASSERTM ignored in release build: " Msg); } while (0)
#endif

#define FORCE_ASSERTM(msg) ASSERTM(false, msg)
#define PGC_DEBUG_NEXT_CALC
#define PGC_CLASS(T)                                                   \
    public:                                                              \
		virtual void RetrieveDescriptor(char *&p_Pointer, long &p_Size) const  \
		{                                                                \
			p_Pointer = (char *) this;                                   \
			p_Size = sizeof(T);                                          \
		}

#define REQUESTPROMOTION(member) RequestPromotion((PGC::PGC_TypeCheck **) &member)
//#define PROMOTENOW(member) RequestPromotion((PGC::PGC_TypeCheck **) &member)
#define PROMOTENOW(member) PromoteNow((PGC::PGC_TypeCheck **) &member)

#define NULL 0


namespace PGC
{
	PGC_EXPORT void logwarning(const std::string& msg);

	PGC_EXPORT void assertm(const char* expr_str, bool expr, const char* file, const int line, const std::string &msg);

	typedef void OnErrorHandler(bool p_Err, const std::string& p_Description);

	class PGC_Exception : public std::exception {
		std::string m_Message;
	public:
		PGC_Exception(const std::string& msg)
			: m_Message(msg) {}

		const char* what() const noexcept override
		{ 
			return m_Message.c_str();
		}
	};

	class PGC_Transaction;

	//  const long typeid = TYPEID

	class PGC_EXPORT PGC_Base
		: public PGC_TypeCheck
	{
	public:
		PGC_Base();
		PGC_Base(PGC_Transaction & p_Transaction);
		PGC_Base(const PGC_Base& p_Other);
		PGC_Base(PGC_Base&& p_Other) noexcept;
		virtual ~PGC_Base();

		//	Name:  PromoteMembers
		//  Description:
		//      Traverse each member variable which is a pointer to another object and promote it to the new transaction. 
		//		Promotion means copying the object into memory allocated for another transaction.
		//	Usage:
		//		Inheriting class to override.
		//		For each member promote it to the new transaction.
		//		    RequestPromotion(m_MemberPointer1);
		//		    RequestPromotion(m_MemberPointer2);
		//		    ...
		virtual void PromoteMembers();

		virtual void RegisterMembers() {};
		void RegisterMember(PGC_Base* p_Base);

		//  Call this method after assigning a new pointer to a member.
		//  Request prompotion for p_Base(copying to the same transaction as this object) when the transaction the p_Base object lives in is destroyed.

		bool IsPromotedMarker() const;

		virtual PGC_Base *Clone(PGC_Transaction &p_Transaction);
		virtual PGC_Base* CloneTo(void* memory) const;

		// NEW: move this object into pre-allocated memory (uses move-ctor)
		// Post-condition: *this is left in a valid-but-unspecified "moved-from" state.
		virtual PGC_Base* MoveTo(void* memory)
		{
			// Fallback to copy; classes that want true move will override via PGC_MOVE_TO(T)
			return CloneTo(memory);
		}

		void RequestPromotion(PGC_TypeCheck** p_Base);

		void PromoteNow(PGC_TypeCheck** p_Base);

		PGC_Transaction* GetTransaction() override;
		const PGC_Transaction* GetTransaction() const override;
		void SetTransaction(PGC_Transaction*);
		PGC_TypeCheck*GetPromotedCopy() override;
		void SetPromotedCopy(PGC_TypeCheck *p_Base) override;

		PGC_Base* GetNext(const void* p_End) const noexcept;
		void SetNext(PGC_Base *p_Base);

		virtual void RetrieveDescriptor(char*& p_Pointer, long& p_Size) const = 0;
		size_t Size() const;

		virtual PGC_TypeCheck* GetLogicalPointer() override
		{
			return this;
		}
		virtual PGC_Transaction* GetLogicalOwnerTransaction() override
		{ 
			return GetTransaction();
		}
		virtual PGC_Promotion* GetLogicalPromotion() override
		{
			return nullptr;
		}

		void* operator new(size_t, void* p) noexcept { return p; };
		void *operator new(size_t p_size, PGC_Transaction &p_Transaction);
		void *operator new(size_t p_size);
		void operator delete(void*, void*) noexcept { /* nothing to do */ }
		void operator delete(void *p_Object, PGC_Transaction &p_Transaction);
		void operator delete(void *p_Object);
	private:
		PGC_Transaction* m_Transaction = nullptr;
		PGC_TypeCheck* m_PromotedCopy = nullptr;
	#ifdef PGC_DEBUG_NEXT_CALC
		PGC_Base* m_Next = nullptr;
	#endif
	};

	class WithoutPGC {
		void* m_Data;
		short m_Extra;
		virtual ~WithoutPGC() {} // vtable included
	};

	class WithPGC : public PGC::PGC_Base {
		void* m_Data;
		short m_Extra;
		virtual ~WithPGC() {} // vtable included
	};

#define PGC_OVERHEAD (sizeof(::PGC::WithPGC) - sizeof(::PGC::WithoutPGC))

}