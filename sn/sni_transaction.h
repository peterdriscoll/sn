#if !defined(SNI_TRANSACTION_H_INCLUDED)
#define SNI_TRANSACTION_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "exp_ctrl_sn.h"
#include "pgc_transaction.h"

namespace SNI
{
	class SN_EXPORT SNI_Transaction : public PGC::PGC_Transaction
	{
	public:
		SNI_Transaction();
		virtual ~SNI_Transaction();

		virtual void EndTransaction();
		virtual void PromoteExternals(PGC_Transaction *p_Direction);
	};
}

#endif // !defined(SNI_TRANSACTION_H_INCLUDED)
