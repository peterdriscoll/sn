#include "sn_debug.h"

#include "sni_debug.h"

#include "sn_pch.h"

namespace SN
{
	SN_Debug::SN_Debug(const SN_Expression &p_Expression)
		: SN_Expression(new SNI::SNI_Debug(p_Expression))
	{
	}

	SN_Debug::~SN_Debug()
	{
	}

	SNI::SNI_Debug* SN_Debug::GetSNI_Debug() const
	{
		return dynamic_cast<SNI::SNI_Debug*>(m_Expression);
	}
}
