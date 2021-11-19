#if !defined(SN_OPERATORVARIABLES_H_INCLUDED)
#define SN_OPERATORVARIABLES_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_domain.h"
#include "sn_variable.h"

namespace SN
{
	class SN_EXPORT SN_OperatorVariables: public SN_Domain
	{
	public:
		SN_OperatorVariables();

		SN_Variable FunctionCall;
		SN_Variable ImperativeCall;

		void Setup();
		void Cleanup();
	private:
		void Link();
	};
}

#endif // !defined(SN_OPERATORVARIABLES_H_INCLUDED)
