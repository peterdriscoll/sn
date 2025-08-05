//  pgc_stacktransaction.h: interface for the PGC_StackTransaction class.
//
//  Description:
//      This class may be used to create a transaction (see PGC_Transaction), but only if the   
//      existing top transaction exceeds a size threshhold.  There is a cost of 1 byte in using this.
//      This is equivalent to calling,
//      * StartStackTransaction at the start of a block.
//      * EndStackTransaction at the end of a block.
//
//  Usage:
//      PGC_StackTransaction is used to declare a transaction at the start of any recursive function.
//      It gives PGC a place to start a new transaction if the existing TopTransaction has more memory
//      allocated than a threshhold (see NEW_TRANSACTION_THRESHOLD).
//
//  Example 1
//
//      {
//           PGC_StackTransaction transaction;  // Costs 1 byte on the stack
//           ...
//      }
//
//  Example 2
//
//      {
//           PGC_Transaction::StartStackTransaction();  // No memory cost, but the programmer must add EndStackTransaction call.
//           ...
//           PGC_Transaction::EndStackTransaction();
//      }
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "pgc_transaction.h"

namespace PGC
{
	class PGC_StackTransaction
	{
	public:
		inline PGC_StackTransaction(PGC_User &p_User)
		{
			PGC_Transaction::StartStackTransaction(p_User);
		}
		inline ~PGC_StackTransaction()
		{
			PGC_Transaction::EndStackTransaction();
		}
	};
}