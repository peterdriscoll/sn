#include "sni_taggedvalue.h"

#include "sn.h"

#include "sni_world.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_TaggedValue::SNI_TaggedValue()
		: m_Value(skynet::Null)
		, m_World(NULL)
		, m_MarkedForDeletion(false)
	{
	}

	SNI_TaggedValue::SNI_TaggedValue(const SN::SN_Expression &p_Value, SNI_World *p_World)
		: m_Value(p_Value)
		, m_World(p_World)
		, m_MarkedForDeletion(false)
	{
	}

	SNI_TaggedValue::SNI_TaggedValue(const SN::SN_Value &p_Value)
		: m_Value(p_Value)
		, m_World(NULL)
		, m_MarkedForDeletion(false)
	{
	}

	SNI_TaggedValue::SNI_TaggedValue(const SN::SN_Expression &p_Value)
		: m_Value(p_Value)
		, m_World(NULL)
		, m_MarkedForDeletion(false)
	{
	}

	SNI_TaggedValue::SNI_TaggedValue(const SNI_TaggedValue &p_TaggedValue)
		: m_Value(p_TaggedValue.m_Value)
		, m_World(p_TaggedValue.m_World)
		, m_MarkedForDeletion(p_TaggedValue.m_MarkedForDeletion)
	{
	}

	SNI_TaggedValue::~SNI_TaggedValue()
	{
	}

	SN::SN_Expression SNI_TaggedValue::GetValue() const
	{
		return m_Value;
	}

	void SNI_TaggedValue::SetValue(const SN::SN_Expression & p_Value)
	{
		m_Value = p_Value;
	}

	SNI_World * SNI_TaggedValue::GetWorld() const
	{
		return m_World;
	}

	void SNI_TaggedValue::SetWorld(SNI_World *p_World)
	{
		m_World = p_World;
	}

	void SNI_TaggedValue::MarkForDeletion()
	{
		m_MarkedForDeletion = true;
	}
	bool SNI_TaggedValue::IsMarkedForDeletion()
	{
		return m_MarkedForDeletion;
	}

	void SNI_TaggedValue::PromoteMembersExternal(PGC::PGC_Base * p_Base)
	{
		p_Base->REQUESTPROMOTION(m_Value.GetSNI_ExpressionRef());
		p_Base->REQUESTPROMOTION(m_World);
	}
}
