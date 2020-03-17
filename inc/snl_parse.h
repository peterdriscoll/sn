#if !defined(SNL_PARSE_H_INCLUDED)
#define SNL_PARSE_H_INCLUDED

#pragma once

#include "exp_ctrl_sn_lib.h"
#include "sn.h"
#include "snl_validate.h"

namespace SNL
{
	class SN_LIB_EXPORT SNL_Parse: public skynet::Domain
	{
	public:
		SNL_Parse(const SNL_CharacterSet p_CharacterSet, const SNL_Validate &p_Validate);

		skynet::Variable AsInteger;
		skynet::Variable AsScientific;
		skynet::Variable AsNumber;

		skynet::Variable AsString;
		skynet::Variable AsValue;

		skynet::Variable AsName;
		skynet::Variable AsPath;

		skynet::Variable AsCall;
		skynet::Variable AsLambda;

		skynet::Variable AsLambdaPart;
		skynet::Variable AsLambdaTerm;

		skynet::Variable AsFactor;
		skynet::Variable AsTerm;
		skynet::Variable AsExpression;

	private:
		void Setup();
		void Link();
		void Init();

		void DefineValue();
		void DefineReference();
		void DefineLambdaCalculus();
		void DefineExpression();

		const SNL_CharacterSet &m_CharacterSet;
		const SNL_Validate &m_Validate;
	};
}

#endif // !defined(SNP_PARSER_H_INCLUDED)
