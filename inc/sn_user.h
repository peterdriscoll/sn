#pragma once

#include "exp_ctrl_sn.h"
#include "pgc_transaction.h"

namespace SNI
{
	class SNI_Transaction;
	class SNI_User;
}

namespace SN
{
	class SN_EXPORT SN_User
	{
	public:
		SN_User();
		virtual ~SN_User();

		void Init();

		static SN::SN_OperatorVariables& GetOperators();

	private:
		SNI::SNI_Transaction* m_Transaction;
		SNI::SNI_User* m_User;
	};
}
