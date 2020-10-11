#include "sn_assignment.h"

#include "sni_assignment.h"

#include "sn_pch.h"

namespace SN
{
	SN_Assignment::SN_Assignment(const SN_Assignment &p_Assignment)
		: SN_Expression(p_Assignment.m_Expression)
	{
		m_Expression->CreateId();
	}

	SN_Assignment::SN_Assignment(const SN_Expression & p_LeftHandSide, const SN_Expression & p_RightHandSide)
		: SN_Expression(new SNI::SNI_Assignment(p_LeftHandSide.GetSNI_Expression(), p_RightHandSide.GetSNI_Expression()))
	{
		m_Expression->CreateId();
	}

	SN_Assignment::~SN_Assignment()
	{
	}

	SN_Expression SN_Assignment::LeftHandSide() const
	{
		return GetSNI_Assignment()->LeftHandSide();
	}

	SN_Expression SN_Assignment::RightHandSide() const
	{
		return GetSNI_Assignment()->RightHandSide();
	}

	SNI::SNI_Assignment * SN_Assignment::GetSNI_Assignment() const
	{
		return dynamic_cast<SNI::SNI_Assignment *>(m_Expression);
	}
}
