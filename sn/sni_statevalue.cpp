#include "sni_statevalue.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sn_expression.h"
#include "sn_parameter.h"
#include "sn_value.h"
#include "sn_error.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ SNI_Class *SNI_StateValue::m_Class = NULL;
	/*static*/ SNI_Class *SNI_StateValue::Class()
	{
		if (!m_Class)
		{
			m_Class = new SNI_Class("StateValue");
		}
		return m_Class;
	}

	SNI_Class *SNI_StateValue::VClass()
	{
		return Class();
	}

	SNI_StateValue::SNI_StateValue()
		: m_Value(NULL)
		, m_State(NULL)
	{
	}

	SNI_StateValue::SNI_StateValue(SNI_Expression *p_Value, SNI_Expression *p_State)
		: m_Value(p_Value)
		, m_State(p_State)
	{
	}

	SNI_StateValue::~SNI_StateValue()
	{
	}

	SNI_Expression * SNI_StateValue::Copy() const
	{
		return new SNI_StateValue(m_Value->Copy(), m_State->Copy());
	}

	void SNI_StateValue::PromoteMembers()
	{
		REQUESTPROMOTION(m_Value);
		REQUESTPROMOTION(m_State);
	}

	string SNI_StateValue::GetTypeName() const
	{
		return "StateValue";
	}

	string SNI_StateValue::DisplayCpp() const
	{
		string value = "null";
		string state = "null";
		if (m_Value)
		{
			value = m_Value->DisplayCpp();
		}
		if (m_State)
		{
			state = m_State->DisplayCpp();
		}
		return GetTypeName() + "(" + value + ", " + state + ")";
	}

	string SNI_StateValue::DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const
	{
		string value = "null";
		string state = "null";
		if (m_Value)
		{
			value = m_Value->DisplaySN(0, p_DisplayOptions);
		}
		if (m_State)
		{
			state = m_State->DisplaySN(0, p_DisplayOptions);
		}
		return GetTypeName() + "(" + value + ", " + state + ")";
	}

	long SNI_StateValue::GetPriority() const
	{
		return 0;
	}

	bool SNI_StateValue::Equivalent(SNI_Object * p_Other) const
	{
		SNI_StateValue *otherState = dynamic_cast<SNI_StateValue *>(p_Other);
		return otherState
			&& m_Value && otherState->m_Value
			&& m_State && otherState->m_State
			&& m_Value->Equivalent(otherState->m_Value)
			&& m_State->Equivalent(otherState->m_State);
	}

	size_t SNI_StateValue::Hash() const
	{
		return _Hash_representation(m_Value);
	}

	SNI_Expression* SNI_StateValue::Clone(long p_MetaLevel, SNI_Frame* p_Frame, bool& p_Changed)
	{
		bool changed = false;
		SNI_Expression* value = m_Value->Clone(p_MetaLevel, p_Frame, changed);
		SNI_Expression* state = m_State->Clone(p_MetaLevel, p_Frame, changed);
		if (changed)
		{
			p_Changed = true;
			return dynamic_cast<SNI_Expression*>(new SNI_StateValue(value, state));
		}
		return this;
	}

	SN::SN_Expression SNI_StateValue::DoEvaluate(long p_MetaLevel) const
	{
		if (0 < p_MetaLevel)
		{
			return SN::SN_StateValue(m_Value->DoEvaluate(p_MetaLevel), m_State->DoEvaluate(p_MetaLevel));
		}

		SN::SN_Expression value = m_Value;
		string b1 = value.DisplaySN();
		value = m_Value->DoEvaluate(p_MetaLevel);
		if (value.IsError())
		{
			return value;
		}
		SN::SN_Expression rvalue = value;
		if (value.IsNull())
		{
			rvalue = m_Value->DoPartialEvaluate(p_MetaLevel).GetSNI_Expression();
		}
		const_cast<SNI_StateValue*>(this)->m_Value = rvalue.GetSNI_Expression();
		string s1 = rvalue.DisplaySN();

		SN::SN_Expression state = m_State;
		string b2 = state.DisplaySN();
		state = m_State->DoEvaluate(p_MetaLevel);
		if (state.IsError())
		{
			return state;
		}

		SN::SN_Expression rstate = state;
		if (state.IsNull())
		{
			rstate = m_State->DoPartialEvaluate(p_MetaLevel);
		}
		const_cast<SNI_StateValue*>(this)->m_State = rstate.GetSNI_Expression();
		string s2 = rstate.DisplaySN();
		return this;
	}

	SN::SN_Expression SNI_StateValue::DoPartialEvaluate(long p_MetaLevel) const
	{
		if (0 < p_MetaLevel)
		{
			return SN::SN_StateValue(m_Value->DoPartialEvaluate(p_MetaLevel), m_State->DoPartialEvaluate(p_MetaLevel));
		}

		SN::SN_Expression value = m_Value->DoPartialEvaluate(p_MetaLevel);
		if (value.IsError())
		{
			return value;
		}
		const_cast<SNI_StateValue*>(this)->m_Value = value.GetSNI_Expression();

		SN::SN_Expression state = m_State->DoPartialEvaluate(p_MetaLevel);
		if (state.IsError())
		{
			return state;
		}
		const_cast<SNI_StateValue*>(this)->m_State = state.GetSNI_Expression();
		return this;
	}

	SNI_Expression * SNI_StateValue::GetValue()
	{
		return m_Value;
	}

	SNI_Expression* SNI_StateValue::GetState()
	{
		return m_State;
	}

	SN::SN_Expression SNI_StateValue::DoGetValue() const
	{
		SN::SN_Expression value = m_Value->DoEvaluate();
		if (value.IsError())
		{
			return value;
		}
		SN::SN_Expression rvalue = value;
		if (value.IsNull())
		{
			rvalue = m_Value->DoPartialEvaluate().GetSNI_Expression();
		}
		const_cast<SNI_StateValue*>(this)->m_Value = rvalue.GetSNI_Expression();
		return m_Value;
	}

	SN::SN_Expression SNI_StateValue::DoGetState() const
	{
		SN::SN_Expression state = m_State->DoEvaluate();
		if (state.IsError())
		{
			return state;
		}

		SN::SN_Expression rstate = state;
		if (state.IsNull())
		{
			rstate = m_State->DoPartialEvaluate();
		}
		const_cast<SNI_StateValue*>(this)->m_State = rstate.GetSNI_Expression();
		return m_State;
	}
}
