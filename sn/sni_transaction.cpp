#include "sni_transaction.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Transaction::SNI_Transaction()
		: PGC_Transaction(*SNI_User::GetCurrentPGC_User(),  false, SNI_Thread::TopManager()->MemoryPromotionStrategy())
	{
		SNI_Thread::GetThread()->PushFrame(new SNI_Frame());
	}

	SNI_Transaction::~SNI_Transaction()
	{
	}

	void SNI_Transaction::EndTransaction()
	{
		SNI_Thread *thread = SNI_Thread::GetThread();
		if (thread)
		{
			thread->Clear();
		}
		PGC_Transaction::EndTransaction();
	}

	void SNI_Transaction::PromoteExternals(PGC::PGC_Transaction *p_Destination)
	{
		if (p_Destination)
		{
			SNI_Thread::GetThread()->PromoteExternals(p_Destination);
		}
	}
}
