//  pgc_transaction.h: interface for the PGC_Transaction class.
//
//  Description:
//      A PGC_Transaction is a memory allocation heap that is available for the life of the transaction.
//      However when a PGC_Transaction, all memory pointed to another transaction is promoted (copied)
//      to that transaction.
//
//  Usage:
//      Any class that inherits from PGC_Base inherits an overridden new operator.  new will allocate an
//      instance in the top most PGC_Transaction.  Also new (transaction) classname() may be used to select
//      a particular transaction.
//
//      See PGC_Base for the requirements for the usage of this class.
//
//  Example:
//      {
//          PGC_Transaction transaction;
//          ...
//          Some mainly self contained code (ie. a transaction) for which memory may mostly be deleted on completion.
//          ...
//      }
// 
//////////////////////////////////////////////////////////////////////

#if !defined(PGC_TRANSACTION_H_INCLUDED)
#define PGC_TRANSACTION_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "exp_ctrl_pgc.h"
#include "pgc_block.h"
#include "pgc_promotion.h"

#include <vector>
#include <mutex>
using namespace std;

#pragma warning(disable: 4251)

#define MAX_TRANSACTION_STACK 10000
#define NEW_TRANSACTION_THRESHOLD 50000

namespace PGC
{
	class PGC_Base;
	class PGC_Task;

	typedef vector<PGC_Task *> TaskList;

	class PGC_EXPORT PGC_Transaction
	{
	public:
		PGC_Transaction();
		virtual ~PGC_Transaction();

		void *Allocate(size_t p_size);
		bool *GetLiveTransactionPointer();
		void EndTransaction();

		void RegisterForDestruction(PGC_Base *p_Base);
		static PGC_Transaction *RegisterLastForDestruction(PGC_Base *p_Base);

		bool Dieing();

		PGC_Base *FindOwner(void *p_member);

		void SubmitTask(PGC_Task *p_Task);
		PGC_Task *TopTask();

		static PGC_Transaction *TopTransaction();

		static void StartStackTransaction();
		static void EndStackTransaction();

		size_t NetMemoryUsed();
		size_t GrossMemoryUsed();
		static size_t TotalNetMemoryUsed();
		static size_t TotalGrossMemoryUsed();

		static void AddTotalNetMemorySize(long memory);
		static void AddTotalGrossMemorySize(long memory);

		static void ResetNetMemoryUsed();
		static void ResetGrossMemoryUsed();

		void CollectGarbage();

		// Multi threading
		static void MultiThreaded(bool p_enable);
		bool Running();
		static void StartProcessing(PGC_Transaction *);
		void Process();
		void Complete();

	private:
		void ReleaseBlocks();

		static long m_NextThreadNum;

		size_t m_NetMemoryUsed;
		static size_t m_TotalNetMemoryUsed;
		static size_t m_TotalGrossMemoryUsed;

		static size_t m_TransactionDepth;
		static size_t m_NewTransactionThreshold;
		size_t m_StartDepth;

		PGC_Block *m_FirstBlock;
		PGC_Block *m_CurrentBlock;
		bool *m_LiveTransaction;
		bool m_Dieing;
		PGC_Transaction *m_LastTopTransaction;

		// Multi threading
		static bool m_MultiThreaded;
		TaskList m_TaskList;
		mutex m_Mutex;
		thread *m_ProcessThread;
	};
}

#endif // !defined(PGC_TRANSACTION_H_INCLUDED)
