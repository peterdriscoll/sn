#if !defined(SN_TRANSACTION_H_INCLUDED)
#define SN_TRANSACTION_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "exp_ctrl_sn.h"
#include "pgc_transaction.h"

namespace SN
{
	class SN_EXPORT SN_Transaction : public PGC::PGC_Transaction
	{
	public:
		SN_Transaction();
		virtual ~SN_Transaction();
	};
}

#endif // !defined(SN_TRANSACTION_H_INCLUDED)
