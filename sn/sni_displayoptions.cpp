#include "sni_displayoptions.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_DisplayOptions::SNI_DisplayOptions(enum DisplayOptionType p_DebugHTML)
		: m_Level(0)
		, m_MaxLevel(2)
	{
		m_DebugHTML = p_DebugHTML;

	}

	SNI_DisplayOptions::~SNI_DisplayOptions()
	{
	}

	void SNI_DisplayOptions::SetVarName(const string &p_VarName)
	{
		m_VarName = p_VarName;
		m_TypeMap.clear();
	}

	string SNI_DisplayOptions::CreateName(const string &p_TypeName)
	{
		if (p_TypeName == "IsA")
		{
			long dog = 10;
		}
		long id = 0;
		auto it = m_TypeMap.find(p_TypeName);
		if (it != m_TypeMap.end())
		{
			id = it->second;
		}
		m_TypeMap[p_TypeName] = id+1;
		return m_VarName + "_" + p_TypeName + "_" + to_string(id);
	}

	enum DisplayOptionType SNI_DisplayOptions::GetDebugHTML()
	{
		return m_DebugHTML;
	}

	void SNI_DisplayOptions::IncrementLevel()
	{
		++m_Level;
	}

	void SNI_DisplayOptions::DecrementLevel()
	{
		--m_Level;
	}

	long SNI_DisplayOptions::GetLevel()
	{
		return m_Level;
	}

	bool SNI_DisplayOptions::CheckLevel()
	{
		return m_Level < m_MaxLevel;
	}
}
