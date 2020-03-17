#if !defined(SNL_VALIDATE_H_INCLUDED)
#define SNL_VALIDATE_H_INCLUDED

#pragma once

#include "exp_ctrl_sn_lib.h"
#include "sn.h"
#include "snl_characterset.h"

namespace SNL
{
	class SN_LIB_EXPORT SNL_Validate: public skynet::Domain
	{
	public:
		SNL_Validate(const SNL_CharacterSet &p_CharacterSet);

		skynet::Variable IsInteger;
		skynet::Variable IsUnsignedInteger;
		skynet::Variable IsFloatingPoint;
		skynet::Variable IsFloatingPointExtension;
		skynet::Variable IsExponential;
		skynet::Variable IsExponentialExtension;

		skynet::Variable IsString;
		skynet::Variable IsWhiteSpaceOnly;
		skynet::Variable IsWhiteSpaceOnlyContinuation;
		skynet::Variable IsSimpleComment;
		skynet::Variable IsLineComment;
		skynet::Variable IsComment;
		skynet::Variable IsWhiteSpace;
		skynet::Variable IsWhiteSpaceContinuation;

		skynet::Variable IsName;
		skynet::Variable IsPath;

		skynet::Variable IsCall;
		skynet::Variable IsLambda;

	private:
		void Setup();
		void Link();
		void Init();

		void DefineWhiteSpace();
		void DefineNumber();
		void DefineString();
		void DefineNamePath();

		const SNL_CharacterSet &m_CharacterSet;	
	};
}

#endif // !defined(SNL_VALIDATE_H_INCLUDED)
