#include "sni_worldset.h"

#include "logcontext.h"

#include "sni_world.h"
#include "utility.h"

#include <unordered_map>

#include "sn_pch.h"

namespace SNI
{
	typedef unordered_map<SNI_WorldSet *, SN::SN_Value> ConflictMap;
	/*static*/ long SNI_WorldSet::m_NextWorldSetNo = 0;

	SNI_WorldSet::SNI_WorldSet()
		: m_Mark(false)
		, m_Complete(false)
		, m_WorldSetNo(++m_NextWorldSetNo)
		, m_NextWorldNo(0)
	{
	}

	SNI_WorldSet::~SNI_WorldSet()
	{

	}

	string SNI_WorldSet::DisplaySN() const
	{
		return "S" + to_string(m_WorldSetNo) + (IsEmpty() ? " empty" : "");
	}

	string SNI_WorldSet::DisplayShort() const
	{
		return "S" + to_string(m_WorldSetNo);
	}

	string SNI_WorldSet::DisplayLong() const
	{
		return "S" + to_string(m_WorldSetNo) + " " + DisplayWorlds();
	}

	string SNI_WorldSet::DisplayWorlds() const
	{
		string result;
		for (size_t j = 0; j < m_WorldList.size(); j++)
		{
			if (result.length())
			{
				result += ", ";
			}
			result += m_WorldList[j]->DisplaySN();
		}
		return "{" + result + "}";
	}

	string SNI_WorldSet::LogHeading(SN::LogContext &context, long p_Width) const
	{
		string heading;
		for (size_t j = 0; j < m_ChildSetList.size(); j++)
		{
			heading += context.PadWidth(m_ChildSetList[j]->DisplaySN(), p_Width, '.') + m_ChildSetList[j]->LogHeading(context, p_Width);
		}
		return heading;
	}

	string SNI_WorldSet::LogUnderline(SN::LogContext &context, long p_Width) const
	{
		string underline;
		for (size_t j = 0; j < m_ChildSetList.size(); j++)
		{
			underline += context.PadWidth("", p_Width, '-') + m_ChildSetList[j]->LogUnderline(context, p_Width);
		}
		return underline;
	}

	void SNI_WorldSet::LogSN() const
	{
		SN::LogContext context("Worldset " + DisplaySN());
		long width = 10;
		context.LogText("World Set", LogHeading(context, width));
		context.LogText("", LogUnderline(context, width));
		for (size_t j = 0; j < m_WorldList.size(); j++)
		{
			context.LogText("", m_WorldList[j]->LogText(context, width));
		}
	}

	long SNI_WorldSet::NextWorldNo()
	{
		return ++m_NextWorldNo;
	}

	void SNI_WorldSet::AddChildWorldSet(SNI_WorldSet * p_WorldSet)
	{
		for (size_t j = 0; j < m_ChildSetList.size(); j++)
		{
			if (m_ChildSetList[j] == p_WorldSet)
			{
				return;
			}
		}
		m_ChildSetList.push_back(p_WorldSet);
	}

	void SNI_WorldSet::PromoteMembers()
	{
	}

	SNI_World * SNI_WorldSet::JoinWorldsArgs(AddWorldType p_AddWorld, CreateWorldType p_CreateWorld, bool &exists, SNI_World *p_World1, SNI_World *p_World2, SNI_World *p_World3, SNI_World *p_World4)
	{
		SNI_World *p_WorldList[] = { p_World1, p_World2, p_World3, p_World4 };
		return SNI_WorldSet::JoinWorldsArray(p_AddWorld, p_CreateWorld, exists, 4, p_WorldList, NULL);
	}

	SNI_World * SNI_WorldSet::JoinWorldsArray(AddWorldType p_AddWorld, CreateWorldType p_CreateWorld, bool &exists, long p_NumWorlds, SNI_World *p_WorldList[], SNI_World * p_ExtraWorld)
	{
		bool active = (p_CreateWorld == AlwaysCreateWorld);
		SNI_World *contextWorld = SNI_World::ContextWorld();
		if (p_WorldList)
		{
			for (long i = 0; i < p_NumWorlds; i++)
			{
				active |= p_WorldList[i] && p_WorldList[i]->IsAnyActive();
			}
		}
		active |= p_ExtraWorld && p_ExtraWorld->IsAnyActive();
		active |= contextWorld && contextWorld->IsAnyActive();

		if (active)
		{
			SNI_World * world = new SNI_World(this);
			if (p_WorldList)
			{
				for (long j = 0; j < p_NumWorlds; j++)
				{
					if (p_WorldList[j] && p_WorldList[j]->IsAnyActive())
					{
						if (!world->AddChildWorld(p_WorldList[j]))
						{
							exists = false;
							return NULL; // There is conflict between the child worlds, so the world does not exist.
						}
						AddChildWorldSet(p_WorldList[j]->GetWorldSet());
					}
				}
			}
			if (p_ExtraWorld && p_ExtraWorld->IsAnyActive())
			{
				if (!world->AddChildWorld(p_ExtraWorld))
				{
					exists = false;
					return NULL; // There is conflict between the child worlds, so the world does not exist.
				}
				AddChildWorldSet(p_ExtraWorld->GetWorldSet());
			}
			if (contextWorld && contextWorld->IsAnyActive())
			{
				if (!world->AddChildWorld(contextWorld))
				{
					exists = false;
					return NULL; // There is conflict between the child worlds, so the world does not exist.
				}
				AddChildWorldSet(contextWorld->GetWorldSet());
			}
			if (p_AddWorld == AutoAddWorld)
			{
				m_WorldList.push_back(world);
			}
			exists = true;
			return world; // The world exists and has child worlds.
		}
		exists = true;
		return NULL; // Create the world later.
	}

	void SNI_WorldSet::AddToSetList(SNI_World *p_world)
	{
		m_WorldList.push_back(p_world);
	}

	SNI_World * SNI_WorldSet::CreateWorld()
	{
		SNI_World * world = new SNI_World(this);
		world->Activate();
		m_WorldList.push_back(world);
		return world;
	}

	SNI_WorldSet * SNI_WorldSet::Clone()
	{
		return new SNI_WorldSet();
	}

	SNI_World * SNI_WorldSet::CreateCloneWorld(SNI_World * p_Parentworld)
	{
		if (p_Parentworld)
		{
			SNI_World * world = new SNI_World(this, p_Parentworld);
			m_WorldList.push_back(world);
			return world;
		}
		return NULL;
	}

	bool SNI_WorldSet::IsEmpty() const
	{
		return m_WorldList.size() == 0;
	}

	void SNI_WorldSet::Mark(bool p_Mark)
	{
		m_Mark = p_Mark;
	}

	bool SNI_WorldSet::HasMutualExclusion()
	{
		return m_Mark;
	}

	void SNI_WorldSet::RemoveWorld(SNI_World *world)
	{
		for (size_t j = 0; j < m_WorldList.size(); j++)
		{
			if (m_WorldList[j] == world)
			{
				m_WorldList.erase(m_WorldList.begin() + j);
				return;
			}
		}
	}

	void SNI_WorldSet::CheckDependentWorlds()
	{
		SN::LogContext context("SNI_WorldSet::CheckDependentWorlds(" + DisplayShort() + ")");
		LogSN();
		MarkAllWorldInChildSets(false);
		MarkChildWorlds(true);
		FailUnmarkedWorldsInChildSets(true);
	}

	void SNI_WorldSet::MarkAllWorldInChildSets(bool p_Mark)
	{
		MarkWorlds(p_Mark);
		for (SNI_WorldSetList::iterator it = m_ChildSetList.begin(); it != m_ChildSetList.end(); it++)
		{
			(*it)->MarkAllWorldInChildSets(p_Mark);
		}
	}

	void SNI_WorldSet::FailUnmarkedWorldsInChildSets(bool p_Mark)
	{
		FailUnmarkedWorlds(p_Mark);
		for (SNI_WorldSetList::iterator it = m_ChildSetList.begin(); it != m_ChildSetList.end(); it++)
		{
			(*it)->FailUnmarkedWorldsInChildSets(p_Mark);
		}
	}

	void SNI_WorldSet::MarkWorlds(bool p_Mark)
	{
		for (SNI_WorldList::iterator it = m_WorldList.begin(); it != m_WorldList.end(); it++)
		{
			(*it)->MarkWorld(p_Mark);
		}
	}

	void SNI_WorldSet::FailUnmarkedWorlds(bool p_Mark)
	{
		for (SNI_WorldList::iterator it = m_WorldList.begin(); it != m_WorldList.end();)
		{
			SNI_World *world = (*it);
			if (!world->HasMark(p_Mark))
			{
				world->MarkEmpty();
				it = m_WorldList.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void SNI_WorldSet::MarkChildWorlds(bool p_Mark)
	{
		for (SNI_WorldList::iterator it = m_WorldList.begin(); it != m_WorldList.end(); it++)
		{
			(*it)->MarkChildWorlds(p_Mark);
		}
	}

	SNI_WorldSet * SNI_WorldSet::OneParent()
	{
		if (m_ChildSetList.size() == 1)
		{
			SNI_WorldSet *parent = m_ChildSetList[0]->OneParent();
			if (parent)
			{
				return parent;
			}
			return m_ChildSetList[0];
		}
		return NULL;
	}
	bool SNI_WorldSet::IsComplete()
	{
		if (!m_Complete)
		{
			return false;
		}
		for (SNI_WorldSet *worldSet : m_ChildSetList)
		{
			if (!worldSet->IsComplete())
			{
				return false;
			}
		}
		return true;
	}
	void SNI_WorldSet::Complete()
	{
		m_Complete = true;
	}
}
