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

	/*static*/ SNI_Frame * SNI_Frame::Top()
	{
		if (m_FrameStack.size())
		{
			return m_FrameStack.back();
		}
		return NULL;
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
		string heading;
		string typeLine;
		vector<string> data;
		size_t minFixedWidth = 0;
		string delimeter = "";
		bool hasMoreRows = true;
		string filler;
		for (const SNI_Variable *v : m_VariableList)
		{
			size_t fixedWidth = minFixedWidth;
			string name = v->GetName();
			SN::SN_Expression e = v->GetValue(false);
			string value;
			string typeText;
			if (v->IsKnownValue())
			{
				value = e.DisplaySN();
				typeText = e.GetSNI_Expression()->GetTypeName();
			}
			if (fixedWidth < name.size())
			{
				fixedWidth = name.size();
			}
			if (fixedWidth < typeText.size())
			{
				fixedWidth = typeText.size();
			}
			if (e.IsKnownValue())
			{
				e.ForEach(
					[&fixedWidth](const SN::SN_Expression &p_Expression, SNI_World *p_World) -> SN::SN_Error
				{
					string valueText;
					if (p_Expression.IsKnownValue())
					{
						valueText = p_Expression.DisplaySN() + string(p_World ? "::" + p_World->DisplayShort() : "");
					}
					if (fixedWidth < valueText.size())
					{
						fixedWidth = valueText.size();
					}
					return SN::SN_Error(true);
				}
				);
			}
			heading += delimeter + Pad(name, fixedWidth);
			typeLine += delimeter + Pad(typeText, fixedWidth);
			size_t row = 0;
			if (e.IsKnownValue())
			{
				e.ForEach(
					[&data, &row, &delimeter, &filler, fixedWidth](const SN::SN_Expression &p_Expression, SNI_World *p_World)->SN::SN_Error
				{
					string valueText;
					if (p_Expression.IsKnownValue())
					{
						valueText = p_Expression.DisplaySN() + string(p_World ? "::" + p_World->DisplayShort() : "");
					}
					row++;
					if (data.size() < row)
					{
						data.push_back(filler);
					}
					data[row - 1] += delimeter + Pad(valueText, fixedWidth);
					return SN::SN_Error(true);
				}
				);
			}
			string fillerField = delimeter + string(fixedWidth,' ');
			for (size_t j = row; j < data.size(); j++)
			{
				data[j] += fillerField;
			}
			filler += fillerField;
			delimeter = " | ";
		}
		string result = heading + " |";
		if (!data.empty())
		{
			result += "\n" + typeLine + " |";
			for (string &line : data)
			{
				result += "\n" + line + " |";
			}
		}
		return result;
	}

	SNI_Variable * SNI_Frame::CreateTemporary()
	{
		SNI_Variable * result = new SNI_Variable();
		m_VariableList.push_back(result);
		return result;
	}

	void SNI_Frame::PromoteMembers()
	{
	}
}
