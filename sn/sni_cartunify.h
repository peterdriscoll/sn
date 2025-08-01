#if !defined(SNI_CARTUNIFY_H_INCLUDED)
#define SNI_CARTUNIFY_H_INCLUDED

#pragma once

#include "sn_expression.h"
#include "sn_parameter.h"
#include "sn_valueset.h"

#include "sni_cart.h"

namespace SNI
{
	class SNI_World;
	class SNI_WorldSet;
	class SNI_FunctionDef;
	class SNI_Binary;
	class SNI_Unary;

	class SNI_CartUnify : public SNI_Cart
	{
	public:
		SNI_CartUnify(const SNI_FunctionDef *p_FunctionDef, long p_Depth, SN::SN_Expression * p_InputList, SN::SN_Expression * p_ParamList, bool *p_Output, long p_CalcPos, long p_TotalCalcc, const SNI_Expression *p_Source);
		virtual ~SNI_CartUnify();

		SN::SN_Error ForEachUnify();

		virtual SN::SN_Error ProcessValue(long p_Depth, const SN::SN_Expression & p_Param, SNI_World * p_World);

	protected:
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
		const SNI_Expression *m_Source;
	};
}

#endif //SNI_CARTUNIFY_H_INCLUDED
