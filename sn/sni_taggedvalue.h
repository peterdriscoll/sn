#if !defined(SNI_TAGGEDVALUE_H_INCLUDED)
#define SNI_TAGGEDVALUE_H_INCLUDED

#pragma once

#include "sn_value.h"
#include "ref.h"

namespace SN
{
	class SNI_World;
}

namespace SNI
{
	class SNI_TaggedValue
	{
		PGC_CLASS(SNI_TaggedValue);
	public:
		SNI_TaggedValue();
		SNI_TaggedValue(const SNI_TaggedValue &p_TaggedValue);
		SNI_TaggedValue(const SN::SN_Value &p_Value);
		SNI_TaggedValue(const SN::SN_Expression &p_Value);
		SNI_TaggedValue(const SN::SN_Expression &p_Value, SNI_World *p_World);
		virtual ~SNI_TaggedValue();

		virtual SN::SN_Expression GetValue() const;
		virtual void SetValue(const SN::SN_Expression &p_Value);
		virtual SNI_World * GetWorld() const;
		virtual void SetWorld(SNI_World * p_World);
		void MarkForDeletion();
		bool IsMarkedForDeletion();

		void PromoteMembersExternal(PGC::PGC_Base *p_Base);
	private:
		SN::SN_Expression m_Value;
		SNI_World *  m_World;
		bool m_MarkedForDeletion;
	};

	typedef vector<SNI_TaggedValue> SNI_TaggedValueList;
}

#endif // !defined(SNI_TAGGEDVALUE_H_INCLUDED)
