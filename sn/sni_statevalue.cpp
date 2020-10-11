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
		: m_State(NULL)
		, m_Value(NULL)
	{
	}

	SNI_StateValue::SNI_StateValue(SNI_Expression *p_State, SNI_Expression *p_Value)
		: m_State(p_State)
		, m_Value(p_Value)
	{
	}

	SNI_StateValue::~SNI_StateValue()
	{
	}

	SNI_Expression * SNI_StateValue::Copy() const
	{
		return new SNI_StateValue(dynamic_cast<SNI_State *>(m_State->Copy()), m_Value->Copy());
	}

	void SNI_StateValue::PromoteMembers()
	{
		REQUESTPROMOTION(m_State);
		REQUESTPROMOTION(m_Value);
	}

	string SNI_StateValue::GetTypeName() const
	{
		return "StateValue";
	}

	string SNI_StateValue::DisplayCpp() const
	{
		string state = "null";
		string value = "null";
		if (m_State)
		{
			state = m_State->DisplayCpp();
		}
		if (m_Value)
		{
			value = m_Value->DisplayCpp();
		}

		return GetTypeName() + "(" + state + ", " + value + ")";
	}

	string SNI_StateValue::DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const
	{
		string state = "null";
		string value = "null";
		if (m_State)
		{
			state = m_State->DisplaySN(0, p_DisplayOptions);
		}
		if (m_Value)
		{
			value = m_Value->DisplaySN(0, p_DisplayOptions);
		}

		return GetTypeName() + "(" + state + ", " + value + ")";
	}

	long SNI_StateValue::GetPriority() const
	{
		return 0;
	}

	bool SNI_StateValue::Equivalent(SNI_Object * p_Other) const
	{
		SNI_StateValue *otherState = dynamic_cast<SNI_StateValue *>(p_Other);
		return otherState
			&& m_State && otherState->m_State
			&& m_Value && otherState->m_Value
			&& m_State->Equivalent(otherState->m_State)
			&& m_Value->Equivalent(otherState->m_Value);
	}

	size_t SNI_StateValue::Hash() const
	{
		return 0; // _Hash_representation(m_Map);
	}

	SNI_Expression * SNI_StateValue::GetState()
	{
		return m_State;
	}
	
	SNI_Expression * SNI_StateValue::GetValue()
	{
		return m_Value;
	}
}
