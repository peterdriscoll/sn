#include "sni_frame.h"

#include "sni_variable.h"

#include "sn_pch.h"

namespace SNI
{
	long m_MaxThreadNum = 0;
	thread_local long t_ThreadNum = -1;
	thread_local long t_MaxFrameNum = 0;

	/*static*/ SNI_ThreadFrameList SNI_Frame::m_ThreadFrameList;
	
	/*static*/ SNI_FrameList & SNI_Frame::GetFrameList()
	{
		long threadNum = GetThreadNum();
		if (threadNum >= m_ThreadFrameList.size())
		{
			m_ThreadFrameList.resize(threadNum+1);
		}
		return m_ThreadFrameList[threadNum];
	}

	/*static*/ SNI_Frame *SNI_Frame::Push(const SNI_Expression *p_Function, SNI_Expression *p_Result)
	{
		SNI_Frame *newFrame = new SNI_Frame(p_Function);
		if (p_Result)
		{
			newFrame->CreateParameter(0)->SetValue(p_Result);
		}
		SN::SN_Manager::GetTopManager().Lock();
		GetFrameList().push_back(newFrame);
		SN::SN_Manager::GetTopManager().Unlock();
		return newFrame;
	}

	/*static*/ void SNI_Frame::PushFrame(SNI_Frame *p_Frame)
	{
		SN::SN_Manager::GetTopManager().Lock();
		GetFrameList().push_back(p_Frame);
		SN::SN_Manager::GetTopManager().Unlock();
	}

	/*static*/ void SNI_Frame::Pop()
	{
		SN::SN_Manager::GetTopManager().Lock();
		GetFrameList().pop_back();
		SN::SN_Manager::GetTopManager().Unlock();
	}

	/*static*/ SNI_Frame * SNI_Frame::Top()
	{
		if (GetFrameList().size())
		{
			return GetFrameList().back();
		}
		return NULL;
	}

	/*static*/ long SNI_Frame::GetThreadNum()
	{
		if (t_ThreadNum == -1)
		{
			t_ThreadNum = m_MaxThreadNum++;
		}
		return t_ThreadNum;
	}

	/*static*/ long SNI_Frame::GetFrameStackDepth()
	{
		return GetFrameList().size();
	}

	/*static*/ void SNI_Frame::DisplayFrameStack(long p_Depth)
	{
		size_t base = 0;
		if (0 < p_Depth)
		{
			base = GetFrameList().size() - p_Depth;
		}
		for (size_t j = base; j < GetFrameList().size(); j++)
		{
			LOG(WriteFrame(SN::DebugLevel, GetFrameList()[j]));
		}
	}

	/*static*/ void SNI_Frame::DisplayFrameStack(long p_ThreadNum, long p_Depth)
	{
		SNI_FrameList &frameList = m_ThreadFrameList[p_ThreadNum];
		size_t base = 0;
		if (0 < p_Depth)
		{
			base = frameList.size() - p_Depth;
		}
		for (size_t j = frameList.size(); j > base; j--)
		{
			LOG(WriteFrame(SN::DebugLevel, frameList[j]));
		}
	}

	void SNI_Frame::WriteWebStack(ostream &p_Stream, long p_Depth)
	{
		p_Stream << "<table><tr>\n";
		for (size_t k = 0; k < m_ThreadFrameList.size(); k++)
		{
			SNI_FrameList &frameList = m_ThreadFrameList[k];
			p_Stream << "<td><table>\n";
			p_Stream << "<caption>Thread " << k << "</caption>";
			size_t base = 0;
			if (0 < p_Depth)
			{
				base = frameList.size() - p_Depth;
			}
			for (size_t j = frameList.size(); j > base ; j--)
			{
				p_Stream << "<tr><td align = \"center\">\n";
				frameList[j-1]->WriteWebFrame(p_Stream);
				p_Stream << "<tr><td>\n";
			}
			p_Stream << "</table></td>\n";
		}
		p_Stream << "</tr></table>\n";
	}

	/*static*/ void SNI_Frame::DisplayName(const string &p_Name)
	{
		SNI_Variable *v = LookupVariable(p_Name);
		LOG(WriteVariable(SN::DebugLevel, v));
	}

	/*static*/ SNI_Variable *SNI_Frame::LookupVariable(const string &p_Name)
	{
		for (SNI_Frame *f : GetFrameList())
		{
			SNI_Variable *v = f->LookupVariableInFrame(p_Name);
			if (v)
			{
				return v;
			}
		}
		return NULL;
	}

	SNI_Variable *SNI_Frame::LookupVariableInFrame(const string & p_Name)
	{
		for (SNI_Variable *v : m_VariableList)
		{
			if (p_Name == v->FrameName())
			{
				return v;
			}
		}
		return NULL;
	}

	SNI_Frame::SNI_Frame(SN::SN_Expression p_Function)
	    : m_ThreadNum(GetThreadNum())
		, m_FrameNum(++t_MaxFrameNum)
		, m_Function(p_Function)
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
			SN::SN_Manager::GetTopManager().Lock();
			m_VariableList.push_back(l_Result);
			SN::SN_Manager::GetTopManager().Unlock();
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

	SNI_Variable * SNI_Frame::GetVariable(size_t p_VariableNum)
	{
		return m_VariableList[p_VariableNum];
	}

	SNI_Variable * SNI_Frame::GetResult()
	{
		ASSERTM(m_VariableList.size(), "Result is first parameter of frame.");
		return m_VariableList[0];
	}

	string SNI_Frame::NameSuffix()
	{
		if (m_ThreadNum == 0)
		{	// Main thread
			return "_" + to_string(m_FrameNum);
		}
		return "_"+to_string(m_ThreadNum) + "_" + to_string(m_FrameNum);
	}

    string SNI_Frame::GetLogDescription()
	{
		string heading;
		string typeLine;
		vector<string> data;
		size_t minFixedWidth = 0;
		size_t debugFieldWidth = SNI_Manager::GetTopManager()->DebugFieldWidth();
		string delimeter = "";
		bool hasMoreRows = true;
		string filler;
		for (const SNI_Variable *v : m_VariableList)
		{
			size_t fixedWidth = minFixedWidth;
			string name = v->FrameName();
			SN::SN_Expression e = v->GetValue(false);
			string typeText;
			if (v->IsKnownValue())
			{
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
			e.ForEach(
				[&fixedWidth](const SN::SN_Expression &p_Expression, SNI_World *p_World) -> SN::SN_Error
				{
					string valueText;
					if (!p_Expression.IsNull())
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
			if (debugFieldWidth < fixedWidth)
			{
				fixedWidth = debugFieldWidth;
			}
			heading += delimeter + Pad(name, fixedWidth);
			typeLine += delimeter + Pad(typeText, fixedWidth);
			size_t row = 0;
			e.ForEach(
				[&data, &row, &delimeter, &filler, fixedWidth](const SN::SN_Expression &p_Expression, SNI_World *p_World)->SN::SN_Error
				{
					string valueText;
					if (!p_Expression.IsNull())
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
		return GetLogShortDescription() +  "\n" + result;
	}

	void SNI_Frame::WriteWebFrame(ostream &p_Stream)
	{
		p_Stream << "<table border=\"1\">\n";
		p_Stream << "<caption>" << "Frame " << m_FrameNum << " " << m_Function.DisplaySN() << "</caption>\n";
		size_t debugFieldWidth = SNI_Manager::GetTopManager()->DebugFieldWidth();
		p_Stream << "<tr>\n";
		for (const SNI_Variable *v : m_VariableList)
		{
			p_Stream << "<th>" << v->FrameName() << "</th>\n";
		}
		p_Stream << "</tr>\n";
		p_Stream << "<tr>\n";
		for (const SNI_Variable *v : m_VariableList)
		{
			SN::SN_Expression e = v->GetValue(false);
			string typeText;
			if (v->IsKnownValue())
			{
				typeText = e.GetSNI_Expression()->GetTypeName();
			}
			p_Stream << "<td>" << typeText << "</td>\n";
		}
		p_Stream << "</tr>\n";
		p_Stream << "<tr>\n";
		for (const SNI_Variable *v : m_VariableList)
		{
			p_Stream << "<td>\n";
			SN::SN_Expression e = v->GetValue(false);
			string delimeter;
			e.ForEach(
				[&p_Stream, &delimeter, debugFieldWidth](const SN::SN_Expression &p_Expression, SNI_World *p_World)->SN::SN_Error
				{
					string valueText;
					if (!p_Expression.IsNull())
					{
						valueText = p_Expression.DisplaySN() + string(p_World ? "::" + p_World->DisplayShort() : "");
					}
					p_Stream << delimeter << Details(valueText, debugFieldWidth);
					delimeter = "<br>";
					return SN::SN_Error(true);
				}
			);
			p_Stream << "</td>\n";
		}
		p_Stream << "</tr>\n";
		p_Stream << "</table>\n";
	}

	string SNI_Frame::GetLogShortDescription()
	{
		size_t debugTitleWidth = SNI_Manager::GetTopManager()->DebugTitleWidth();
		if (m_ThreadNum == 1)
		{
			return "Frame " + to_string(m_FrameNum) + " " + Pad(m_Function.DisplaySN(), debugTitleWidth);
		}
		return "Frame " + to_string(m_FrameNum) + " thread " + to_string(m_ThreadNum) + " " + Pad(m_Function.DisplaySN(), debugTitleWidth);
	}

	SNI_Variable * SNI_Frame::CreateTemporary()
	{
		SNI_Variable * result = new SNI_Variable();
		SN::SN_Manager::GetTopManager().Lock();
		m_VariableList.push_back(result);
		SN::SN_Manager::GetTopManager().Unlock();
		return result;
	}

	SNI_Variable * SNI_Frame::CreateParameter(size_t p_ParamNum)
	{
		string paramName = "result";
		if (p_ParamNum > 0)
		{
			paramName = "param" + to_string(p_ParamNum);
		}
		return CreateParameterByName(paramName);
	}

	SNI_Variable * SNI_Frame::CreateParameterByName(const string & p_ParamName)
	{
		SNI_Variable * result = new SNI_Variable();
		result->SetName(p_ParamName + NameSuffix());
		SN::SN_Manager::GetTopManager().Lock();
		m_VariableList.push_back(result);
		SN::SN_Manager::GetTopManager().Unlock();
		return result;
	}

	void SNI_Frame::PromoteMembers()
	{
	}
}
