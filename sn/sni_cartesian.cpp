#include "sni_cartesian.h"

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
	SNI_Cartesian::SNI_Cartesian(const SN::SN_Expression &p_Expression, long p_Index, SNI_FunctionDef *p_FunctionDef)
		: m_Expression(p_Expression)
		, m_Cartesian(NULL)
		, m_InputList(NULL)
		, m_ValueList(NULL)
		, m_WorldList(NULL)
		, m_FunctionDef(p_FunctionDef)
		, m_Null(true)
		, m_IsReferableValue(false)
		, m_Base(false)
		, m_Index(p_Index)
	{};

	SNI_Cartesian::~SNI_Cartesian()
	{
		Cleanup();
		delete [] m_Cartesian;
	};

	SNI_Cartesian &SNI_Cartesian::operator *(const SN::SN_Cartesian &p_Cartesian)
	{
		if (m_Cartesian)
			(*m_Cartesian) * p_Cartesian;
		else
			m_Cartesian = p_Cartesian.GetSNI_Cartesian();
		return *this;
	};

	void SNI_Cartesian::Cleanup()
	{
		if (m_Base)
		{
			delete[] m_InputList;
			delete[] m_ValueList;
			delete[] m_WorldList;
		}
		m_Base = true;
	}

	long SNI_Cartesian::Depth()
	{
		return m_Cartesian ? 1 + m_Cartesian->Depth() : 1;
	};

	long SNI_Cartesian::Index()
	{
		return m_Index;
	}

	SN::SN_Error SNI_Cartesian::ForEach(std::function<SN::SN_Error(long p_Depth, SN::SN_Expression *p_ParamList, SNI_World **p_WorldList, long p_CalcPos, long p_TotalCalc)> p_Action)
	{
		long depth = Depth();

		return Loop(0, new SN::SN_Expression[depth], new SNI_World*[depth], -1, 0, p_Action);
	};

	SN::SN_Value SNI_Cartesian::ForEachCall(const SNI_FunctionDef *p_FunctionDef)
	{
		long depth = Depth();
		SN::SN_ValueSet result;
		Cleanup();
		SN::SN_Error e1 = LoopSetupCall(0, p_FunctionDef, new SN::SN_Expression[depth], new SN::SN_Expression[depth], new SNI_World*[depth], result);
		if (e1.GetDelay())
		{
			if (SN::SN_Manager::GetTopManager().DelayOnEvaluate())
			{
				SN::SN_ParameterList *l_ParameterList = new SN::SN_ParameterList(Depth());
				SN::SN_Variable l_Result;
				LoadParameterList(*l_ParameterList, l_Result);
				SNI_DelayedProcessor::GetProcessor()->Delay(p_FunctionDef, l_ParameterList, l_Result);
				return l_Result;
			}
			return skynet::Null;
		}
		else if (e1.IsError())
		{
			return e1;
		}
		SN::SN_Error e2 = LoopCall(0);
		if (e2.IsError())
		{
			return e2;
		}
		return result.SimplifyValue();
	}

	SN::SN_Error SNI_Cartesian::LoopSetupCall(long p_Depth, const SNI_FunctionDef *p_FunctionDef, SN::SN_Expression * p_InputList, SN::SN_Expression * p_ValueList, SNI_World ** p_WorldList, SN::SN_ValueSet p_Result)
	{
		SN::LogContext context("SN::SN_Error SNI_Cartesian::LoopSetupCall");
		SN::SN_Error response(true);
		m_FunctionDef = p_FunctionDef;
		m_InputList = p_InputList;
		m_ValueList = p_ValueList;
		m_WorldList = p_WorldList;
		if (!m_Expression.IsNullValue())
		{
			m_Evaluate = m_Expression.Evaluate();
		}
		else
		{
			m_Evaluate = m_Expression;
		}
		if (m_Evaluate.IsError())
		{
			SN::SN_Error e = m_Evaluate;
			e.AddNote(context, NULL, "Error construct SNI_Cartesian product");
			return e;
		}
		m_InputList[m_Index] = m_Evaluate;

		context.LogExpression(p_FunctionDef->DisplayCpp(), " Depth = " + to_string(p_Depth) + " Index " + to_string(m_Index) + " Null = " + to_string(m_Null) + ")");
		context.LogExpression("Expression", m_Expression);
		context.LogExpression("Evaluate", m_Evaluate);
		context.LogText("Cart Setup", p_FunctionDef->DisplayCpp() + "::CardinalityOfCall(" + DisplayValues(p_Depth + 1, p_InputList) + " Index " + to_string(m_Index) + " )");
		if (SN::SN_Manager::GetTopManager().MaxCardinalityCall() < p_FunctionDef->CardinalityOfCall(p_Depth + 1, p_InputList))
		{
			if (m_Cartesian)
			{
				response = m_Cartesian->LoopSetupCall(p_Depth + 1, p_FunctionDef, p_InputList, p_ValueList, p_WorldList, p_Result);
			}
			else
			{
				context.LogText(p_FunctionDef->DisplaySN0(), "Delay");
				return SN::SN_Error(true, true);
			}
		}
		context.LogText(p_FunctionDef->DisplaySN0(), "Process now");
		m_Result = p_Result;
		return response;
	};

	SN::SN_Error SNI_Cartesian::ProcessValueCall(const SN::SN_Expression &p_Param, SNI_World*p_World, long p_Depth)
	{
		m_ValueList[m_Index] = p_Param;
		m_WorldList[m_Index] = p_World;

		SN::LogContext context("SNI_Cartesian::ProcessValue(" + m_FunctionDef->DisplayCpp() + ", Depth+1 " + to_string(p_Depth + 1) + " Index " + to_string(m_Index) + " )");
		context.LogExpression("Param", p_Param, p_World);
		if (SN::SN_Manager::GetTopManager().MaxCardinalityCall() < m_FunctionDef->CardinalityOfCall(p_Depth + 1, m_ValueList))
		{
			if (m_Cartesian)
			{
				SN::SN_Error e = m_Cartesian->LoopCall(p_Depth + 1);
				if (e.IsError())
				{
					e.AddNote(context, NULL, "Error constructing SNI_Cartesian product");
					return e;
				}
			}
			else
			{
				context.LogText(m_FunctionDef->DisplayCpp(), "Delay");
				bool exists = false;
				SNI_WorldSet *worldSet = m_ValueList[m_CalcPos].GetWorldSet();
				context.LogText("Join Worlds", m_FunctionDef->DisplayCpp() + " " + DisplayWorlds(p_Depth + 1, m_WorldList));
				SNI_World *world = worldSet->JoinWorldsArray(AutoAddWorld, AlwaysCreateWorld, exists, p_Depth + 1, m_WorldList);
				if (exists)
				{
					context.LogText("exists", world->DisplaySN());
					SN::SN_ParameterList *l_ParameterList = new SN::SN_ParameterList();
					for (long j = 0; j < p_Depth; j++)
					{
						l_ParameterList->push_back(m_ValueList[j]);
					}
					SNI_DelayedProcessor::GetProcessor()->Delay(m_FunctionDef, l_ParameterList, m_Result, world);
				}
			}
		}
		else
		{
			return m_FunctionDef->CallElement(p_Depth + 1, m_ValueList, m_WorldList, m_Result);
		}

		return SN::SN_Error(true);
	}

	SN::SN_Error SNI_Cartesian::LoopCall(long p_Depth)
	{
		m_Evaluate.ForEachCall(this, p_Depth);
		return SN::SN_Error(true);
	}

	SN::SN_Error SNI_Cartesian::Loop(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, std::function<SN::SN_Error(long p_Depth, SN::SN_Expression *p_ParamList, SNI_World **p_WorldList, long p_CalcPos, long p_TotalCalc)> p_Action)
	{
		SNI_Cartesian *SNI_Cartesian = m_Cartesian;
		m_Evaluate.ForEach(
			[&p_ParamList, &p_WorldList, p_Depth, p_CalcPos, p_TotalCalc, SNI_Cartesian, p_Action](const SN::SN_Expression &p_Expression, SNI_World *p_World) -> SN::SN_Error
			{
				p_ParamList[p_Depth] = p_Expression;
				p_WorldList[p_Depth] = p_World;
				if (SNI_Cartesian)
				{
					SN::SN_Error e = SNI_Cartesian->Loop(p_Depth + 1, p_ParamList, p_WorldList, p_CalcPos, p_TotalCalc, p_Action);
					if (e.IsError())
					{
						return e;
					}
				}
				else
				{
					SN::SN_Error e = p_Action(p_Depth, p_ParamList, p_WorldList, p_CalcPos, p_TotalCalc);
					if (e.IsError())
					{
						return e;
					}
				}
				return SN::SN_Error(true);
			}
		);
		return SN::SN_Error(true);
	}

	SN::SN_Error SNI_Cartesian::LoadParameterList(SN::SN_ParameterList &p_ParameterList, SN::SN_Expression &p_Result)
	{
		if (m_Index == PU2_Result)
		{
			if (m_Null)
			{
				p_Result = m_Expression;
			}
			else
			{
				p_Result = m_Evaluate;
			}
		}
		else
		{
			if (m_Null)
			{
				p_ParameterList[m_Index - 1] = m_Expression;
			}
			else
			{
				p_ParameterList[m_Index - 1] = m_Evaluate;
			}
		}
		if (m_Cartesian)
		{
			return m_Cartesian->LoadParameterList(p_ParameterList, p_Result);
		}
		return SN::SN_Error();
	}
}
