#include "sni_displayoptions.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_DisplayOptions::SNI_DisplayOptions()
	{

	}

	SNI_DisplayOptions::~SNI_DisplayOptions()
	{

	}

	bool SNI_DisplayOptions::FindVariable(SNI_Variable *p_Variable) const
	{
		for (unsigned long j = 0; j<m_DisplayVariableList.size(); j++)
		{
			if (m_DisplayVariableList[j] == p_Variable)
			{
				return true;
			}
		}
		return false;
	}
}
