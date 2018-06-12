//  SNI_DelayedCall class
//
//  Description:
//      Some function calls do not have sufficient information yet to calculate the unknown variables.
//      Every call represents a relationship between variables. Variables may have no value, a set of
//      values or one precise value.
//      Excluding the variable being calculated, the determinancy of the calculation is an estimate of the
//      number of values may be generated for the calculated variable. It is the product of the number of values 
//      for the other variables.

#include "sni_delayedcall.h"

#include "sn_error.h"

#include "sni_variable.h"
#include "sni_error.h"
#include "sni_null.h"
#include "sni_world.h"
#include "sni_helpers.h"

#include <climits>

#include "sn_pch.h"

#pragma warning (disable: 4018)

namespace SNI
{
	SNI_DelayedCall::SNI_DelayedCall()
		: m_ParameterList(NULL)
		, m_Scheduled(false)
		, m_Frame(NULL)
		, m_World(NULL)
	{
	}

	SNI_DelayedCall::SNI_DelayedCall(SN::SN_FunctionDef p_Function, SN::SN_Expression *p_ParameterList, SNI_Frame *p_Frame, SNI_World * p_World)
		: m_Function(p_Function)
		, m_ParameterList(p_ParameterList)
		, m_Scheduled(false)
		, m_Locked(false)
		, m_Frame(p_Frame)
		, m_World(NULL)
	{
	}

	SNI_DelayedCall::~SNI_DelayedCall()
	{
	}

	// Estimate the number of values that will be created in the unknown variable. This value may be infinite, represented by LONG_MAX.
	size_t SNI_DelayedCall::CallCardinality() const
	{
		return m_Function.GetSNI_FunctionDef()->Cardinality(m_ParameterList);
	}

	bool SNI_DelayedCall::IsNull() const
	{
		return true;
	}

	bool SNI_DelayedCall::Run()
	{
		if (m_Frame)
		{
			SNI_Frame::PushFrame(m_Frame);
		}
		m_Error = m_Function.GetSNI_FunctionDef()->UnifyArray(m_ParameterList);
		if (m_Frame)
		{
			SNI_Frame::Pop();
		}
		if (m_Error.IsError())
		{
			if (m_World)
			{
				m_World->Fail();
				return true;
			}
		}
		return m_Error.GetBool();
	}

	bool SNI_DelayedCall::IsCallRequested() const
	{
		for (size_t i = 0; i < m_Function.GetSNI_FunctionDef()->GetNumParameters(); i++)
		{
			SN::SN_Expression var = m_ParameterList[i];
			if (var.IsRequested())
			{
				return true;
			}
		}
		return false;
	}

	// Attach this delayed call as the value of any parameters, and the result that are variables.
	void SNI_DelayedCall::LinkToVariables()
	{
		for (size_t i = 0; i < m_Function.GetSNI_FunctionDef()->GetNumParameters(); i++)
		{
			LinkVariable(m_ParameterList[i]);
		}
	}


	void SNI_DelayedCall::LinkVariable(SN::SN_Expression &p_Parameter)
	{
		if (SN::Is<SNI_Variable*>(p_Parameter))
		{
			SNI_Variable *variable = dynamic_cast<SNI_Variable*>(p_Parameter.GetSNI_Expression());
			variable->AttachDelayedCall(this);
		}
	}

	void SNI_DelayedCall::SetWorld(SNI_World * p_World)
	{
		m_World = p_World;
	}

	bool SNI_DelayedCall::IsLocked() const
	{
		return m_Locked;
	}
	void SNI_DelayedCall::Lock()
	{
		m_Locked = true;
	}

	void SNI_DelayedCall::Unlock()
	{
		m_Locked = false;
	}

	void SNI_DelayedCall::Display()
	{
		LOG(WriteLine(SN::DebugLevel, "  " + m_Function.GetSNI_FunctionDef()->GetLogDescription(m_ParameterList)));
	}

	bool SNI_DelayedCall::IsScheduled()
	{
		return m_Scheduled;
	}

	void SNI_DelayedCall::MarkScheduled()
	{
		m_Scheduled = true;
	}
}
