#if !defined(SN_TRANSACTION_H_INCLUDED)
#define SN_TRANSACTION_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "exp_ctrl_sn.h"
#include "pgc_transaction.h"

namespace SNI
{
	class SNI_Transaction;
}

namespace SN
{
	class SN_EXPORT SN_Transaction
	{
	public:
		size_t NetMemoryUsed();
		size_t GrossMemoryUsed();
		static void RegisterInWebServer();
		static bool InWebServer();

		SN_Transaction(bool p_Init);

		SN_Transaction();
		virtual ~SN_Transaction();

		void Init();

	private:
		SNI::SNI_Transaction *m_Transaction;
	};
}

#endif // !defined(SN_TRANSACTION_H_INCLUDED)
