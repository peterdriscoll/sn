#include "sni_world.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sni_worldset.h"

#include "sn_pch.h"

namespace SNI
{
	std::size_t WorldKeyHasher::operator()(const SNI_World * k) const
	{
		return hash<long long>()(k->GetWorldNo());
	}

	bool WorldKeyEqual::operator()(SNI_World* p_Left, SNI_World* p_Right) const
	{
		return p_Left->Equivalent(p_Right);
	}

	SNI_World::SNI_World(SNI_WorldSet * p_WorldSet, SNI_World *p_CloneParent)
		: m_Mark(false)
		, m_IsEmpty(false)
		, m_Active(false)
		, m_WorldSet(p_WorldSet)
		, m_CloneParent(p_CloneParent)
		, m_Reason(NoReason)
	{
		if (m_WorldSet)
		{
			m_WorldNo = m_WorldSet->NextWorldNo();
		}
	}

	SNI_World::~SNI_World()
	{

	}

	long long SNI_World::GetWorldNo() const
	{
		if (m_WorldSet)
		{
			long long no = m_WorldSet->GetWorldSetNo();
			return no <<32 | m_WorldNo;
		}
		return m_WorldNo;
	}

	string SNI_World::DisplayShort() const
	{
		if (m_WorldSet)
		{
			return m_WorldSet->DisplayShort() + "_W" + to_string(m_WorldNo);
		}
		return "Empty_W" + to_string(m_WorldNo);
	}

	string SNI_World::DisplayShortJS() const
	{
		return MakeBreakPointJS(DisplayShort(), SN::ErrorId);
	}

	string SNI_World::DisplaySN(SNI_DisplayOptions & p_DisplayOptions)
	{
		return DisplayConditionSN(p_DisplayOptions);
	}

	string SNI_World::DisplayCondition(SNI_DisplayOptions & p_DisplayOptions) const
	{
		string result;
		if (m_ChildList.empty())
		{
			result = "(" + m_WorldSet->DisplayCondition(p_DisplayOptions, m_Value) + ")";
		}
		else
		{
			for (SNI_World *w : m_ChildList)
			{
				if (!result.empty())
				{
					result += "&&";
				}
				result += w->DisplayShort();
			}
		}
		string negated;
		for (SNI_World *w : m_NegatedList)
		{
			if (!negated.empty())
			{
				negated += "&&";
			}
			negated += w->DisplayShort();
		}
		if (!negated.empty())
		{
			negated = "&& !(" + negated + ")";
		}
		return DisplayShort() + "=" + result + negated;
	}

	string SNI_World::DisplayConditionSN(SNI_DisplayOptions & p_DisplayOptions) const
	{
		return 	SetBreakPoint(DisplayCondition(p_DisplayOptions), p_DisplayOptions);
	}

	string SNI_World::DisplaySNChildWorlds(SNI_DisplayOptions & p_DisplayOptions) const
	{
		if (m_ChildList.size() == 0)
		{
			return "";
		}
		string result = "{";
		string separator;
		for (SNI_WorldList::const_iterator it = m_ChildList.begin(); it < m_ChildList.end(); it++)
		{
			result += separator + (*it)->DisplaySN(p_DisplayOptions);
			separator = ", ";
		}
		result += "}";
		return result;
	}

	string SNI_World::SetBreakPoint(const string &p_Caption, SNI_DisplayOptions & p_DisplayOptions) const
	{
		switch (p_DisplayOptions.GetDebugHTML())
		{
		case doTextOnly:
			if (p_Caption == "~" || p_Caption == ";" || p_Caption == "end")
			{
				return "";
			}
			return p_Caption;
		case doDebugPointsHTML:
			return p_Caption;
		case doDebugPointsJS:
		{
			string breakPoint = DisplayShortJS();
			return "<button title='" + breakPoint + "' ng-click='setbreakpoint(" + breakPoint + ")' ng-class='breakpointdefaultclass(" + breakPoint + ", \""+ Reason() + "\")'>" + p_Caption + "</button>";
		}
		}
		return "";
	}

	string SNI_World::LogText(SN::LogContext &context, long p_Width) const
	{
		string text;
#ifdef USE_LOGGING
		for (size_t j = 0; j < m_ChildList.size(); j++)
		{
			text += context.PadWidth(DisplayShort(), p_Width, '.') + m_ChildList[j]->LogText(context, p_Width);
		}
#endif
		return text;
	}

	void SNI_World::WriteJS(ostream & p_Stream, const string & tabs, SNI_DisplayOptions &p_DisplayOptions) const
	{
		p_Stream << tabs << "\t\"id\" : \"" << DisplayShort() << "\",\n";
		p_Stream << tabs << "\t\"breakpoint\" : " << DisplayShortJS() << ",\n";
		p_Stream << tabs << "\t\"expression\" : \"" << EscapeStringToJSON(m_Value.DisplaySN(p_DisplayOptions)) << "\",\n";
		p_Stream << tabs << "\t\"condition\" : \"" << EscapeStringToJSON(DisplayCondition(p_DisplayOptions)) << "\",\n";
		p_Stream << tabs << "\t\"empty\" : " << (IsEmpty() ? "true" : "false") << ",\n";
		p_Stream << tabs << "\t\"fail\" : " << (IsFailMarked() ? "true" : "false") << ",\n";
		p_Stream << tabs << "\t\"reason\" : \"" << Reason() << "\"\n";
	}

	void SNI_World::PromoteMembers()
	{
		REQUESTPROMOTION(m_WorldSet);
		for (SNI_World *& w : m_ChildList)
		{
			REQUESTPROMOTION(w);
		}
		REQUESTPROMOTION(m_CloneParent);
	}

	SNI_WorldSet * SNI_World::GetWorldSet()
	{
		return m_WorldSet;
	}

	void SNI_World::AddToSetList()
	{
		m_WorldSet->AddToSetList(this);
	}

	bool SNI_World::AddChildWorld(SNI_World *p_World)
	{
		ASSERTM(p_World, "Attempt to add null child world.");
		p_World->Mark(false);
		for (size_t j = 0; j<m_ChildList.size(); j++)
		{
			m_ChildList[j]->Mark(true);
		}
		if (!p_World->HasMutualExclusion())
		{
			m_ChildList.push_back(p_World);
			return true;
		}
		return false;
	}

	bool SNI_World::CompatibleWorld(SNI_World *p_World)
	{
		if (!p_World)
		{
			return true;
		}

		if (IsNegated(p_World))
		{
			return false;
		}

		p_World->Mark(false);
		Mark(true);
		if (!p_World->HasMutualExclusion())
		{
			return true;
		}
		return false;
	}

	void SNI_World::ClearFailMark()
	{
		m_FailMark = false;
	}

	bool SNI_World::IsFailMarked() const
	{
		return m_FailMark;
	}

	void SNI_World::Mark(bool p_Mark)
	{
		m_Mark = p_Mark;
		if (m_Active)
		{
			m_WorldSet->Mark(p_Mark);
		}

		for (size_t j = 0; j<m_ChildList.size(); j++)
		{
			m_ChildList[j]->Mark(p_Mark);
		}
	}

	bool SNI_World::HasMark(bool p_Mark)
	{
		return m_Mark == p_Mark;
	}

	void SNI_World::MarkWorld(bool p_Mark)
	{
		m_Mark = p_Mark;
	}

	void SNI_World::MarkChildWorlds2()
	{
		for (SNI_World* w: m_ChildList)
		{
			w->MarkWorld(true);
		}
	}

	bool SNI_World::HasMutualExclusion()
	{
		if (m_Active && !m_Mark)
		{
			if (m_WorldSet->HasMutualExclusion())
			{
				return true;
			}
		}

		for (size_t j = 0; j<m_ChildList.size(); j++)
		{
			if (m_ChildList[j]->HasMutualExclusion())
			{
				return true;
			}
		}

		return false;
	}

	bool SNI_World::IsActive() const
	{
		return m_Active;
	}

	bool SNI_World::IsAnyActive() const
	{
		if (m_Active)
		{
			return true;
		}
		for (size_t j = 0; j<m_ChildList.size(); j++)
		{
			if (m_ChildList[j]->IsAnyActive())
			{
				return true;
			}
		}
		return false;
	}

	void SNI_World::Activate()
	{
		m_Active = true;
	}

	bool SNI_World::IsEmpty() const
	{
		if (m_CloneParent && m_CloneParent->IsEmpty())
		{
			return true;
		}
		return m_IsEmpty;
	}

	bool SNI_World::HasEmptyChild() const
	{
		for (SNI_World *world : m_ChildList)
		{
			if (world->IsEmpty())
			{
				return true;
			}
			if (world->HasEmptyChild())
			{
				return true;
			}
		}
		return false;
	}

	void SNI_World::MarkEmpty(enum FailReason p_Reason)
	{
		SNI_DisplayOptions displayOptions(doTextOnly);
		LOG(WriteLine(SN::DebugLevel, "Fail " + ReasonString(p_Reason) + " " + DisplayCondition(displayOptions)));
		m_IsEmpty = true;
		m_Reason = p_Reason;
		if (m_WorldSet)
		{
			m_WorldSet->ScheduleCheckForFails();
		}
	}

	SN::SN_Error SNI_World::Fail(enum FailReason p_Reason)
	{
		return FailInContext(SNI_Thread::GetThread()->ContextWorld(), p_Reason);
	}

	SN::SN_Error SNI_World::FailInContext(SNI_World *p_ContextWorld, enum FailReason p_Reason)
	{
		if (p_ContextWorld)
		{
			p_ContextWorld->Negate(this);
			return skynet::OK;
		}

		MarkEmpty(p_Reason);
		if (m_WorldSet)
		{
			return SNI_Thread::GetThread()->CheckForFails();
		}
		return skynet::OK;
	}

	void SNI_World::Negate(SNI_World * p_World)
	{
		ASSERTM(p_World, "Attempt to add null child world.");
		for (SNI_World *w : m_NegatedList)
		{
			if (w == p_World)
			{
				return;
			}
		}
		m_NegatedList.push_back(p_World);
	}

	bool SNI_World::IsNegated(SNI_World * p_World)
	{
		ASSERTM(p_World, "Attempt to add null child world.");
		for (SNI_World *w : m_NegatedList)
		{
			if (w == p_World)
			{
				return true;
			}
		}
		return false;
	}

	string SNI_World::Reason() const
	{
		return ReasonString(m_Reason);
	}

	/*static*/ string SNI_World::ReasonString(enum FailReason p_Reason)
	{
		switch (p_Reason)
		{
		case IncompatibleValue:
			return "CONFLICT";
		case EmptyChild:
			return "EC";
		case MissingInResult:
			return "MIR";
		case NegatedInAllValues:
			return "NIAV";
		}
		return "NONE";
	}

	void SNI_World::CountNegatedMap(SNI_WorldCount &negatedMap) const
	{
		for (SNI_World *w : m_NegatedList)
		{
			if (!w->IsEmpty())
			{ 
				if (negatedMap.find(w) == negatedMap.end())
				{
					negatedMap[w] = 0;
				}
				negatedMap[w]++;
			}
		}
	}

	void SNI_World::DeleteEmptyFromNegatedMap()
	{
		for (auto it = m_NegatedList.begin(); it != m_NegatedList.end();)
		{
			SNI_World *w = *it;
			if (w->IsEmpty())
			{
				it = m_NegatedList.erase(it);
			}
			else
			{
				it++;
			}
		}
	}

	SNI_World * SNI_World::OneParent(SNI_WorldSet *parentWorldSet)
	{
		if (m_ChildList.size() == 1)
		{
			if (m_ChildList[0]->GetWorldSet() == parentWorldSet)
			{
				return m_ChildList[0];
			}
			return m_ChildList[0]->OneParent(parentWorldSet);
		}
		return NULL;
	}

	void SNI_World::AttachValue(const SN::SN_Expression & p_Value)
	{
		m_Value = p_Value;
		m_ValueString = m_Value.DisplayValueSN();
	}

	void SNI_World::BreakPointIfDeleted()
	{
		if (IsEmpty())
		{
			SNI_Thread::GetThread()->SetDebugId(DisplayShort());
			SNI_Thread::GetThread()->DebugCommand(SN::FailPoint, "Deleted world", SN::CallId);
		}
	}
}