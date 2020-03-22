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

		skynet::Variable AsExpression;

		skynet::Variable AsLambda;
		skynet::Variable AsLet;
		skynet::Variable AsLocal;
		skynet::Variable AsIf;

		skynet::Variable AsBooleanExpression;
		skynet::Variable AsBooleanTerm;
		skynet::Variable AsBooleanFactor;
		skynet::Variable AsBooleanValue;

		skynet::Variable AsComparisonExpression;

		skynet::Variable AsArithmeticExpression;
		skynet::Variable AsArithmeticTerm;
		skynet::Variable AsArithmeticFactor;

		skynet::Variable AsFunctionCall;
		skynet::Variable AsSubscript;
		skynet::Variable AsPath;

		skynet::Variable AsValueRef;
		skynet::Variable AsFactor;

		skynet::Variable AsName;
		skynet::Variable AsValue;
		skynet::Variable AsBracket;

		skynet::Variable AsInteger;
		skynet::Variable AsScientific;
		skynet::Variable AsNumber;

		skynet::Variable AsString;

		skynet::Variable AsBoolean;
	private:
		void Setup();
		void Link();
		void Init();

		void DefineExpression();
		void DefineBooleanExpression();
		void DefineComparisonExpression();
		void DefineArithmeticExpression();

		void DefineFunctionCall();
		void DefineSubscript();
		void DefinePath();

		void DefineFactor();

		void DefineValue();

		const SNL_CharacterSet &m_CharacterSet;
		const SNL_Validate &m_Validate;
	};
}

#endif // !defined(SNP_PARSER_H_INCLUDED)
