#include "sni_mapping.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sn_expression.h"
#include "sn_parameter.h"
#include "sn_value.h"
#include "sn_error.h"

#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ SNI_Class *SNI_Mapping::m_Class = NULL;
	/*static*/ SNI_Class *SNI_Mapping::Class()
	{
		if (!m_Class)
		{
			m_Class = new SNI_Class("Mapping");
		}
		return m_Class;
	}

	SNI_Class *SNI_Mapping::VClass()
	{
		return Class();
	}

	SNI_Mapping::SNI_Mapping()
	: m_Fixed(false)
	, m_DefaultValue(NULL)
	{

	}

	SNI_Mapping::~SNI_Mapping()
	{

	}

	SNI_Expression * SNI_Mapping::Copy() const
	{
		return new SNI_Mapping(*this);
	}

	void SNI_Mapping::PromoteMembers()
	{
		for (auto &pair : m_Map)
		{
			REQUESTPROMOTION(pair.first);
			REQUESTPROMOTION(pair.second);
		}
		REQUESTPROMOTION(m_DefaultValue);
	}

	string SNI_Mapping::GetTypeName() const
	{
		return "Mapping";
	}

	string SNI_Mapping::DisplayCpp() const
	{
		return GetTypeName() + "()";
	}

	string SNI_Mapping::DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const
	{
		return GetTypeName() + "()";
	}

	long SNI_Mapping::GetPriority() const
	{
		return 3;
	}

	bool SNI_Mapping::Equivalent(SNI_Object * p_Other) const
	{
		if (m_Fixed)
		{
			if (dynamic_cast<SNI_Mapping *>(p_Other))
			{
				SNI_Mapping * l_mapping = dynamic_cast<SNI_Mapping *>(p_Other);
				for (auto &pair : m_Map)
				{
					if (!pair.second.Equivalent(l_mapping->DoSubscriptCall(pair.first.GetSNI_Expression())))
					{
						return false;
					}
				}

				for (auto &pair : l_mapping->m_Map)
				{
					if (!pair.second.Equivalent(DoSubscriptCall(pair.first.GetSNI_Expression())))
					{
						return false;
					}
				}
			}
		}
		return false;
	}

	size_t SNI_Mapping::Hash() const
	{
		return 0; // _Hash_representation(m_Map);
	}

	bool SNI_Mapping::IsFixed() const
	{
		return m_Fixed;
	}

	SN::SN_Error SNI_Mapping::AssertSubscriptValue(const SNI_Value * p_Index, SN::SN_Expression p_Result)
	{
		auto it = m_Map.find(p_Index);
		if (it == m_Map.end())
		{
			m_Map.emplace(p_Index, p_Result);
			return skynet::OK;
		}
		return p_Result.AssertValue(it->second);
	}

	// Functions
	SN::SN_Value SNI_Mapping::DoSubscriptCall(const SN::SN_Value & p_Index) const
	{
		return m_Map.at(p_Index);
	}

	SN::SN_Value SNI_Mapping::DoReverseSubscript(const SN::SN_Value & p_Result) const
	{
		if (!m_Map.size())
		{
			return SN::SN_Error(false, false, GetTypeName() + " No values in map.");
		}
		if (!m_Fixed)
		{
			return SN::SN_Error(false, false, GetTypeName() + " Default value not fixed.");
		}
		if (p_Result.Equivalent(m_DefaultValue))
		{
			return SN::SN_Error(false, false, GetTypeName() + " Undefined operation, because result matches default " + m_DefaultValue->DisplaySN0());
		}
		SN::SN_ValueSet vs;
		SNI_WorldSet *worldSet = vs.GetWorldSet();
		for (auto &pair : m_Map)
		{
			if (p_Result.Equivalent(pair.second))
			{
				SN::SN_Error err = vs.AddValue(pair.first, 0, NULL, worldSet);
				if (err.IsError())
				{
					return err;
				}
			}
		}
		if (vs.Cardinality())
		{
			return vs;
		}
		return SN::SN_Error(false, false, GetTypeName() + " No matching values in map.");
	}

	SN::SN_Value SNI_Mapping::DoCountIf(SN::SN_Expression p_Value) const
	{
		if (!m_Fixed)
		{
			return SN::SN_Error(false, false, GetTypeName() + " CountIf - Number of values in map may change. Fix the map before counting.");
		}
		SN_DECLARE_VALUE(countCondition, p_Value);
		long count = 0;
		for (auto &pair : m_Map)
		{
			if (p_Value.IsNull() || countCondition(pair.second).DoEvaluate().GetBool())
			{
				++count;
			}
			Breakpoint(SN::DebugStop, SN::RightId, GetTypeName(), "Process next value", this, SN::CallPoint);
		}
		return SN::SN_Long(count);
	}

	SN::SN_Value SNI_Mapping::DoCountAll() const
	{
		if (!m_Fixed)
		{
			return SN::SN_Error(false, false, GetTypeName() + " CountAll - Number of values in map may change. Fix the map before counting.");
		}
		return SN::SN_Long(static_cast<long>(m_Map.size()));
	}

	SN::SN_Value SNI_Mapping::DoSum() const
	{
		if (!m_Fixed)
		{
			return SN::SN_Error(false, false, GetTypeName() + " Sum - Number of values in map may change. Fix the map before summing.");
		}
		SN::SN_Expression sum;
		for (auto &pair : m_Map)
		{
			if (pair.second.IsNullValue())
			{
				return SN::SN_Error(false, false, GetTypeName() + " Sum - Null value found.");
			}
			if (sum.IsNull())
			{
				sum = pair.second;
			}
			else
			{
				sum = sum.GetSNI_Value()->DoAdd(pair.second.GetSNI_Value());
			}
		}
		return sum;
	}

	void SNI_Mapping::Fix(SN::SN_Expression p_Value)
	{
		m_Fixed = true;
		m_DefaultValue = p_Value.GetSNI_Expression();
	}
}
