#include "sni_frame.h"

#include "sni_variable.h"

#include "sn_pch.h"

namespace SNI
{
	long m_MaxThreadNum = 0;
	thread_local long t_MaxFrameNum = 0;

	/*static*/ size_t SNI_Frame::StackMemory()
	{
		size_t stackTop = (size_t) &stackTop;
		static size_t stackBottom = stackTop;
		return stackBottom - stackTop;
	}

	/*static*/ SNI_Frame *SNI_Frame::Push(const SNI_Expression *p_Function, SNI_Expression *p_Result)
	{
		SNI_Frame *newFrame = new SNI_Frame(p_Function);
		newFrame->CreateParameter(0, p_Result);
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

	/*static*/ size_t SNI_Frame::GetFrameStackDepth()
	{
		return SNI_Thread::GetThread()->GetFrameStackDepth();
	}

	/*static*/ void SNI::SNI_Frame::DisplayFrameStack(size_t p_Depth)
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
		, m_StepCount(SNI_Thread::GetThread()->GetStepCount()+1)
		, m_BreakPointJS(MakeBreakPointJS("", 0))
		, m_Cardinality(std::size_t{ 0 })
		, m_StackUsage(StackMemory())
		, m_ContextWorld(SNI_Thread::GetThread()->ContextWorld())
		, m_ResultIndex(std::size_t{ 0 })
	{
	}

	SNI_Frame::SNI_Frame(SN::SN_Expression p_Function)
	    : m_ThreadNum(SNI_Thread::GetThread()->GetThreadNum())
		, m_FrameNum(++t_MaxFrameNum)
		, m_StepCount(SNI_Thread::GetThread()->GetStepCount()+1)
		, m_Function(p_Function)
		, m_BreakPointJS(MakeBreakPointJS("", 0))
		, m_Cardinality(0)
		, m_StackUsage(StackMemory())
		, m_ContextWorld(SNI_Thread::GetThread()->ContextWorld())
		, m_ResultIndex(0)
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
		SNI_Expression * result = p_Expression->Clone(0, this, p_Changed);
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
			string typeText = v->GetValueTypeName();
			if (fixedWidth < name.size())
			{
				fixedWidth = name.size();
			}
			if (fixedWidth < typeText.size())
			{
				fixedWidth = typeText.size();
			}
			if (v->GetSafeValue())
			{
				SN::SN_Expression e = v->GetSafeValue();
				e.ForEach(
					[&fixedWidth](const SN::SN_Expression &p_Expression, SNI_World *p_World) -> SN::SN_Error
				{
					string valueText;
					if (!p_Expression.IsNull())
					{
						SNI_DisplayOptions displayOptions(doTextOnly);
						valueText = p_Expression.DisplaySN() + string(p_World ? "::" + p_World->DisplaySN(displayOptions) : "");
					}
					if (fixedWidth < valueText.size())
					{
						fixedWidth = valueText.size();
					}
					return skynet::OK;
				});
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
						SNI_DisplayOptions displayOptions(doTextOnly);
						valueText = p_Expression.DisplaySN() + string(p_World ? "::" + p_World->DisplaySN(displayOptions) : "");
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
				string fillerField = delimeter + string(fixedWidth, ' ');
				for (size_t j = row; j < data.size(); j++)
				{
					data[j] += fillerField;
				}
				filler += fillerField;
				delimeter = " | ";
			}
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

	string SNI_Frame::GetBreakPoint()
	{
		return m_BreakPoint;
	}

	string SNI_Frame::GetBreakPointJS()
	{
		return m_BreakPointJS;
	}

	void SNI_Frame::SetBreakPoint(const string & p_BreakPoint, const string & p_BreakPointJS)
	{
		m_BreakPoint = p_BreakPoint;
		m_BreakPointJS = p_BreakPointJS;
	}

	bool SNI_Frame::HasCode() const
	{
		return m_Function.IsVariable();
	}

	void SNI_Frame::SetCloneFunction(const SNI_Expression * p_Function)
	{
		if (HasCode())
		{
			m_CloneFunction = p_Function;
		}
	}

	void SNI_Frame::WriteWebFrame(ostream &p_Stream, size_t p_FrameStackPos, size_t p_DebugFieldWidth, SNI::SNI_DisplayOptions &p_DisplayOptions)
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
			string typeText = v->GetValueTypeName();
			p_Stream << "<td>" << typeText << "</td>\n";
		}
		p_Stream << "</tr>\n";
		p_Stream << "<tr>\n";
		for (const SNI_Variable *v : m_VariableList)
		{
			p_Stream << "<td><div class='frame'>\n";
			SN::SN_Expression e = v->GetSafeValue();
			string delimeter;
            e.ForEach(
				[&p_Stream, &delimeter, p_DebugFieldWidth, &p_DisplayOptions](const SN::SN_Expression &p_Expression, SNI_World *p_World)->SN::SN_Error
				{
					string valueText;
					if (!p_Expression.IsNull() || p_Expression.IsKnownTypeValue())
					{
						valueText = p_Expression.DisplaySN(p_DisplayOptions) + string(p_World ? "::" + p_World->DisplaySN(p_DisplayOptions) : "");
					}
					p_Stream << delimeter << Details(valueText, p_DebugFieldWidth);
					delimeter = "<br/>";
					return skynet::OK;
				}
			);
			p_Stream << "</div></td>\n";
		}
		p_Stream << "</tr></table></div>\n";
	}

	void SNI_Frame::WriteJSON(ostream &p_Stream, size_t p_FrameStackPos, size_t p_DebugFieldWidth, SNI::SNI_DisplayOptions &p_DisplayOptions)
	{
		string function;
		if (HasCode())
		{
			if (m_Function.IsVariable())
			{
				function = m_Function.GetSNI_Variable()->FrameName();
			}
			else
			{
				function = m_CloneFunction.DisplaySN();
			}
		}
		else
		{
			function = m_Function.DisplaySN();
		}
		p_Stream << "\t\t\"function\" : \"" << EscapeStringToJSON(function) << "\",\n";
		string card_string = "&infin;";
		if (m_Cardinality == 0)
		{
			card_string = "";
		}
		else if (m_Cardinality < CARDINALITY_MAX)
		{
			card_string = to_string(m_Cardinality);
		}
		p_Stream << "\t\t\"functioncardinality\" : \"" << card_string << "\",\n";
		p_Stream << "\t\t\"framepos\" : \"" << p_FrameStackPos << "\",\n";
		p_Stream << "\t\t\"framenum\" : \"" << m_FrameNum << "\",\n";
		p_Stream << "\t\t\"stepcount\" : \"" << m_StepCount << "\",\n";
		p_Stream << "\t\t\"typename\" : \"" << m_Function.GetValueTypeName() << "\",\n";
		p_Stream << "\t\t\"breakpoint\" : " << m_BreakPointJS << ",\n";
		p_Stream << "\t\t\"stackusage\" : " << to_string(m_StackUsage) << ",\n";
		string context;
		if (m_ContextWorld)
		{
			context = m_ContextWorld->DisplaySN(p_DisplayOptions);
		}
		p_Stream << "\t\t\"contextworld\" : \"" << EscapeStringToJSON(context) << "\",\n";
		if (HasCode())
		{
			p_Stream << "\t\t\"hascode\" : true,\n";
		}
		else
		{
			p_Stream << "\t\t\"hascode\" : false,\n";
		}
		if (HasCode())
		{
			WriteVariable(p_Stream, m_Function, m_CloneFunction, -1, "\t\t", p_DebugFieldWidth, p_DisplayOptions);
		}
		else
		{
			SNI_Variable *var = m_Function.GetSNI_Variable();
			SN::SN_Expression val;
			if (var)
			{
				val = var->GetSafeValue();
			}
			WriteVariable(p_Stream, m_Function, val, -1, "\t\t", p_DebugFieldWidth, p_DisplayOptions);
		}
		p_Stream << ",\n";
		p_Stream << "\t\t\"variables\" : [\n";
		string delimeter;
		long index = 0;
		for (const SNI_Variable *v : m_VariableList)
		{
			if (v)
			{
				SN::SN_Expression var(v);
				SN::SN_Expression val = v->GetSafeValue();
				p_DisplayOptions.SetVarName(v->FrameName());
				p_Stream << delimeter << "\t\t\t{\n";
				WriteVariable(p_Stream, var, val, index++, "\t\t\t\t", p_DebugFieldWidth, p_DisplayOptions);
				p_Stream << "\t\t\t}";
				delimeter = ",\n";
			}
		}
		p_Stream << "\n\t\t]";

		if (m_ResultIndex < m_VariableList.size())
		{
			SN::SN_Expression result_var(m_VariableList[m_ResultIndex]);
			SN::SN_Expression result_val = m_VariableList[m_ResultIndex]->GetSafeValue();
			p_Stream << ",\n\t\t\"result\" : {\n";
			WriteVariable(p_Stream, result_var, result_val, index++, "\t\t\t", p_DebugFieldWidth, p_DisplayOptions);
			p_Stream << "\t\t}";
		}

		p_Stream << "\n\t}";
	}

	void SNI_Frame::WriteCallJS(boost::property_tree::ptree &callStackNode, size_t p_FrameStackPos, SNI::SNI_DisplayOptions & p_DisplayOptions)
	{
		callStackNode.put("name", m_Function.GetSNI_Variable()->FramePathName());

		if (m_CloneFunction.GetSNI_Expression())
		{
			boost::property_tree::ptree functionNode;
			boost::property_tree::ptree valueNode;

			m_CloneFunction.GetSafeValue().ForEach(
				[&valueNode, &p_DisplayOptions](const SN::SN_Expression &p_Expression, SNI_World *p_World)->SN::SN_Error
			{
				if (p_Expression.GetSNI_Expression())
				{
					string valueTextHTML = p_Expression.DisplaySN(p_DisplayOptions) + string(p_World ? "::" + p_World->DisplaySN(p_DisplayOptions) : "");
						
					boost::property_tree::ptree worldNode;
					worldNode.put("text", valueTextHTML);
					valueNode.push_back(std::make_pair("", worldNode));
				}
				return skynet::OK;
			});

			functionNode.push_back(make_pair("value", valueNode));
			callStackNode.push_back(make_pair("function", functionNode));
		}

		SNI_Variable *result = m_VariableList[m_ResultIndex];
		if (result)
		{
			boost::property_tree::ptree resultNode;

			SN::SN_Expression value = result;
			boost::property_tree::ptree valueNode;

			value.GetSafeValue().ForEach(
				[&valueNode, &p_DisplayOptions](const SN::SN_Expression &p_Expression, SNI_World *p_World)->SN::SN_Error
			{
				if (p_Expression.GetSNI_Expression())
				{
					string valueTextHTML = p_Expression.DisplaySN(p_DisplayOptions) + string(p_World ? "::" + p_World->DisplaySN(p_DisplayOptions) : "");

					boost::property_tree::ptree worldNode;
					worldNode.put("text", valueTextHTML);
					valueNode.push_back(std::make_pair("", worldNode));
				}
				return skynet::OK;
			});

			resultNode.push_back(make_pair("value", valueNode));
			callStackNode.push_back(make_pair("result", resultNode));
		}

		callStackNode.put("breakpoint", m_BreakPointJS);
	}
	
	void SNI_Frame::WriteStackJS(ostream &p_Stream, string &p_Delimeter, size_t p_DebugFieldWidth, SNI::SNI_DisplayOptions &p_DisplayOptions)
	{
		if (m_Function.IsVariable())
		{
			p_Stream << p_Delimeter << "\t{\n";
			WriteVariable(p_Stream, m_Function, m_CloneFunction, -1, "\t", p_DebugFieldWidth, p_DisplayOptions);
			p_Stream << "\t}\n";
			p_Delimeter = ",";
		}
	}

	void SNI_Frame::WriteVariable(ostream &p_Stream, SN::SN_Expression &p_Variable, SN::SN_Expression &p_Value, long j, const string &p_Prefix, size_t p_DebugFieldWidth, SNI::SNI_DisplayOptions &p_DisplayOptions)
	{
		SNI_Variable *v = p_Variable.GetSNI_Variable();
		if (v)
		{
			v->WriteJSON(p_Stream, p_Prefix, p_DebugFieldWidth, p_DisplayOptions);
		}
		else
		{
			p_Stream << p_Prefix << "\"name\" : \"\"";
		}
		const SNI_Expression* value = p_Value.GetSNI_Expression();
		if (value)
		{
			WriteCardinalityJSON(p_Stream, p_Value, j, p_Prefix + "\t");
			value->WriteJSON(p_Stream, p_Prefix + "\t", p_DebugFieldWidth, p_DisplayOptions);
		}
	}

	void SNI_Frame::WriteCardinalityJSON(ostream& p_Stream, SN::SN_Expression& p_Value, long p_ParamNum, const string& p_Prefix) const
	{
		size_t card = p_Value.Cardinality();
		SNI_FunctionDef* functionDef = m_Function.GetSNI_FunctionDef();
		string context_card;
		if (functionDef)
		{
			if (0 <= p_ParamNum && p_ParamNum < functionDef->GetNumParameters())
			{
				size_t cardParam = functionDef->ParamCardinality(p_Value, p_ParamNum);
				if (card != cardParam)
				{
					context_card = "/&infin;";
					if (card < CARDINALITY_MAX)
					{
						context_card = "/" + to_string(card);
					}
				}
			}
		}
		string card_string = "&infin;";
		if (card < CARDINALITY_MAX)
		{
			card_string = to_string(card);
		}
		p_Stream << ",\n" << p_Prefix << "\"cardinality\" : \"" << card_string << context_card << "\",\n";
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

	SNI_Variable * SNI_Frame::CreateSplitVariable()
	{
		SNI_Variable * result = new SNI_Variable();
		result->SetName("_split" + NameSuffix());
		SNI_Thread::GetThread()->Lock();
		m_VariableList.push_back(result);
		SNI_Thread::GetThread()->Unlock();
		return result;
	}

	SNI_Variable * SNI_Frame::CreateVariable(const string &p_Name, const string & p_DomainName, enum skynet::DefineType p_DefineType)
	{
		SNI_Variable * result = new SNI_Variable(p_Name, p_DomainName, p_DefineType);
		SNI_Thread::GetThread()->Lock();
		m_VariableList.push_back(result);
		SNI_Thread::GetThread()->Unlock();
		return result;
	}

	SNI_Variable * SNI_Frame::CreateParameter(size_t p_ParamNum, SN::SN_Expression p_Value)
	{
		string paramName = "result";
		if (p_ParamNum > 0)
		{
			paramName = "param" + to_string(p_ParamNum);
		}
		else
		{
			m_ResultIndex = 0;
		}
		return CreateParameterByName(paramName, p_Value);
	}

	SNI_Variable * SNI_Frame::CreateParameterByName(const string & p_ParamName, SN::SN_Expression p_Value)
	{
		SNI_Variable * result = new SNI_Variable();
		result->SetName(p_ParamName + NameSuffix());
		result->SetValue(p_Value);
		SNI_Thread::GetThread()->Lock();
		m_VariableList.push_back(result);
		SNI_Thread::GetThread()->Unlock();
		return result;
	}

	void SNI_Frame::AttachParameter(SN::SN_Expression p_Param)
	{
		SNI_Thread::GetThread()->Lock();
		m_VariableList.push_back(p_Param.GetSNI_Variable());
		SNI_Thread::GetThread()->Unlock();
	}

	void SNI_Frame::AddVariables(SNI_VariablePointerMap &p_Map)
	{
		SNI_Expression* function = m_Function.GetSNI_Expression();
		if (function)
		{
			function->AddVariables(0, p_Map);
		}
		SNI_Expression* cloneFunction = m_CloneFunction.GetSNI_Expression();
		if (cloneFunction)
		{
			cloneFunction->AddVariables(0, p_Map);
		}
		for (SNI_Variable *v: m_VariableList)
		{
			v->AddVariables(0, p_Map);
		}
	}

	void SNI_Frame::RecordSavePoint(vector<bool> p_SavePoint)
	{
		for (auto v : m_VariableList)
		{
			p_SavePoint.push_back(v->IsKnownValue());
		}
	}

	void SNI_Frame::RestoreSavePoint(vector<bool> p_SavePoint)
	{
		size_t j = 0;
		for (auto v : m_VariableList)
		{
			if (j >= p_SavePoint.size() || !p_SavePoint[j])
			{
				v->SetValue(skynet::Null);
			}
		}
	}

	void SNI_Frame::PromoteMembers()
	{
/*
		REQUESTPROMOTION(m_Function);
		for (SNI_Replacement &r : m_ReplacementList)
		{
			r.PromoteMembersExternal(this);
		}
		for (SNI_Variable *v : m_VariableList)
		{
			REQUESTPROMOTION(v);
		}
*/
	}

	void SNI_Frame::RegisterCardinality(size_t p_Cardinality)
	{
		m_Cardinality = p_Cardinality;
	}

	void SNI_Frame::PromoteExternals(PGC::PGC_Transaction *p_Transaction)
	{
		for (SNI_Variable *(&v) : m_VariableList)
		{
			PGC::PGC_TypeCheck* base = static_cast<PGC::PGC_TypeCheck*>(v);
			PGC::PGC_Promotion::CheckRequestPromotion(&base, base->GetTransaction(), p_Transaction, PGC::PromotionStrategy::Backstabbing);
			v = dynamic_cast<SNI_Variable *>(base);
		}
	}
}
