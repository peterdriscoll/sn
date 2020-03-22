#if !defined(SNL_CHARACTERSET_H_INCLUDED)
#define SNL_CHARACTERSET_H_INCLUDED

#pragma once

#include "exp_ctrl_sn_lib.h"
#include "sn.h"

namespace SNL
{
	class SN_LIB_EXPORT SNL_CharacterSet: public skynet::Domain
	{
	public:
		SNL_CharacterSet();

		skynet::Variable Sign;
		skynet::Variable TimesDivide;
		skynet::Variable OperatorChar;
		skynet::Variable Digit;
		skynet::Variable AlphaLower;
		skynet::Variable AlphaUpper;
		skynet::Variable Alpha;
		skynet::Variable AlphaUnder;
		skynet::Variable AlphaNumeric;
		skynet::Variable AlphaUnderNumeric;
		skynet::Variable White;

	private:
		void Setup();
		void Link();
		void Init();
	};
}

#endif // !defined(SNL_CHARACTERSET_H_INCLUDED)
