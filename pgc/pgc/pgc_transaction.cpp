#include "pgc_base.h"
#include "pgc_transaction.h"
#include "pgc_task.h"

#include "pgc_pch.h"

namespace PGC
{
	thread_local PGC_Transaction *m_TopTransaction = 0;
	thread_local PGC_Transaction *m_AllocatingTransaction = 0;
	thread_local long m_AllocatingDepth = 0;

	/*static*/ size_t PGC_Transaction::m_TotalNetMemoryUsed = 0;
	/*static*/ size_t PGC_Transaction::m_TotalGrossMemoryUsed = 0;

	/*static*/ size_t PGC_Transaction::m_TransactionDepth = 0;
	/*static*/ size_t PGC_Transaction::m_NewTransactionThreshold = NEW_TRANSACTION_THRESHOLD;

	/*static*/ bool PGC_Transaction::m_MultiThreaded = false;

	/*static*/ long PGC_Transaction::m_NextThreadNum = 0;

	/*static*/ std::atomic<uint32_t> PGC_Transaction::s_NextID{ 1 };
	
	thread_local bool m_InWebServer = false;

	/*static*/ void PGC_Transaction::RegisterInWebServer()
	{
		m_InWebServer = true;
	}

	bool PGC_Transaction::InWebServer()
	{
		return m_InWebServer;
	}

	PGC_Transaction::PGC_Transaction(bool p_IsStatic, PromotionStrategy p_PromotionStrategy)
		: m_FirstBlock(0)
		, m_CurrentBlock(0)
		, m_Dieing(false)
		, m_NetMemoryUsed(0)
		, m_ProcessThread(NULL)
		, m_IsStatic(p_IsStatic)
		, m_PromotionStrategy(p_PromotionStrategy)
		, m_LiveTransaction(std::make_shared<bool>(true))
	{
		if (m_InWebServer)
		{
			long dog = 10;
		}
		PGC_Transaction::AddTotalGrossMemorySize(sizeof(PGC_Transaction));
		m_ID = s_NextID.fetch_add(1, std::memory_order_relaxed);

		m_LastTopTransaction = m_TopTransaction;
		m_TopTransaction = this;
		m_StartDepth = m_TransactionDepth;

		if (m_MultiThreaded)
		{
			m_ProcessThread = new thread(StartProcessing, this);
		}
	}

	PGC_Transaction::~PGC_Transaction()
	{
		if (!m_Dieing)
		{
			EndTransaction();
		}
		PGC_Transaction::AddTotalGrossMemorySize(-((long) sizeof(PGC_Transaction)));
	}

	PromotionStrategy PGC_Transaction::GetPromotionStrategy() const
	{
		return m_PromotionStrategy;
	}

	void PGC_Transaction::EndTransaction()
	{
		m_Dieing = true;
		Finish();
		PromoteExternals(m_LastTopTransaction);
		PGC_Promotion::PromoteRequests();  // Promote requests away from this transaction.
		ReleaseBlocks();

		m_TopTransaction = m_LastTopTransaction;
	}

	void PGC_Transaction::ReleaseBlocks()
	{
		PGC_Block *theBlock = m_FirstBlock;
		m_FirstBlock = NULL;
		while (theBlock)
		{
			PGC_Block *nextBlock = theBlock->GetNextBlock();
			theBlock->DestroyUncopied(this);
			delete theBlock;
			theBlock = nextBlock;
		}
		*m_LiveTransaction = false;
	}

	std::shared_ptr<bool> PGC_Transaction::GetLiveTransactionPointer()
	{
		return m_LiveTransaction;
	}


	bool PGC_Transaction::Dieing()
	{
		return m_Dieing;
	}

	void *PGC_Transaction::Allocate(size_t p_size)
	{
		ASSERTM(p_size < BlockSize, "Allocation " + to_string(p_size) + " bigger than block size "s + to_string(BlockSize));
		if (m_CurrentBlock == NULL)
		{
			m_FirstBlock = new PGC_Block(NULL);
			m_CurrentBlock = m_FirstBlock;
		}
		void *mem = m_CurrentBlock->Allocate(p_size);
		if (mem == NULL)
		{
			m_CurrentBlock = new PGC_Block(m_CurrentBlock);
			mem = m_CurrentBlock->Allocate(p_size);
		}
		m_AllocatingTransaction = this;
		m_NetMemoryUsed += p_size - PGC_OVERHEAD;
		ASSERTM(mem, "Memory allocation failure.");
		return mem;
	}

	PGC_Base *PGC_Transaction::FindOwner(void *p_member)
	{
		PGC_Base *owner;
		PGC_Block *theBlock = m_FirstBlock;
		while (theBlock)
		{
			owner = theBlock->FindOwner(p_member);
			if (owner)
			{
				return owner;
			}
			theBlock = theBlock->GetNextBlock();
		}
		if (m_LastTopTransaction)
		{
			return m_LastTopTransaction->FindOwner(p_member);
		}
		return NULL;
	}

	void PGC_Transaction::SubmitTask(PGC_Task * p_Task)
	{
		m_Mutex.lock();
		m_TaskList.push_back(p_Task);
		m_Mutex.unlock();
	}

	PGC_Task * PGC_Transaction::TopTask()
	{
		if (m_TaskList.empty())
		{
			return NULL;
		}
		return *m_TaskList.rbegin();
	}

	void PGC_Transaction::RegisterForDestruction(PGC_Base *p_Base)
	{
		m_CurrentBlock->RegisterForDestruction(p_Base);
	}

	void *PGC_Transaction::CreateNew(size_t p_size)
	{
		ASSERTM(m_AllocatingDepth == 0, "Nested memory allocation.");
		++m_AllocatingDepth;
		return Allocate(p_size);
	}

	/*static*/ PGC_Transaction* PGC_Transaction::RegisterLastForDestruction(PGC_Base *p_Base)
	{
		ASSERTM(m_AllocatingDepth == 1, "Nested memory destruction.");
		--m_AllocatingDepth;
		m_AllocatingTransaction->RegisterForDestruction(p_Base);
		return m_AllocatingTransaction;
	}

	/*static*/ PGC_Transaction *PGC_Transaction::TopTransaction()
	{
		if ((m_TopTransaction == 0) && !m_InWebServer)
		{
			new PGC_Transaction(true);
		}
		return m_TopTransaction;
	}

	size_t PGC_Transaction::NetMemoryUsed()
	{
		size_t memory = 0;
		PGC_Block *theBlock = m_FirstBlock;
		while (theBlock)
		{
			memory += theBlock->NetMemoryUsed();
			theBlock = theBlock->GetNextBlock();
		}
		ASSERTM(m_NetMemoryUsed == memory, "Memory calculation error " + to_string(m_NetMemoryUsed) + " != " + to_string(memory));
		return memory;
	}

	size_t PGC_Transaction::GrossMemoryUsed()
	{
		size_t memory = sizeof(PGC_Transaction);
		PGC_Block *theBlock = m_FirstBlock;
		while (theBlock)
		{
			memory += sizeof(*theBlock);
			theBlock = theBlock->GetNextBlock();
		}
		return memory;
	}

	/*static*/ void PGC_Transaction::ResetNetMemoryUsed()
	{
		m_TotalNetMemoryUsed = 0;
	}

	/*static*/ void PGC_Transaction::ResetGrossMemoryUsed()
	{
		m_TotalGrossMemoryUsed = PGC_Promotion::PromotionFreeMemory();
	}

	void PGC_Transaction::CollectGarbage()
	{
	}

	void PGC_Transaction::MultiThreaded(bool p_enable)
	{
		m_MultiThreaded = p_enable;
	}

	bool PGC_Transaction::Running()
	{
		return 	m_ProcessThread != NULL;
	}

	void PGC_Transaction::StartProcessing(PGC_Transaction *p_Transaction)
	{
		p_Transaction->Process();
	}

	void PGC_Transaction::Process()
	{
		while (!m_Dieing)
		{
			CollectGarbage();
			for (TaskList::iterator it = m_TaskList.begin(); it != m_TaskList.end(); it++)
			{
				if (m_Dieing)
				{
					break;
				}
				(*it)->Run();
			}
			this_thread::yield();
		}
	}

	// Perform final processing;
	void PGC_Transaction::Finish()
	{
		if (m_ProcessThread)
		{
			m_ProcessThread->join();
		}
		for (TaskList::iterator it = m_TaskList.begin(); it != m_TaskList.end(); it++)
		{
			PGC_Task *task = (*it);
			if (!task->Finish())
			{
				task->Promote(m_LastTopTransaction);
			}
		}
		m_TaskList.clear();
	}

	/*static*/ size_t PGC_Transaction::TotalNetMemoryUsed()
	{
		return m_TotalNetMemoryUsed;
	}

	/*static*/ size_t PGC_Transaction::TotalGrossMemoryUsed()
	{
		return m_TotalGrossMemoryUsed;
	}

	/*static*/ void PGC_Transaction::AddTotalNetMemorySize(long memory)
	{
		m_TotalNetMemoryUsed += memory;
	}

	/*static*/ void PGC_Transaction::AddTotalGrossMemorySize(long memory)
	{
		m_TotalGrossMemoryUsed += memory;
	}

	//  Usage:
	//     StartStackTransaction and EndStackTransaction should be used in pairs, at the start and
	//     end of a function, or block within a function.  
	//  Purpose:
	//      These functions will automatically create transactions as needed.
	//  Description:
	//      Start a transaction,if it is needed.  The method will start a transaction if there is none,
	//      Or if the net memory usage of the current top transaction passes a threshold.
	/*static*/ void PGC_Transaction::StartStackTransaction()
	{
		++m_TransactionDepth;
		if (m_TopTransaction)
		{
			if (m_TopTransaction->NetMemoryUsed() > m_NewTransactionThreshold)
			{
				new PGC_Transaction();
			}
		}
		else
		{
			new PGC_Transaction();
		}
	}

	//  Usage:
	//      Used to match calls to StartStackTransaction.
	//  Purpose:
	//      End the current transaction when the same depth has been reached as when the transaction was started.
	//  Description:
	//      Check the transaction on the top of the stack to see it should end.
	/*static*/ void PGC_Transaction::EndStackTransaction()
	{
		if (m_TopTransaction)
		{
			if (m_TransactionDepth == m_TopTransaction->m_StartDepth)
			{
				delete m_TopTransaction;
			}
		}
		--m_TransactionDepth;
	}

	void PGC_Transaction::PromoteExternals(PGC_Transaction *p_Direction)
	{
	}

	bool PGC_Transaction::IsStatic()
	{
		return m_IsStatic;
	}
}
