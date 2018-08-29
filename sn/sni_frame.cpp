#include "sni_frame.h"

#include "sni_variable.h"

#include "sn_pch.h"

namespace SNI
{
	long m_MaxThreadNum = 0;
	thread_local long t_MaxFrameNum = 0;


	/*static*/ SNI_Frame *SNI_Frame::Push(const SNI_Expression *p_Function, SNI_Expression *p_Result)
	{
		SNI_Frame *newFrame = new SNI_Frame(p_Function);
		if (p_Result)
		{
			newFrame->CreateParameter(0)->SetValue(p_Result);
		}
		PushFrame(newFrame);
		return newFrame;
	}

	/*static*/ void SNI_Frame::PushFrame(SNI_Frame *p_Frame)
	{
		SNI_Thread::GetThread()->PushFrame(p_Frame);
	}

	/*static*/ void SNI_Frame::Pop()
	{
		SNI_Thread::GetThread()->PopFrame();
	}

	/*static*/ SNI_Frame * SNI_Frame::Top()
	{
		return SNI_Thread::GetThread()->Top();
	}

	/*static*/ long SNI_Frame::GetFrameStackDepth()
	{
		return SNI_Thread::GetThread()->GetFrameStackDepth();
	}

	/*static*/ void SNI_Frame::DisplayFrameStack(long p_Depth)
	{
		return SNI_Thread::GetThread()->DisplayFrameStack(p_Depth);
	}

	/*static*/ void SNI_Frame::DisplayFrameStack(long p_ThreadNum, long p_Depth)
	{
		return SNI_Thread::GetThreadByNumber(p_ThreadNum)->DisplayFrameStack(p_Depth);
	}

	/*static*/ void SNI_Frame::DisplayName(long p_ThreadNum, const string &p_Name)
	{
		SNI_Variable *v = LookupVariable(p_ThreadNum, p_Name);
		LOG(WriteVariable(SN::DebugLevel, v));
	}

	/*static*/ SNI_Variable *SNI_Frame::LookupVariable(long p_ThreadNum, const string &p_Name)
	{
		return SNI_Thread::GetThreadByNumber(p_ThreadNum)->LookupVariable(p_Name);
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

	SNI_Frame::SNI_Frame()
		: m_ThreadNum(SNI_Thread::GetThread()->GetThreadNum())
		, m_FrameNum(++t_MaxFrameNum)
	{
	}

	SNI_Frame::SNI_Frame(SN::SN_Expression p_Function)
	    : m_ThreadNum(SNI_Thread::GetThread()->GetThreadNum())
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
			SNI_Thread::GetThread()->Lock();
			m_VariableList.push_back(l_Result);
			SNI_Thread::GetThread()->Unlock();
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
		ASSERTM(m_VariableList.size(), "Result is first parameter of.");
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

    string SNI_Frame::GetLogDescription(SNI_Manager *p_Manager)
	{
		string heading;
		string typeLine;
		vector<string> data;
		size_t minFixedWidth = 0;
		size_t debugFieldWidth = p_Manager->DebugFieldWidth();
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
					return skynet::OK;
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
					return skynet::OK;
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
		return GetLogShortDescription(p_Manager) +  "\n" + result;
	}

	void SNI_Frame::WriteWebFrame(ostream &p_Stream, size_t p_FrameStackPos, size_t p_DebugFieldWidth)
	{
		p_Stream << "<div style = 'overflow-x:auto;white-space:nowrap;width:900px'>\n";
		p_Stream << "<table class='frame'>\n";
		p_Stream << "<caption>" << "Frame " << p_FrameStackPos << ":" << m_FrameNum << " " << m_Function.DisplaySN() << "</caption>\n";
		p_Stream << "<tr>\n";
		for (const SNI_Variable *v : m_VariableList)
		{
			p_Stream << "<th>" << v->FrameName() << "</th>\n";
		}
		p_Stream << "</tr>\n";
		p_Stream << "<tr>\n";
		for (const SNI_Variable *v : m_VariableList)
		{
			SN::SN_Expression e = v->GetSafeValue();
			string typeText;
			if (e.GetSNI_Expression())
			{
				typeText = e.GetSNI_Expression()->GetTypeName();
			}
			p_Stream << "<td>" << typeText << "</td>\n";
		}
		p_Stream << "</tr>\n";
		p_Stream << "<tr>\n";
		for (const SNI_Variable *v : m_VariableList)
		{
			p_Stream << "<td><div class='frame'>\n";
			SN::SN_Expression e = v->GetValue(false);
			string delimeter;
            e.ForEach(
				[&p_Stream, &delimeter, p_DebugFieldWidth](const SN::SN_Expression &p_Expression, SNI_World *p_World)->SN::SN_Error
				{
					string valueText;
					if (!p_Expression.IsNull())
					{
						valueText = p_Expression.DisplaySN() + string(p_World ? "::" + p_World->DisplayShort() : "");
					}
					p_Stream << delimeter << Details(valueText, p_DebugFieldWidth);
					delimeter = "<br>";
					return skynet::OK;
				}
			);
			p_Stream << "</div></td>\n";
		}
		p_Stream << "</tr></table></div>\n";
	}

	void SNI_Frame::WriteJS(ostream &p_Stream, size_t p_FrameStackPos, size_t p_DebugFieldWidth)
	{
		p_Stream << "\t{\n";
		p_Stream << "\t\t\"function\" : \"" << m_Function.DisplaySN() << "\",\n";
		p_Stream << "\t\t\"framepos\" : \"" << p_FrameStackPos << "\",\n";
		p_Stream << "\t\t\"framenum\" : \"" << m_FrameNum << "\",\n";		
		p_Stream << "\t\t\"variables\" : [\n";
		string delimeter;
		for (const SNI_Variable *v : m_VariableList)
		{
			p_Stream << "\t\t" << delimeter << "\t{\n";
			p_Stream << "\t\t\t\t\"name\" : \"" << v->FrameName() << "\"";
			SN::SN_Expression e = v->GetSafeValue();
			string typeText;
			if (e.GetSNI_Expression())
			{
				typeText = e.GetSNI_Expression()->GetTypeName();
				p_Stream << ",\n\t\t\t\t\"typetext\" : \"" << typeText << "\",\n";
				p_Stream << "\t\t\t\t\"value\" : [";
				string delimeter;
				e.ForEach(
					[&p_Stream, &delimeter, p_DebugFieldWidth](const SN::SN_Expression &p_Expression, SNI_World *p_World)->SN::SN_Error
				{
					string valueText;
					if (!p_Expression.IsNull())
					{
						valueText = p_Expression.DisplaySN() + string(p_World ? "::" + p_World->DisplayShort() : "");
					}
					p_Stream << delimeter << DetailsFS(valueText, p_DebugFieldWidth);
					delimeter = ",";
					return skynet::OK;
				});
				p_Stream << "]";
			}
			p_Stream << "\n\t\t\t}\n";
			delimeter = ",";
		}
		p_Stream << "\t\t]\n";
		p_Stream << "\t}\n";
	}

	string SNI_Frame::GetLogShortDescription(SNI_Manager *p_Manager)
	{
		size_t debugTitleWidth = p_Manager->DebugTitleWidth();
		if (m_ThreadNum == 1)
		{
			return "Frame " + to_string(m_FrameNum) + " " + Pad(m_Function.DisplaySN(), debugTitleWidth);
		}
		return "Frame " + to_string(m_FrameNum) + " thread " + to_string(m_ThreadNum) + " " + Pad(m_Function.DisplaySN(), debugTitleWidth);
	}

	SNI_Variable * SNI_Frame::CreateTemporary()
	{
		SNI_Variable * result = new SNI_Variable();
		SNI_Thread::GetThread()->Lock();
		m_VariableList.push_back(result);
		SNI_Thread::GetThread()->Unlock();
		return result;
	}

	SNI_Variable * SNI_Frame::CreateVariable(const string &p_Name)
	{
		SNI_Variable * result = new SNI_Variable();
		result->SetName(p_Name);
		SNI_Thread::GetThread()->Lock();
		m_VariableList.push_back(result);
		SNI_Thread::GetThread()->Unlock();
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
		SNI_Thread::GetThread()->Lock();
		m_VariableList.push_back(result);
		SNI_Thread::GetThread()->Unlock();
		return result;
	}

	void SNI_Frame::PromoteMembers()
	{
	}
}
