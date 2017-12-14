#include "sni_frame.h"

#include "sni_variable.h"

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
	SNI_FrameList m_FrameStack;

	/*static*/ SNI_Frame *SNI_Frame::Push()
	{
		SNI_Frame *newFrame = new SNI_Frame;
		m_FrameStack.push_back(newFrame);
		return newFrame;
	}

	/*static*/ void SNI_Frame::Pop()
	{
		m_FrameStack.pop_back();
	}

	/*static*/ long SNI_Frame::GetThreadNum()
	{
		return t_ThreadNum;
	}

	/*static*/ long SNI_Frame::GetFrameStackDepth()
	{
		return m_FrameStack.size();
	}

	SNI_Frame::SNI_Frame()
		: m_ThreadNum(GetThreadNum())
		, m_FrameNum(++t_MaxFrameNum)

	{
	}

	SNI_Frame::~SNI_Frame()
	{
	}

	SNI_Expression *SNI_Frame::ReplaceVariable(SNI_Variable *p_Variable, bool &p_Changed)
	{

		SNI_Variable* l_Result = p_Variable;
		for (SNI_Replacement &r: m_ReplacementList)
		{

			SNI_Variable* l_NewVariable = r.ReplaceVariable(p_Variable);
			if (l_NewVariable)
			{
				p_Changed = true;
				l_Result = l_NewVariable;
				break;
			}
		}
		bool found = false;
		for (const SNI_Variable *v : m_VariableList)
		{
			if (v == l_Result)
			{
				found = true;
			}
		}
		if (!found)
		{
			m_VariableList.push_back(p_Variable);
		}
		return l_Result;
	}

	SNI_Expression * SNI_Frame::CloneReplace(bool &p_Changed, SNI_Variable *p_From, SNI_Variable *p_To, SNI_Expression *p_Expression)
	{
		m_ReplacementList.push_back(SNI_Replacement(p_From, p_To));
		SNI_Expression * result = p_Expression->Clone(this, p_Changed);
		m_ReplacementList.pop_back();
		return result;
	}

	string SNI_Frame::GenerateName(SNI_Variable *p_Variable)
	{
		if (m_ThreadNum == 1)
		{	// Main thread
			return to_string(++m_FrameNum) + "." + p_Variable->GetName();
		}
		return to_string(++m_ThreadNum) + "_" + to_string(++m_FrameNum) + "." + p_Variable->GetName();
	}

	string SNI_Frame::GetLogDescription()
	{
		return string();
	}

	void SNI_Frame::PromoteMembers()
	{
	}
}
