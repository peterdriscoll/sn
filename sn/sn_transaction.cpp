#include "sn_transaction.h"

#include "sn_pch.h"

namespace SN
{
	SN_Transaction::SN_Transaction()
	{
		SNI_Thread::GetThread()->PushFrame(new SNI_Frame());
	}

	SN_Transaction::~SN_Transaction()
	{
		SNI_Thread::GetThread()->PopFrame();
	}
}
