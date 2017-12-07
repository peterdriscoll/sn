#include "sni_frame.h"

#include "sn_pch.h"

namespace SNI
{
	long m_MaxThreadNum = 0;
	thread_local long t_ThreadNum = -1;
	thread_local long t_MaxFrameNum = 0;
	long GetThreadNum()
	{
		if (t_ThreadNum == -1)
		{
			t_ThreadNum = ++m_MaxThreadNum;
		}
		return t_ThreadNum;
	}

	SNI_Frame::SNI_Frame()
		: m_ThreadNum(GetThreadNum())
		, m_FrameNum(++t_MaxFrameNum)

	{
	}

	SNI_Frame::~SNI_Frame()
	{
	}

	SNI_ReplacementList & SNI_Frame::GetReplacementList()
	{
		return m_ReplacementList;
	}

	SNI_Expression *SNI_Frame::ReplaceVariable(SNI_Variable *p_Variable, bool &p_Changed)
	{
		for (unsigned long j = 0; j < m_ReplacementList.size(); j++)
		{

			SNI_Variable* l_to = m_ReplacementList[j].ReplaceVariable(p_Variable);
			if (l_to)
			{
				p_Changed = true;
				return dynamic_cast<SNI_Expression *>(l_to);
			}
		}
		return p_Variable;
	}

	void SNI_Frame::PromoteMembers()
	{
	}
}
