#if !defined(SNI_CARTCALL_H_INCLUDED)
#define SNI_CARTCALL_H_INCLUDED

#pragma once

#include "sn_expression.h"
#include "sn_parameter.h"
#include "sn_valueset.h"

using namespace std;

namespace SNI
{
	class SNI_World;
	class SNI_WorldSet;
	class SNI_FunctionDef;
	class SNI_Binary;
	class SNI_Unary;

	class SNI_CartCall
	{
	public:
		SNI_CartCall(const SNI_FunctionDef *p_FunctionDef, long p_Depth, SN::SN_Expression * p_InputList);
		virtual ~SNI_CartCall();

		SN::SN_Error ForEach();

		SN::SN_Error ProcessValue(long p_Depth, const SN::SN_Expression & p_Param, SNI_World * p_World);

	private:
		const SNI_FunctionDef *m_FunctionDef;
		long m_Depth;
		SN::SN_Expression * m_InputList;
		SN::SN_Expression * m_ValueList;
		SNI_World ** m_WorldList;
		SN::SN_ValueSet m_Result;
	};
}

#endif //SNI_CARTCALL_H_INCLUDED
