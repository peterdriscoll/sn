#if !defined(SNI_CARTESIAN_H_INCLUDED)
#define SNI_CARTESIAN_H_INCLUDED

#pragma once

#include "sn_expression.h"
#include "sn_parameter.h"
#include "sn_valueset.h"

#include <functional>
using namespace std;

namespace SNI
{
	class SNI_World;
	class SNI_WorldSet;
	class SNI_FunctionDef;
	class SNI_Binary;
	class SNI_Unary;

	class SNI_Cartesian
	{
	public:
		SNI_Cartesian(const SN::SN_Expression &p_Expression, long p_Index, SNI_FunctionDef *p_FunctionDef);
		virtual ~SNI_Cartesian();

		SNI_Cartesian &operator *(const SN::SN_Cartesian &p_SN_Cartesian);

		SN::SN_Error ForEach(std::function<SN::SN_Error (long p_Depth, SN::SN_Expression *p_ParamList, SNI_World**p_WorldList, long p_CalcPos, long p_TotalCalc)> p_Action);

	private:
		SN::SN_Error Loop(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, std::function<SN::SN_Error(long p_Depth, SN::SN_Expression *p_ParamList, SNI_World **p_WorldList, long p_CalcPos, long p_TotalCalc)> p_Action);

		long Depth();

		SN::SN_Expression m_Evaluate;
		SNI_Cartesian *m_Cartesian;
	};
}

#endif //SNI_CARTESIAN_H_INCLUDED
