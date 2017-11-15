#if !defined(SN_FUNCTIONDEF_H_INCLUDED)
#define SN_FUNCTIONDEF_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"

#include <string>
#include <vector>
using namespace std;

namespace SNI
{
	class SNI_FunctionDef;
	class SNI_Binary;
	class SNI_Unary;
}

namespace SN
{
	class SN_Parameter;
	typedef vector<SN_Parameter> SN_ParameterList;

	class SN_Expression;
	class SN_Value;
	class SN_Error;

	class SN_EXPORT SN_FunctionDef
	{
	public:
		SN_FunctionDef();
		SN_FunctionDef(const SN_FunctionDef &p_Function);
		SN_FunctionDef(const SNI::SNI_FunctionDef *p_Function);
		virtual ~SN_FunctionDef();

		// Comparison
		SN_Expression operator ==(const SN_Expression &p_Other) const;

		// Function
		SN_Expression operator()(const SN_Expression &p_Parameter) const;

		// Valuesets of functions
		SN::SN_Expression operator ||(const SN::SN_Expression &p_Other) const;

		SNI::SNI_Binary * GetSNI_Binary() const;
		SNI::SNI_Unary * GetSNI_Unary() const;

		SNI::SNI_FunctionDef * GetSNI_FunctionDef() const;

	private:
		SNI::SNI_FunctionDef * m_FunctionDef;
	};
}

#endif // !defined(SN_FUNCTIONDEF_H_INCLUDED)
