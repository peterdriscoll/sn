#if !defined(SNI_CART_H_INCLUDED)
#define SNI_CART_H_INCLUDED

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

	class SNI_Cart
	{
	public:
		SNI_Cart(const SNI_FunctionDef *p_FunctionDef, long p_Depth, SN::SN_Expression * p_InputList, SN::SN_Expression * p_ParamList, bool *p_Output, long p_CalcPos, long p_TotalCalc);
		virtual ~SNI_Cart();

		SN::SN_Error ForEachUnify();

		SN::SN_Error ProcessValueUnify(long p_Depth, const SN::SN_Expression & p_Param, SNI_World * p_World);


	protected:
		void CreateVariablesForOutput();
		SN::SN_Error CaptureOutput();
		SN::SN_Error DelayUnify();

	private:
		const SNI_FunctionDef *m_FunctionDef;
		long m_Depth;
		SN::SN_Expression * m_InputList;
		SN::SN_Expression * m_ParamList;
		bool *m_Output;
		SN::SN_Expression * m_ValueList;
		SNI_World ** m_WorldList;
		long m_CalcPos;
		long *m_ValueCalcPos;
		long *m_ValueTotalCalc;
		SNI_WorldSet *m_WorldSet;
	};
}

#endif //SNI_CART_H_INCLUDED
