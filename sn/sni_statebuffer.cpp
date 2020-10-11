#include "sni_statebuffer.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_StateBuffer::SNI_StateBuffer()
		: m_LastStateId(0)
	{

	}

	SNI_StateBuffer::~SNI_StateBuffer()
	{

	}

	void SNI_StateBuffer::PromoteMembers()
	{
		for (auto &pair : m_Map)
		{
			REQUESTPROMOTION(pair.first.m_LValue);
			REQUESTPROMOTION(pair.second);
		}
	}

	bool SNI_StateBuffer::Equivalent(SNI_Object * p_Other) const
	{
		if (dynamic_cast<SNI_StateBuffer *>(p_Other))
		{
			SNI_StateBuffer * l_mapping = dynamic_cast<SNI_StateBuffer *>(p_Other);
			return m_Map == l_mapping->m_Map;
		}
		return false;
	}

	unsigned long long SNI_StateBuffer::Compose(SNI_Assignment * p_Assignment)
	{
		++m_LastStateId;
		StateBufferKey key(dynamic_cast<SNI_Value *>(p_Assignment->LeftHandSide()), m_LastStateId);
		m_Map.insert(std::make_pair(key, p_Assignment->RightHandSide()));
		return m_LastStateId;
	}
}
