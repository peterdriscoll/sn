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
#pragma once

#include "exp_ctrl_pgc.h"
#include "pgc_block.h"
#include "pgc_promotion.h"
#include "pgc_promotionstrategy.h"
#include "pgc_typecheck.h"

#include <vector>
#include <mutex>
#include <memory>
#include <atomic>

#pragma warning(disable: 4251)

#define MAX_TRANSACTION_STACK 10000
#define NEW_TRANSACTION_THRESHOLD 50000

namespace PGC
{
	class PGC_Base;
	class PGC_Task;

	typedef std::vector<PGC_Task *> TaskList;

	class PGC_EXPORT PGC_Transaction : public PGC_TypeCheck
	{
	public:
		PGC_Transaction() = delete; // Disable default constructor

		PGC_Transaction(PGC_User &p_User, bool p_IsStatic = false, PromotionStrategy p_PromotionStrategy = g_DefaultPromotionStrategy);
		virtual ~PGC_Transaction();

		std::mutex m_Mutex;

		uint32_t GetID() const { return m_ID; }

		PromotionStrategy GetPromotionStrategy() const;

		PGC_User* GetUser() const;

		inline void* Allocate(std::size_t size) {
			return Allocate(size, alignof(std::max_align_t));
		}
		void* Allocate(size_t p_size, std::size_t align);
		std::shared_ptr<bool> GetLiveTransactionPointer();
		bool IsDescendantOf(const PGC_Transaction* other) const noexcept;
		virtual void EndTransaction();

		void RegisterForDestruction(PGC_Base *p_Base);
		void *CreateNew(size_t p_size);
		static PGC_Transaction *RegisterLastForDestruction(PGC_Base *p_Base);

		bool IsDying();

		PGC_Base *FindOwner(void *p_member);

		void SubmitTask(PGC_Task *p_Task);
		PGC_Task *TopTask();

		static PGC_Transaction *TopTransaction();

		static void StartStackTransaction(PGC_User &p_User);
		static void EndStackTransaction();

		size_t NetMemoryUsed();
		size_t GrossMemoryUsed();

		void CollectGarbage();

		// Multi threading
		static void MultiThreaded(bool p_enable);
		bool Running();
		static void StartProcessing(PGC_Transaction *);
		void Process();
		void Finish();

		static void RegisterInWebServer();
		static bool InWebServer();

		virtual void PromoteExternals(PGC_Transaction *p_Direction);

		bool IsStatic();

		virtual PGC_TypeCheck* GetLogicalPointer() override;
		virtual PGC_Transaction* GetLogicalOwnerTransaction() override;
		virtual PGC_Promotion* GetLogicalPromotion() override;
		void Pin();
		void Unpin();
	private:

		void CallFinalizersForAllBlocks();
		void CallDestructorsForAllBlocks();
		void DeleteBlocks();
		void ReleaseBlocks();

		PromotionStrategy m_PromotionStrategy;

		static std::atomic<uint32_t> s_NextID;

		PGC_User* m_User;
		static long m_NextThreadNum;

		size_t m_NetMemoryUsed;

		static size_t m_TransactionDepth;
		static size_t m_NewTransactionThreshold;
		size_t m_StartDepth;

		PGC_Block *m_FirstBlock;
		PGC_Block *m_CurrentBlock;
		std::shared_ptr<bool> m_LiveTransaction;
		bool m_Dieing;
		PGC_Transaction *m_LastTopTransaction;
		bool m_IsStatic;

		// Multi threading
		static bool m_MultiThreaded;
		TaskList m_TaskList;
		std::thread *m_ProcessThread;

		uint32_t m_ID;
	};
}