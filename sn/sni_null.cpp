#include "sni_null.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Null::SNI_Null()
	{
	}

	SNI_Null::SNI_Null(const SNI_Null & /* p_Other */)
	{
	}

	SNI_Null::~SNI_Null()
	{
	}

	void SNI_Null::PromoteMembers()
	{
	}

	string SNI_Null::GetTypeName() const
	{
		return "Null";
	}

	string SNI_Null::DisplayCpp() const
	{
		return "NULL";
	}

	string SNI_Null::DisplaySN(long /*priority*/, SNI_VariableConstPointerList & /*p_DisplayVariableList*/) const
	{
		return "null";
	}

	long SNI_Null::GetPriority() const
	{
		return 100;
	}

	bool SNI_Null::Equivalent(SNI_Object * p_Other) const
	{
		return dynamic_cast<SNI_Null *>(p_Other) != NULL;
	}

	size_t SNI_Null::Cardinality(size_t p_MaxCardinality)
	{
		return p_MaxCardinality;
	}

	bool SNI_Null::IsNull() const
	{
		return true;
	}

	bool SNI_Null::IsNullValue() const
	{
		return true;
	}

	bool SNI_Null::IsKnownValue() const
	{
		return false;
	}

	bool SNI_Null::IsReferableValue() const
	{
		return false;
	}

	SN::SN_Error SNI_Null::AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World **p_WorldList, SNI_WorldSet *p_WorldSet)
	{
		if (!p_WorldSet)
		{
			return true;
		}

		// The assumption is that no one cares about the value. That is why it is given as Null.
		// Their could be an active Null, but that would be a value set.
		bool exists = false;
		SNI_WorldSet *l_WorldSet = p_WorldSet;
		return p_Value.ForEach(
			[l_WorldSet, &exists, p_NumWorlds, p_WorldList, p_Value](const SN::SN_Expression &p_Param, SNI::SNI_World *p_World) -> SN::SN_Error
			{
				SNI_World *world = l_WorldSet->JoinWorldsArray(AutoAddWorld, AlwaysCreateWorld, exists, p_NumWorlds, p_WorldList, p_World);
				return true;
			}
		);
	}
}
