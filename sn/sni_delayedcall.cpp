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
	SNI_DelayedCall::SNI_DelayedCall(SN::SN_FunctionDef p_Function, size_t p_NumParams, SN::SN_Expression *p_ParamList, const SNI_Expression *p_Source, SNI_Frame *p_Frame, SNI_World * p_World)
		: m_Function(p_Function)
		, m_NumParams(p_NumParams)
		, m_ParamList(NULL)
		, m_Source(p_Source)
		, m_Scheduled(false)
		, m_Locked(false)
		, m_Frame(p_Frame)
		, m_World(p_World)
		, m_Error(skynet::OK)
	{
		ASSERTM(m_NumParams == m_Function.GetSNI_FunctionDef()->GetNumParameters(), "Inconsistent number of parameters.");
		m_ParamList = new SN::SN_Expression[m_NumParams];
		for (long j = 0; j < m_NumParams; j++)
		{
			m_ParamList[j] = p_ParamList[j];
		}
	}

	SNI_DelayedCall::~SNI_DelayedCall()
	{
		delete[] m_ParamList;
	}

	std::string SNI_DelayedCall::GetTypeName() const
	{
		return "Delayed";
	}

	std::string SNI_DelayedCall::DisplaySN(long priority, SNI_DisplayOptions & p_DisplayOptions) const
	{
		return "[["
			+ m_Function.GetSNI_FunctionDef()->DisplayCall(0, p_DisplayOptions, m_NumParams, m_ParamList + 1, m_Source)
			+ "="
			+ m_ParamList[PU1_Result].DisplaySN(p_DisplayOptions)
			+ "]]";
	}

	void SNI_DelayedCall::AddVariables(long p_MetaLevel, SNI_VariablePointerMap& p_Map)
	{
		m_Function.GetSNI_FunctionDef()->AddVariablesForCall(p_MetaLevel, p_Map, m_NumParams, m_ParamList);
	}

	// Estimate the number of values that will be created in the unknown variable. This value may be infinite, represented by LONG_MAX.
	size_t SNI_DelayedCall::CallCardinality() const
	{
		return m_Function.GetSNI_FunctionDef()->Cardinality(m_ParamList);
	}

	void SNI_DelayedCall::ExpandedBooleanResult()
	{
		m_Function.GetSNI_FunctionDef()->ExpandedBooleanResult(m_ParamList);
	}

	bool SNI_DelayedCall::IsNull() const
	{
		return true;
	}

	SN::SN_Error SNI_DelayedCall::GetError() const
	{
		return m_Error;
	}

	SN::SN_Error SNI_DelayedCall::Run()
	{
		if (m_Frame)
		{
			SNI_Frame::PushFrame(m_Frame);
		}
		m_Error = m_Function.GetSNI_FunctionDef()->UnifyArray(m_ParamList, m_Source).GetError();
		if (m_Frame)
		{
			SNI_Frame::Pop();
		}
		if (m_Error.IsError())
		{
			if (m_World)
			{
				SN::SN_Error err = m_World->Fail(IncompatibleValue);
				if (err.IsError())
				{
					return err;
				}
			}
		}
		return m_Error;
	}

	bool SNI_DelayedCall::IsCallRequested() const
	{
		for (size_t i = 0; i < m_NumParams; i++)
		{
			SN::SN_Expression var = m_ParamList[i];
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
			LinkVariable(m_ParamList[i]);
		}
	}


	void SNI_DelayedCall::LinkVariable(SN::SN_Expression &p_Parameter)
	{
		if (p_Parameter.IsVariable())
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
		LOG(WriteLine(SN::DebugLevel, "  " + m_Function.GetSNI_FunctionDef()->GetLogDescription(m_ParamList)));
	}

	bool SNI_DelayedCall::IsScheduled()
	{
		return m_Scheduled;
	}

	void SNI_DelayedCall::MarkScheduled()
	{
		m_Scheduled = true;
	}

	bool SNI_DelayedCall::EmptyWorld()
	{
		return m_World && m_World->IsEmpty();
	}
	
	void SNI_DelayedCall::WriteJSON(ostream &p_Stream, SNI::SNI_DisplayOptions &p_DisplayOptions)
	{
		if (!EmptyWorld())
		{
			p_Stream << "\t\t\"expression\" : \"" << EscapeStringToJSON(m_Function.GetSNI_FunctionDef()->DisplayCall(0, p_DisplayOptions, m_NumParams, m_ParamList + 1, m_Source)) << "\",\n";
			p_Stream << "\t\t\"result\" : \"" << EscapeStringToJSON(m_ParamList[PU1_Result].DisplayValueSN(p_DisplayOptions)) << "\",\n";
			size_t card = CallCardinality();
			std::string card_string = "&infin;";
			if (card < CARDINALITY_MAX)
			{
				card_string = to_string(card);
			}
			p_Stream << "\t\t\"cardinality\" : \"" << card_string << "\",\n";
			SNI_DisplayOptions displayOptions(doTextOnly);
			p_Stream << "\t\t\"world\" : \"" << (m_World?m_World->DisplaySN(displayOptions):"") << "\"\n";
		}
	}

	void SNI_DelayedCall::PromoteMembers()
	{

		// m_Function;
		for (size_t j = 0; j < m_NumParams; j++)
		{
			RequestPromotion((PGC::PGC_TypeCheck **)&(m_ParamList[j].GetSNI_ExpressionRef()));
		}
		/*
		RequestPromotion((PGC::PGC_TypeCheck**)&m_Source);
		RequestPromotion((PGC::PGC_TypeCheck**)&m_Frame);
		//SNI_World             *m_World;
		//SN::SN_Error          m_Error;
*/
	}
}
