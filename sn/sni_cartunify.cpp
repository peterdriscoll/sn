#include "sni_cartunify.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

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
#include "sni_expression.h"
#include "utility.h"

#include "sn_pch.h"

#pragma warning (disable: 4018)

namespace SNI
{
	SNI_CartUnify::SNI_CartUnify(const SNI_FunctionDef *p_FunctionDef, long p_Depth, SN::SN_Expression * p_InputList, SN::SN_Expression * p_ParamList, bool *p_Output, long p_CalcPos, long p_TotalCalc, const SNI_Expression *p_Source)
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
		, m_Source(p_Source)
	{
		m_ValueTotalCalc[0] = p_Depth;
		m_ValueCalcPos[0] = -1;
		if (p_TotalCalc == 0)
		{
			m_WorldSet = new SNI_WorldSet();
		}
		else
		{
			m_WorldSet = NULL;
		}
	}

	SNI_CartUnify::~SNI_CartUnify()
	{
		if (m_WorldSet)
		{
			m_WorldSet->Complete();
		}
		delete [] m_ValueList;
		delete [] m_WorldList;
		delete [] m_ValueCalcPos;
		delete [] m_ValueTotalCalc;
	};

	SN::SN_Error SNI_CartUnify::ProcessValue(long p_Depth, const SN::SN_Expression &p_Param, SNI_World*p_World)
	{
		SN::SN_Error e = skynet::OK;
		m_ValueList[p_Depth] = p_Param;
		m_WorldList[p_Depth] = p_World;
		m_ValueTotalCalc[p_Depth + 1] = m_ValueTotalCalc[p_Depth];
		m_ValueCalcPos[p_Depth + 1] = m_ValueCalcPos[p_Depth];
		if (!m_Output[p_Depth] && m_FunctionDef->IsKnownValue(m_ValueList[p_Depth], p_Depth))
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
			long calcPos = m_CalcPos;
			if (calcPos < 0)
			{
				calcPos = m_ValueCalcPos[p_Depth + 1];
			}
			size_t maxCard = SNI_Thread::TopManager()->MaxCardinalityUnify();
			size_t card = m_FunctionDef->CardinalityOfUnify(p_Depth + 1, m_ValueList, calcPos, m_ValueTotalCalc[p_Depth + 1]);

			if (maxCard < card)
			{
				e = DelayUnify();
			}
			else
			{
				e = m_FunctionDef->UnifyElement(p_Depth + 1, m_ValueList, m_WorldList, calcPos, m_ValueTotalCalc[p_Depth + 1], m_WorldSet, m_Source);
			}
		}
		return e;
	}

	SN::SN_Error SNI_CartUnify::ForEachUnify()
	{
		SN::SN_Error e = skynet::OK;
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
				e = m_WorldSet->CheckDependentWorlds();
				if (e.IsError())
				{
					return e;
				}
				if (m_WorldSet->IsEmpty())
				{
					return SN::SN_Error(false, false, "SNI_Cart: World set is empty after matching values in cartesian product.");
				}
			}
		}
		return e;
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
			SN::SN_Expression *l_ParamList = new SN::SN_Expression[m_Depth];
			for (long j = 0; j < m_Depth; j++)
			{
				if (j == m_CalcPos)
				{
					SN::SN_Expression value = m_ValueList[m_CalcPos].GetVariableValue(true);
					if (value.IsValueSet())
					{
						SNI_Frame* topFrame = SNI_Frame::Top();
						SN::SN_ValueSet vs = value;
						SN::SN_Variable var(topFrame->CreateTemporary());
						var.SetName(vs.GenerateTempVariableName() + "_" + to_string(j));
						if (!vs.IsNull())
						{
							vs.AddTaggedValue(var, world);
						}
						l_ParamList[j] = var;
					}
					else
					{
						l_ParamList[j] = value;
					}
				}
				else
				{
					l_ParamList[j] = m_ValueList[j];
				}
			}
			SNI_User::GetCurrentUser()->GetDelayedProcessor()->Run(m_FunctionDef, m_Depth, l_ParamList, m_Source, world);
		}
		return skynet::OK;
	}
}