#include "sni_cartunify.h"

#include "logcontext.h"
#include "sn.h"
#include "sn_value.h"
#include "sn_valueset.h"
#include "sn_parameter.h"
#include "sn_variable.h"
#include "sn_error.h"
#include "sn_manager.h"

#include "sni_functiondef.h"
#include "sni_world.h"
#include "sni_worldset.h"
#include "sni_result.h"
#include "sni_helpers.h"
#include "sni_delayedprocessor.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_CartUnify::SNI_CartUnify(const SNI_FunctionDef *p_FunctionDef, long p_Depth, SN::SN_Expression * p_InputList, SN::SN_Expression * p_ParamList, bool *p_Output, long p_CalcPos, long p_TotalCalc)
		: m_FunctionDef(p_FunctionDef)
		, m_Depth(p_Depth)
		, m_InputList(p_InputList)
		, m_ParamList(p_ParamList)
		, m_Output(p_Output)
	    , m_ValueList(new SN::SN_Expression[p_Depth])
	    , m_WorldList(new SNI_World *[p_Depth])
		, m_CalcPos(p_CalcPos)
		, m_ValueCalcPos(new long[p_Depth+1])
		, m_ValueTotalCalc(new long[p_Depth+1])
	{
		m_ValueTotalCalc[0] = p_Depth;
		m_ValueCalcPos[0] = -1;
		if (p_TotalCalc == 0)
		{
			m_WorldSet = new SNI_WorldSet;
		}
		else
		{
			m_WorldSet = NULL;
		}
	}

	SNI_CartUnify::~SNI_CartUnify()
	{
		delete [] m_ParamList;
		delete [] m_ValueList;
		delete [] m_WorldList;
		delete [] m_ValueCalcPos;
		delete [] m_ValueTotalCalc;
	};

	SN::SN_Error SNI_CartUnify::ProcessValue(long p_Depth, const SN::SN_Expression &p_Param, SNI_World*p_World)
	{
		SN::SN_Error e(true);
		m_ValueList[p_Depth] = p_Param;
		m_WorldList[p_Depth] = p_World;
		m_ValueTotalCalc[p_Depth + 1] = m_ValueTotalCalc[p_Depth];
		m_ValueCalcPos[p_Depth + 1] = m_ValueCalcPos[p_Depth];
		if (!m_Output[p_Depth] && m_ValueList[p_Depth].IsKnownValue())
		{
			m_ValueTotalCalc[p_Depth + 1]--;
		}
		else
		{
			m_ValueCalcPos[p_Depth + 1] = p_Depth;
		}
		if (p_Depth < m_Depth-1)
		{
			if (m_Output[p_Depth + 1])
			{
				e = ProcessValue(p_Depth + 1, m_InputList[p_Depth + 1], NULL);
			}
			else
			{
				e = m_InputList[p_Depth+1].GetSNI_Expression()->ForEachCart(p_Depth+1, this);
			}
		}
		else
		{
			size_t maxCard = SN::SN_Manager::GetTopManager().MaxCardinalityUnify();
			size_t card = m_FunctionDef->CardinalityOfUnify(p_Depth + 1, m_ValueList, m_ValueCalcPos[p_Depth + 1], m_ValueTotalCalc[p_Depth + 1]);
			if (maxCard < card)
			{
				e = DelayUnify();
			}
			else
			{
				e = m_FunctionDef->UnifyElement(p_Depth + 1, m_ValueList, m_WorldList, m_ValueCalcPos[p_Depth + 1], m_ValueTotalCalc[p_Depth + 1], m_WorldSet);
			}
		}
		return e;
	}

	SN::SN_Error SNI_CartUnify::ForEachUnify()
	{
		SN::SN_Error e;
		CreateVariablesForOutput();
		if (m_Output[0])
		{
			e = ProcessValue(0, m_InputList[0], NULL);
		}
		else
		{
			e = m_InputList[0].GetSNI_Expression()->ForEachCart(0, this);
		}
		if (e.GetBool())
		{
			if (m_WorldSet)
			{
				m_WorldSet->CheckDependentWorlds();
				if (m_WorldSet->IsEmpty())
				{
					return SN::SN_Error("SNI_Cart: World set is empty after matching values in cartesian product.");
				}
			}
			e = CaptureOutput();
		}
		return e;
	}

	void SNI_CartUnify::CreateVariablesForOutput()
	{
		for (long j = 0; j < m_Depth; j++)
		{
			if (m_Output[j])
			{
				m_InputList[j] = new SNI_ValueSet;
			}
		}
	}

	SN::SN_Error SNI_CartUnify::CaptureOutput()
	{
		for (size_t j = 0; j < m_Depth; j++)
		{
			if (m_Output[j])
			{
				SN::SN_Value simple = m_InputList[j].SimplifyValue();
				SN::SN_Error  e = m_ParamList[j].AssertValue(simple);
				if (e.IsError())
				{
					return e;
				}
			}
			else
			{
				m_InputList[j].Simplify();
			}
		}
		return true;
	}

	SN::SN_Error SNI_CartUnify::DelayUnify()
	{
		if (!m_FunctionDef->AllowDelay())
		{
			return SN::SN_Error(true, true);
		}
		bool exists = false;
		SNI_WorldSet *worldSet = m_WorldSet;
		if (0 <= m_CalcPos)
		{
			worldSet = m_ValueList[m_CalcPos].GetWorldSet();
		}
		if (!worldSet)
		{
			FORCE_ASSERTM("No world set.");
		}
		SNI_World *world = worldSet->JoinWorldsArray(AutoAddWorld, AlwaysCreateWorld, exists, m_Depth, m_WorldList);
		if (exists)
		{
			SN::SN_ParameterList *l_ParameterList = new SN::SN_ParameterList();
			for (long j = 0; j < m_Depth; j++)
			{
				if (j == m_CalcPos)
				{
					SN::SN_ValueSet vs = m_ValueList[m_CalcPos].GetVariableValue(false);
					if (vs.IsNull())
					{
						FORCE_ASSERTM("Unexpected null value.");
					}
					SN::SN_Variable var;
					var.SetName(vs.GenerateTempVariableName() + "_" + to_string(j));
					if (!vs.IsNull())
					{
						vs.AddTaggedValue(var, world);
					}
					l_ParameterList->push_back(var);
				}
				else
				{
					l_ParameterList->push_back(m_ValueList[j]);
				}
			}
			SNI_DelayedProcessor::GetProcessor()->Delay(m_FunctionDef, l_ParameterList, world);
		}
		return true;
	}
}