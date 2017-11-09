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
		SN::SN_Value ForEachCall(const SNI_FunctionDef * p_FunctionDef);

		// Call
		SN::SN_Error ProcessValueCall(const SN::SN_Expression & p_Param, SNI_World * p_World, long p_Depth);
		// Unify

	private:
		// Call
		SN::SN_Error LoopSetupCall(long p_Depth, const SNI_FunctionDef * p_FunctionDef, SN::SN_Expression * p_InputList, SN::SN_Expression * p_ValueList, SNI_World ** p_WorldList, SN::SN_ValueSet p_Result);
		SN::SN_Error LoopCall(long p_Depth);

		// Generic
		SN::SN_Error Loop(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, std::function<SN::SN_Error(long p_Depth, SN::SN_Expression *p_ParamList, SNI_World **p_WorldList, long p_CalcPos, long p_TotalCalc)> p_Action);

		long Depth();
		long Index();
		SN::SN_Error LoadParameterList(SN::SN_ParameterList &p_ParameterList, SN::SN_Expression &p_Result);

		SN::SN_Expression m_Expression;
		SN::SN_Expression m_Evaluate;
		SNI_Cartesian *m_Cartesian;

		void Cleanup();

		const SNI_FunctionDef *m_FunctionDef;
		SN::SN_Expression *m_InputList;
		SN::SN_Expression *m_ValueList;
		SNI_World **m_WorldList;
		SN::SN_ValueSet m_Result;
		bool m_Base;
		long m_CalcPos;
		long m_TotalCalc;
		long m_ValueCalcPos;
		long m_ValueTotalCalc;
		long m_ParamCalcPos;
		long m_ParamTotalCalc;
		SNI_WorldSet *m_WorldSet;
		bool m_Null;
		bool m_IsReferableValue;
		long m_Index;
	};
}

#endif //SNI_CARTESIAN_H_INCLUDED
