#include "sni_splitter.h"

#include "sn_expression.h"

#include "sni_worldset.h"
#include "sni_world.h"

#include "sn_pch.h"


SNI_Splitter::SNI_Splitter()
{
}


SNI_Splitter::~SNI_Splitter()
{
}

bool SNI_Splitter::PositiveNotNull()
{
	m_PositiveSimple = m_Positive.SimplifyValue();
	return !m_PositiveSimple.IsNullValue();
}

bool SNI_Splitter::NegativeNotNull()
{
	m_NegativeSimple = m_Negative.SimplifyValue();
	return !m_NegativeSimple.IsNullValue();
}

SN::SN_Expression SNI_Splitter::Positive()
{
	return m_PositiveSimple;
}

SN::SN_Expression SNI_Splitter::Negative()
{
	return m_NegativeSimple;
}

void SNI_Splitter::ProcessValueSplit(const SN::SN_Expression & p_Param, SNI_World * p_World)
{
	if (p_Param.GetBool())
	{
		m_Positive.AddTaggedValue(skynet::True, p_World);
		m_Negative.AddTaggedValue(skynet::Null, p_World);
	}
	else
	{
		m_Negative.AddTaggedValue(skynet::True, p_World);
		m_Positive.AddTaggedValue(skynet::Null, p_World);
	}
}
