#include "sni_world.h"

#include "logcontext.h"

#include "sni_worldset.h"

#include "sn_pch.h"

namespace SNI
{
	thread_local SNI_WorldList g_ContextStack;

	/*static*/ SNI_World * SNI_World::ContextWorld()
	{
		if (g_ContextStack.size())
		{
			return g_ContextStack.back();
		}
		return NULL;
	}

	/*static*/ SNI_WorldList & SNI_World::ContextWorldList()
	{
		return g_ContextStack;
	}

	/*static*/ void SNI_World::PushContextWorld(SNI_World * p_Context)
	{
		g_ContextStack.push_back(p_Context);
	}

	/*static*/ void SNI_World::PopContextWorld()
	{
		g_ContextStack.pop_back();
	}

	SNI_World::SNI_World(SNI_WorldSet * p_WorldSet, SNI_World *p_CloneParent)
		: m_Mark(false)
		, m_IsEmpty(false)
		, m_Active(false)
		, m_WorldSet(p_WorldSet)
		, m_CloneParent(p_CloneParent)
	{
		if (m_WorldSet)
		{
			m_WorldNo = m_WorldSet->NextWorldNo();
		}
	}

	SNI_World::~SNI_World()
	{

	}

	string SNI_World::DisplaySN() const
	{
		return m_WorldSet->DisplaySN() + ":W" + to_string(m_WorldNo) + (IsActive() ? "+" : "") + (IsEmpty() ? "<fail>" : "" + DisplaySNChildWorlds());
	}

	string SNI_World::DisplayShort() const
	{
		return m_WorldSet->DisplayShort() + ":W" + to_string(m_WorldNo);
	}

	string SNI_World::DisplaySN(SNI_DisplayOptions & p_DisplayOptions)
	{
		return DisplayCondition();
	}

	string SNI_World::DisplayCondition() const
	{
		string result;
		if (m_ChildList.empty())
		{
			result = m_WorldSet->DisplayVariable() + "=" + m_Value.DisplaySN();
		}
		else
		{
			for (SNI_World *w : m_ChildList)
			{
				if (!result.empty())
				{
					result += "&&";
				}
				result += w->DisplayCondition();
			}
		}
		for (SNI_World *w : m_NegatedList)
		{
			if (!result.empty())
			{
				result += "&&!" + w->DisplayCondition();
			}
			result += w->DisplayCondition();
		}
		return result;
	}

	string SNI_World::DisplaySNChildWorlds() const
	{
		if (m_ChildList.size() == 0)
		{
			return "";
		}
		string result = "{";
		string separator;
		for (SNI_WorldList::const_iterator it = m_ChildList.begin(); it < m_ChildList.end(); it++)
		{
			result += separator + (*it)->DisplaySN();
			separator = ", ";
		}
		result += "}";
		return result;
	}

	string SNI_World::LogText(SN::LogContext &context, long p_Width) const
	{
		string text;
		for (size_t j = 0; j < m_ChildList.size(); j++)
		{
			text += context.PadWidth(DisplayShort(), p_Width, '.') + m_ChildList[j]->LogText(context, p_Width);
		}
		return text;
	}

	void SNI_World::WriteJS(ostream & p_Stream, const string & tabs) const
	{
		p_Stream << tabs << "\t\"id\" : \"" << DisplayShort() << "\",\n";
		p_Stream << tabs << "\t\"expression\" : \"" << ReplaceAll(m_Value.DisplaySN(), "\"", "\\\"") << "\",\n";
		p_Stream << tabs << "\t\"condition\" : \"" << ReplaceAll(DisplayCondition(), "\"", "\\\"") << "\",\n";
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

	bool SNI_World::AddFailedContext(SNI_World * p_World)
	{
		ASSERTM(p_World, "Attempt to add null child world.");
		bool found = false;
		for (size_t j = 0; j<m_FailedContextList.size(); j++)
		{
			if (m_FailedContextList[j] == p_World)
			{
				return false;
			}
		}
		m_FailedContextList.push_back(p_World);
		return true;
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
		p_World->Mark(false);
		Mark(true);
		if (!p_World->HasMutualExclusion())
		{
			return true;
		}
		return false;
	}

	void SNI_World::BuildFailedWorldSets(SNI_WorldSetList & p_FailedWorldSetList)
	{
		for (SNI_World *world : m_FailedContextList)
		{
			SNI_WorldSet *worldSet = world->GetWorldSet();
			bool found = false;
			for (SNI_WorldSet *worldSetLoop : p_FailedWorldSetList)
			{
				if (worldSet == worldSetLoop)
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				p_FailedWorldSetList.push_back(worldSet);
			}
		}
	}

	bool SNI_World::CheckForWorldSetFails()
	{
		SNI_WorldSetList worldSetList;
		BuildFailedWorldSets(worldSetList);
		ClearContextFailMarks(worldSetList);
		MarkContextFailMarks();
		return CheckFailedWorldSets(worldSetList);
	}

	void SNI_World::ClearContextFailMarks(SNI_WorldSetList &p_FailedWorldSetList)
	{
		for (SNI_WorldSet *worldSet : p_FailedWorldSetList)
		{
			worldSet->ClearFailMarks();
		}
	}

	void SNI_World::MarkContextFailMarks()
	{
		for (SNI_World *world : m_FailedContextList)
		{
			world->MarkFail();
		}
	}

	bool SNI_World::CheckFailedWorldSets(SNI_WorldSetList &p_FailedWorldSetList)
	{
		for (SNI_WorldSet *worldSet : p_FailedWorldSetList)
		{
			if (worldSet->AllContextFailed())
			{
				return true;
			}
		}
		return false;
	}

	void SNI_World::ClearFailMark()
	{
		m_FailMark = false;
	}

	void SNI_World::MarkFail()
	{
		m_FailMark = true;
	}

	bool SNI_World::IsFailMarked() const
	{
		return m_FailMark;
	}

	bool SNI_World::Contains(SNI_World *p_World) const
	{
		if (p_World == NULL)
		{
			return true;
		}
		if (p_World == this)
		{
			return true;
		}
		for (size_t j = 0; j<m_ChildList.size(); j++)
		{
			if (m_ChildList[j]->Contains(p_World))
			{
				return true;
			}
		}
		return false;
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

	void SNI_World::MarkChildWorlds(bool p_Mark)
	{
		MarkWorld(p_Mark);
		for (SNI_WorldList::iterator it = m_ChildList.begin(); it < m_ChildList.end(); it++)
		{
			(*it)->MarkChildWorlds(p_Mark);
		}
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

	SNI_World *SNI_World::GetContextWorld() const
	{
		if (!m_WorldSet)
		{
			return NULL;
		}
		return m_WorldSet->ContextWorld();
	}

	bool SNI_World::IsProperSubWorld(SNI_World *p_World) const
	{
		if (this == p_World)
		{
			return false;
		}
		return IsSubWorld(p_World);
	}

	bool SNI_World::IsSubWorld(SNI_World *p_World) const
	{
		if (this == p_World)
		{
			return true;
		}
		if (!p_World)
		{
			return true;
		}
		return IsSubWorld(p_World->GetContextWorld());
	}

	void SNI_World::MarkEmpty(enum FailReason p_Reason)
	{
		LOG(WriteLine(SN::DebugLevel, "Fail " + ReasonString(p_Reason) + " " + DisplayCondition()));
		m_IsEmpty = true;
		m_Reason = p_Reason;
		SNI_Thread::GetThread()->SetDebugId(DisplayShort());
		SNI_Thread::GetThread()->DebugCommand(SN::FailPoint, "Fail", SN::CallId);
		if (m_WorldSet)
		{
			m_WorldSet->ScheduleCheckForFails();
		}
	}

	SN::SN_Error SNI_World::Fail(enum FailReason p_Reason)
	{
		return FailInContext(SNI_World::ContextWorld(), p_Reason);
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
			return m_WorldSet->CheckForFails();
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

	bool SNI_World::FailNoRemove()
	{
		return FailNoRemoveInContext(SNI_World::ContextWorld());
	}

	bool SNI_World::FailNoRemoveInContext(SNI_World *p_ContextWorld)
	{
		if (p_ContextWorld && p_ContextWorld->IsProperSubWorld(m_WorldSet->ContextWorld()))
		{
			AddFailedContext(p_ContextWorld);
			if (!CheckForWorldSetFails())
			{
				return false;
			}
		}

		LOG(WriteLine(SN::DebugLevel, "Fail " + DisplayCondition()));
		m_IsEmpty = true;
		return !m_WorldSet->IsEmpty();
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
			if (negatedMap.find(w) == negatedMap.end())
			{
				negatedMap[w] = 0;
			}
			negatedMap[w]++;
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
}