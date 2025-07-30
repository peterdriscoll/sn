#include "sn_valueset.h"
#include "sn_value.h"
#include "sn_expression.h"
#include "sn_error.h"
#include "sn_null.h"
#include "sn_meta.h"

#include "sni_valueset.h"
#include "sni_value.h"
#include "sni_expression.h"
#include "sni_error.h"
#include "sni_null.h"
#include "sni_meta.h"

#include "sn_pch.h"

namespace SN
{
	SN_ValueSet::SN_ValueSet()
		: SN_Expression(new SNI::SNI_ValueSet())
	{
	}

	SN_ValueSet::SN_ValueSet(const SN_Value &p_Value, long)
		: SN_Expression(dynamic_cast<SNI::SNI_ValueSet *>(p_Value.GetSNI_Value()))
	{
	}

	SN_ValueSet::SN_ValueSet(const SN_Expression &p_Expression)
		: SN_Expression(dynamic_cast<SNI::SNI_ValueSet *>(p_Expression.GetSNI_Expression()))
	{
	}

	SN_ValueSet::SN_ValueSet(const SN_ValueSet &p_Value, const SN_Value &p_right)
		: SN_Expression(p_Value)
	{
		GetSNI_ValueSet()->Append(p_right);
	}

	SN_ValueSet::SN_ValueSet(const SN_Value &p_left, const SN_Value &p_right)
		: SN_Expression(new SNI::SNI_ValueSet(p_left, p_right))

	{
	}

	SN_ValueSet::SN_ValueSet(const SN_Value &p_Value)
	{
		if (Is<SNI::SNI_ValueSet *>(p_Value))
		{
			m_Expression = dynamic_cast<SNI::SNI_Expression *>(p_Value.GetSNI_Value());
		}
		else
		{
			m_Expression = dynamic_cast<SNI::SNI_Expression *>(new SNI::SNI_ValueSet(p_Value));
		}
	}

	SN_ValueSet::SN_ValueSet(const SN_Meta &p_Meta)
		: SN_Expression(dynamic_cast<SNI::SNI_ValueSet *>(p_Meta.GetSNI_Meta()))
	{
	}

	SN_ValueSet::SN_ValueSet(const SN_Null &p_Null)
		: SN_Expression(dynamic_cast<SNI::SNI_ValueSet *>(p_Null.GetSNI_Null()))
	{
	}

	SN_ValueSet::SN_ValueSet(const SN_Error &p_Error)
		: SN_Expression(dynamic_cast<SNI::SNI_ValueSet *>(p_Error.GetSNI_Error()))
	{
	}

	SN_ValueSet::SN_ValueSet(const SNI::SNI_ValueSet *p_ValueSet)
		: SN_Expression(p_ValueSet)
	{
	}

	SN_ValueSet::SN_ValueSet(const vector<SNI::SNI_TaggedValue> & /*p_Value*/)
	{
	}

	SN_ValueSet::~SN_ValueSet()
	{
	}

	SN_ValueSet SN_ValueSet::DoRemove(const SN_Value & p_Other)
	{
		return m_Expression->DoRemove(p_Other);
	}

	SNI::SNI_ValueSet * SN_ValueSet::GetSNI_ValueSet() const
	{
		return dynamic_cast<SNI::SNI_ValueSet *>(m_Expression);
	}

	void SN_ValueSet::AddTaggedValue(const SN_Expression &p_Value, SNI::SNI_World *p_World)
	{
		GetSNI_ValueSet()->AddTaggedValue(p_Value, p_World);
	}

	SN_Error SN_ValueSet::AddValue(SN_Expression p_Value, long p_NumWorlds, SNI::SNI_World **p_WorldList, SNI::SNI_WorldSet *p_WorldSet)
	{
		return GetSNI_ValueSet()->AddValue(p_Value, p_NumWorlds, p_WorldList, p_WorldSet);
	}

	string SN_ValueSet::GenerateTempVariableName()
	{
		return GetSNI_ValueSet()->GenerateTempVariableName();
	}

	SNI::SNI_TaggedValue &SN_ValueSet::operator[](size_t p_index)
	{
		return (*GetSNI_ValueSet())[p_index];
	}

	size_t SN_ValueSet::Length() const
	{
		return GetSNI_ValueSet()->Length();
	}

	SNI::SNI_WorldSet *SN_ValueSet::GetWorldSet()
	{
		return GetSNI_ValueSet()->GetWorldSet();
	}

	void SN_ValueSet::SetWorldSet(SNI::SNI_WorldSet * p_WorldSet)
	{
		GetSNI_ValueSet()->SetWorldSet(p_WorldSet);
	}

	void SN_ValueSet::Simplify()
	{
		GetSNI_ValueSet()->Simplify();
	}
}
