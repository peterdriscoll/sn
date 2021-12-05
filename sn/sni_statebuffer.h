#if !defined(SNI_STATEBUFFER_H_INCLUDED)
#define SNI_STATEBUFFER_H_INCLUDED

#pragma once

#include <map>

#include "sni_value.h"

namespace SN
{
	class SN_Expression;
}

namespace SNI
{
	class SNI_Class;
	class SNI_Expression;

	struct StateBufferKey
	{
		const SNI_Value *m_LValue;
		const unsigned long long m_StateId;

		StateBufferKey(SNI_Value *p_LValue, unsigned long long p_StateId)
			: m_LValue(p_LValue)
			, m_StateId(p_StateId)
		{};

		bool operator<(const StateBufferKey &p_Right) const
		{
			return *m_LValue < *p_Right.m_LValue
				|| (*m_LValue == *p_Right.m_LValue && m_StateId < p_Right.m_StateId);
		}

		bool operator==(const StateBufferKey &p_Right) const
		{
			return *m_LValue == *p_Right.m_LValue && m_StateId == p_Right.m_StateId;
		}
	};

	class SNI_StateBuffer : public SNI_Value
	{
		PGC_CLASS(SNI_StateBuffer);
	public:
		SNI_StateBuffer();
		virtual ~SNI_StateBuffer();

		virtual bool Equivalent(SNI_Object * p_Other) const;

		unsigned long long DoAssign(SNI_Value* p_Left, SNI_Expression* p_Right);

	protected:
		virtual void PromoteMembers();

	private:
		unsigned long long m_LastStateId;
		map<StateBufferKey, SNI_Expression *> m_Map;
	};
}

#endif // !defined(SNI_STATEBUFFER_H_INCLUDED)
