#if !defined(SN_CARTESIAN_H_INCLUDED)
#define SN_CARTESIAN_H_INCLUDED

#pragma once

#include "sn_expression.h"
#include "sn_parameter.h"
#include "sn_valueset.h"

#include <functional>
using namespace std;

namespace SNI
{
	class SNI_Cartesian;
	class SNI_FunctionDef;
}

using namespace SNI;

#pragma once
namespace SN
{
	class SN_Expression;

	class SN_Cartesian
	{
	public:
		SN_Cartesian(const SN_Expression &p_Expression, long p_Index, SNI_FunctionDef *p_FunctionDef);
		SN_Cartesian::SN_Cartesian(const SN_Cartesian &p_Cartesian);
		virtual ~SN_Cartesian();

		SN_Cartesian &operator *(const SN_Cartesian &p_Cartesian);

		SN_Error ForEach(std::function<SN::SN_Error (long p_Depth, SN_Expression *p_ParamList, SNI::SNI_World**p_WorldList, long p_CalcPos, long p_TotalCalc)> p_Action);
		SN_Value ForEachCall(const SNI::SNI_FunctionDef * p_FunctionDef);

		SNI::SNI_Cartesian *GetSNI_Cartesian() const;
	private:
		SN_Error ForEachUnify(const SNI::SNI_FunctionDef * p_FunctionDef);
		SNI::SNI_Cartesian *m_Cartesian;
	};
}

#endif //SN_CARTESIAN_H_INCLUDED
