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

#if !defined(PGC_BASE_H_INCLUDED)
#define PGC_BASE_H_INCLUDED

#pragma once

#include "exp_ctrl_pgc.h"
#include <string>
using namespace std;

namespace PGC
{
	PGC_EXPORT void assertm(char* expr_str, bool expr, char* file, int line, string msg);

	#ifndef NDEBUG
	#define ASSERTM(Expr, Msg) \
		PGC::assertm(#Expr, Expr, __FILE__, __LINE__, Msg)
	#else
	#define ASSERTM(Expr, Msg) ;
	#endif

	#define FORCE_ASSERTM(msg) ASSERTM(false, msg)

	class PGC_Transaction;

	//  const long typeid = TYPEID

#define PGC_CLASS(T)                                                     \
    public:                                                              \
		virtual void RetrieveDescriptor(char *&p_Pointer, long &p_Size)  \
		{                                                                \
			p_Pointer = (char *) this;                                   \
			p_Size = sizeof(T);                                          \
		}

#define REQUESTPROMOTION(member) RequestPromotion((PGC::PGC_Base **) &member)

#define OVERHEAD (sizeof(void *) + sizeof(short))

#define NULL 0

	class PGC_EXPORT PGC_Base
	{
	public:
		PGC_Base();
		PGC_Base(PGC_Transaction & p_Transaction);
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

		//  Call this method after assigning a new pointer to a member.
		//  Request prompotion for p_Base(copying to the same transaction as this object) when the transaction the p_Base object lives in is destroyed.
		void RequestPromotion(PGC_Base **p_Base);

		virtual PGC_Base *Clone(PGC_Transaction &p_Transaction);

		PGC_Transaction * GetTransaction();
		void SetTransaction(PGC_Transaction *);
		PGC_Base *GetNewCopyBase();
		void SetNewCopyBase(PGC_Base *p_Base);

		PGC_Base *GetNext();
		void SetNext(PGC_Base *p_Base);

		virtual void RetrieveDescriptor(char *&p_Pointer, long &p_Size) = 0;
		size_t Size();

		void *operator new(size_t p_size, PGC_Transaction &p_Transaction);
		void *operator new(size_t p_size);
		void operator delete(void *p_Object, PGC_Transaction &p_Transaction);
		void operator delete(void *p_Object);
	private:
		// Warning:  These variables are used in a strange way.
		// if m_Offset > 0 then m_Transaction stores the transaction for this object.
		// if m_Offset < 0 then m_Transaction stores the new location for this object, during a promotion.
		void *m_Transaction;

		// The sign of m_Offset is used as above.
		// The absolute value is the number of bytes to the next object allocated in a block.
		// This linked list is used for calling destructors.
		short m_Offset;

		// Apology: The strange arrangement above saves 6 bytes.  There are actually 3 fields compressed into 6 bytes.
		// * The Transaction
		// * The new location that the object has been copied to during a promotion.
		// * The next object in the list of objects to be destroyed when a block is destroyed.
	};
}

#endif // !defined(PGC_BASE_H_INCLUDED)
