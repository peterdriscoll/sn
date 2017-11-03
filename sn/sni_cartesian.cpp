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

	SN::SN_Error SNI_Cartesian::ForEachUnify(const SNI_FunctionDef *p_FunctionDef)
	{
		long depth = Depth();
		long paramDepth =  p_FunctionDef->GetNumParameters()+1;
		long l_CalcPos = -1;
		long l_TotalCalc = paramDepth;
		if (!m_Base)
		{
			m_InputList = new SN::SN_Expression[depth];
			m_ValueList = new SN::SN_Expression[depth];
			m_WorldList = new SNI_World*[depth];
			m_Base = true;
		}
		SN::SN_Error e1 = LoopSetupPartialUnify(0, p_FunctionDef, m_InputList, m_ValueList, m_WorldList, l_CalcPos, l_TotalCalc);
		if (e1.GetDelay())
		{
			SN::SN_Error e2 = LoopSetupUnify(0, p_FunctionDef, m_InputList, m_ValueList, m_WorldList, l_CalcPos, l_TotalCalc);
			if (e2.GetDelay())
			{
				if (depth == paramDepth)
				{
					SN::SN_ParameterList *l_ParameterList = new SN::SN_ParameterList(depth-1);
					SN::SN_Expression l_Result;
					LoadParameterList(*l_ParameterList, l_Result);
					SNI_DelayedProcessor::GetProcessor()->Delay(p_FunctionDef, l_ParameterList, l_Result);
				}
				return true;
			}
			else if (e2.IsError())
			{
				return e2;
			}
		}
		else if (e1.IsError())
		{
			return e1;
		}
		SNI_WorldSet * worldSet = NULL;
		if (l_TotalCalc == 0)
		{
			worldSet = new SNI_WorldSet;
		}
		SN::SN_Error e3 = LoopUnify(0, -1, paramDepth, worldSet);
		if (e3.GetDelay())
		{
			SN::SN_ParameterList *l_ParameterList = new SN::SN_ParameterList;
			SN::SN_Value l_Result;
			LoadParameterList(*l_ParameterList, l_Result);
			(*l_ParameterList).pop_back();
			SNI_DelayedProcessor::GetProcessor()->Delay(p_FunctionDef, l_ParameterList, l_Result);
		}
		if (e3.IsError())
		{
			return e3;
		}
		SN::SN_Error e4 = LoopCleanupUnify(0);
		if (e4.IsError())
		{
			return e4;
		}
		if (worldSet)
		{
			worldSet->CheckDependentWorlds();
			return !worldSet->IsEmpty();
		}
		return e4;
	};

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

	SN::SN_Error SNI_Cartesian::LoopSetupUnify(long p_Depth, const SNI_FunctionDef *p_FunctionDef, SN::SN_Expression * p_InputList, SN::SN_Expression * p_ValueList, SNI_World ** p_WorldList, long &p_CalcPos, long &p_TotalCalc)
	{
		SN::LogContext context("SNI_Cartesian::LoopSetupUnify" + p_FunctionDef->DisplayCpp() + "/" + m_FunctionDef->DisplayCpp() + " Depth = " + to_string(p_Depth) + " Index " + to_string(m_Index) + " Null = " + to_string(m_Null) + m_Expression.DisplayValueSN() + ")");
		SN::SN_Error response(true);
		if (!m_FunctionDef)
		{
			m_FunctionDef = p_FunctionDef;
		}
		if (m_FunctionDef == p_FunctionDef)
		{
			m_InputList = p_InputList;
			m_ValueList = p_ValueList;
			m_WorldList = p_WorldList;

			m_IsReferableValue = false;
			if (m_Null)
			{
				if (m_Expression.IsKnownValue())
				{
					m_Evaluate = m_Expression;
				}
				else
				{
					m_Evaluate = m_Expression.Evaluate();
				}
				if (m_Evaluate.IsKnownValue())
				{
					m_Null = false;
					p_TotalCalc--;
				}
				else
				{
					if (m_Expression.IsReferableValue())
					{
						m_Evaluate = m_Expression.GetVariableValue();
						m_IsReferableValue = true;
					}
					else
					{
						m_Evaluate = SN::SN_ValueSet();
					}
				}
			}

			if (m_Evaluate.IsError())
			{
				SN::SN_Error e = m_Evaluate;
				e.AddNote(context, NULL, "Error construct SNI_Cartesian product");
				return e;
			}
			m_InputList[m_Index] = m_Evaluate;
		}
		else
		{
			p_TotalCalc--;
		}

		if (m_Null)
		{
			p_CalcPos = m_Index;
		}

		long calcPos = p_CalcPos;
		if (calcPos == -1 && m_Cartesian)
		{
			calcPos = m_Cartesian->Index();
		}
		context.LogExpression(p_FunctionDef->DisplayCpp(), " Depth = " + to_string(p_Depth) + " Index " + to_string(m_Index) + " Null = " + to_string(m_Null) + ", IsReferableValue = " + to_string(m_IsReferableValue) + ")");
		context.LogExpression("Expression", m_Expression);
		context.LogExpression("Evaluate", m_Evaluate);
		context.LogText("Check unify cardinality ", p_FunctionDef->DisplayCpp() + " Depth+1 " + to_string(p_Depth + 1) + " Calc Pos " + to_string(calcPos) + ", TotalCalc=" + to_string(p_TotalCalc) + " " + DisplayValues(p_Depth + 1, p_InputList) + ")");

		if (SN::SN_Manager::GetTopManager().MaxCardinalityUnify() < p_FunctionDef->CardinalityOfUnify(p_Depth + 1, p_InputList, calcPos, p_TotalCalc))
		{
			if (m_Cartesian && p_Depth < p_FunctionDef->GetNumParameters())
			{
				response = m_Cartesian->LoopSetupUnify(p_Depth + 1, p_FunctionDef, p_InputList, p_ValueList, p_WorldList, p_CalcPos, p_TotalCalc);
			}
			else
			{
				context.LogText(p_FunctionDef->DisplaySN0(), "Delay");
				return SN::SN_Error(true, true);
			}
		}
		else
		{
			context.LogText(p_FunctionDef->DisplaySN0(), "Partial call at depth+1 " + to_string(p_Depth + 1) + " Index " + to_string(m_Index));
			if (m_Cartesian && p_Depth < p_FunctionDef->GetNumParameters())
			{
				response = m_Cartesian->LoopSetupPartialUnify(p_Depth + 1, p_FunctionDef, p_InputList, p_ValueList, p_WorldList, p_CalcPos, p_TotalCalc);
			}
			else
			{
				context.LogText(p_FunctionDef->DisplaySN0(), "Process now");
			}
		}
		m_FunctionDef = p_FunctionDef;
		m_CalcPos = p_CalcPos;
		m_TotalCalc = p_TotalCalc;
		return response;
	};

	SN::SN_Error SNI_Cartesian::LoopSetupPartialUnify(long p_Depth, const SNI_FunctionDef *p_FunctionDef, SN::SN_Expression * p_InputList, SN::SN_Expression * p_ValueList, SNI_World ** p_WorldList, long &p_CalcPos, long &p_TotalCalc)
	{
		SN::LogContext context("SNI_Cartesian::LoopSetupPartialUnify");
		SN::SN_Error response(true);
		if (!m_FunctionDef)
		{
			m_FunctionDef = p_FunctionDef;
		}
		if (m_FunctionDef == p_FunctionDef)
		{
			if (m_Null)
			{
				m_InputList = p_InputList;
				m_ValueList = p_ValueList;
				m_WorldList = p_WorldList;
				m_IsReferableValue = false;
				m_Evaluate = m_Expression.GetVariableValue();
				if (m_Evaluate.IsKnownValue())
				{
					m_Null = false;
					p_TotalCalc--;
				}
				else
				{
					if (m_Evaluate.IsReferableValue())
					{
						m_IsReferableValue = true;
					}
					else
					{
						m_Evaluate = SN::SN_ValueSet();
					}
				}
			}
			m_InputList[m_Index] = m_Evaluate;
		}
		else
		{
			p_TotalCalc--;
		}

		if (m_Null)
		{
			p_CalcPos = m_Index;
		}

		context.LogExpression(p_FunctionDef->DisplayCpp(), " Depth = " + to_string(p_Depth) + " Index " + to_string(m_Index) + " Null = " + to_string(m_Null) + ", IsReferableValue = " + to_string(m_IsReferableValue) + ")");
		context.LogExpression("Expression", m_Expression);
		context.LogExpression("Evaluate", m_Evaluate);
		context.LogText("Check unify cardinality ", p_FunctionDef->DisplayCpp() + " Depth+1 " + to_string(p_Depth + 1) + " Calc Pos " + to_string(p_CalcPos) + ", TotalCalc=" + to_string(p_TotalCalc) + " " + DisplayValues(p_Depth + 1, p_InputList) + ")");
		if (m_Cartesian && p_Depth < p_FunctionDef->GetNumParameters())
		{
			response = m_Cartesian->LoopSetupPartialUnify(p_Depth + 1, p_FunctionDef, p_InputList, p_ValueList, p_WorldList, p_CalcPos, p_TotalCalc);
		}
		else
		{
			if (SN::SN_Manager::GetTopManager().MaxCardinalityUnify() < p_FunctionDef->CardinalityOfUnify(p_Depth + 1, p_InputList, p_CalcPos, p_TotalCalc))
			{
				context.LogText(p_FunctionDef->DisplaySN0(), "Delay");
				response = SN::SN_Error(true, true);
			}
		}
		
		m_FunctionDef = p_FunctionDef;
		m_CalcPos = p_CalcPos;
		m_TotalCalc = p_TotalCalc;
		return response;
	}

	SN::SN_Error SNI_Cartesian::ProcessValueUnify(const SN::SN_Expression &p_Param, SNI_World*p_World, long p_Depth)
	{
		m_ValueList[m_Index] = p_Param;
		m_WorldList[m_Index] = p_World;
		m_ValueTotalCalc = m_ParamTotalCalc;
		m_ValueCalcPos = m_ParamCalcPos;
		if (m_Null || p_Param.IsNullValue())
		{
			m_ValueCalcPos = m_Index;
		}
		else
		{
			m_ValueTotalCalc--;
		}
		SN::LogContext context("SNI_Cartesian::ProcessValue(" + m_FunctionDef->DisplayCpp() + ", Depth+1 " + to_string(p_Depth + 1) + " )");
		context.LogExpression("Param", p_Param, p_World);
		if (m_Cartesian && p_Depth < m_FunctionDef->GetNumParameters())
		{
			SN::SN_Error e = m_Cartesian->LoopUnify(p_Depth + 1, m_ValueCalcPos, m_ValueTotalCalc, m_WorldSet);
			if (e.IsError())
			{
				e.AddNote(context, NULL, "Error constructing SNI_Cartesian product");
				return e;
			}
		}
		else
		{
			context.LogText("Check cardinality ", m_FunctionDef->DisplayCpp() + " Depth+1 " + to_string(p_Depth+1) + " Value Calc Pos " + to_string(m_ValueCalcPos) + ", Value Total Calc " + to_string(m_ValueTotalCalc) + " " + DisplayValues(p_Depth + 1, m_ValueList, m_WorldList) + ")");
			if (SN::SN_Manager::GetTopManager().MaxCardinalityUnify() < m_FunctionDef->CardinalityOfUnify(p_Depth + 1, m_ValueList, m_ValueCalcPos, m_ValueTotalCalc))
			{
				context.LogText(m_FunctionDef->DisplayCpp(), "Delay");
				bool exists = false;
				SNI_WorldSet *worldSet = m_WorldSet;
				if (0 <= m_CalcPos)
				{
					worldSet = m_ValueList[m_CalcPos].GetWorldSet();
				}
				context.LogText("Join Worlds", m_FunctionDef->DisplayCpp() + " " + DisplayWorlds(p_Depth + 1, m_WorldList));
				SNI_World *world = worldSet->JoinWorldsArray(AutoAddWorld, AlwaysCreateWorld, exists, p_Depth + 1, m_WorldList);
				if (exists)
				{
					context.LogText("exists", world->DisplaySN());
					SN::SN_ParameterList *l_ParameterList = new SN::SN_ParameterList();
					for (long j = 0; j <= p_Depth; j++)
					{
						if (j == m_CalcPos)
						{
							SN::SN_ValueSet vs = m_ValueList[m_CalcPos];
							SN::SN_Variable var;
							var.SetName(vs.GenerateTempVariableName());
							if (!vs.IsNull())
							{
								vs.AddTaggedValue(var, world);
							}
							if (j == PU2_Result)
							{
								m_ValueList[j] = var;
							}
							else
							{
								l_ParameterList->push_back(var);
							}
						}
						else if (j != PU2_Result)
						{
							l_ParameterList->push_back(m_ValueList[j]);
						}
					}
					SNI_DelayedProcessor::GetProcessor()->Delay(m_FunctionDef, l_ParameterList, m_ValueList[PU2_Result], world);
				}
				else
				{
					context.LogText("conflict", m_FunctionDef->DisplayCpp() + " " + DisplayValues(p_Depth + 1, m_ValueList, m_WorldList));
				}
			}
			else
			{
				context.LogText(m_FunctionDef->DisplayCpp(), "Unify");
				SN::SN_Error e = LOG_RETURN(context, (m_FunctionDef->UnifyElement(p_Depth + 1, m_ValueList, m_WorldList, m_ValueCalcPos, m_ValueTotalCalc, m_WorldSet)));
				if (e.IsError())
				{
					return e;
				}
			}
		}
		return SN::SN_Error(true);
	}

	SN::SN_Error SNI_Cartesian::LoopUnify(long p_Depth, long p_ValueCalcPos, long p_ValueTotalCalc, SNI_WorldSet *p_WorldSet)
	{
		m_ParamCalcPos = p_ValueCalcPos;
		m_ParamTotalCalc = p_ValueTotalCalc;
		m_WorldSet = p_WorldSet;

		if (m_Null)
		{
			ProcessValueUnify(m_Evaluate, NULL, p_Depth);
		}
		else
		{
			m_Evaluate.ForEachUnify(this, p_Depth);
		}
		return SN::SN_Error(true);
	}

	SN::SN_Error SNI_Cartesian::LoopCleanupUnify(long p_Depth)
	{
		SN::SN_Error response(true);
		if (m_Cartesian && p_Depth < m_FunctionDef->GetNumParameters())
		{
			response = m_Cartesian->LoopCleanupUnify(p_Depth + 1);
		}
		if (m_Null && !m_IsReferableValue)
		{
			m_Null = false;
			SN::SN_Value simple = m_Evaluate.SimplifyValue();
			if (simple.IsKnownValue())
			{
				SN::SN_Error e = m_Expression.AssertValue(simple);
				if (e.IsError())
				{
					return e;
				}
			}
			else
			{
				m_FunctionDef = NULL;
				m_Null = true;
			}
		}
		else
		{
			m_Expression.Simplify();
		}
		return response;
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
