#include "sni_worldset.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sni_world.h"
#include "utility.h"

#include <unordered_map>

#include "sn_pch.h"

namespace SNI
{
	std::size_t WorldSetKeyHasher::operator()(SNI_WorldSet * k) const
	{
		return hash<long>()(k->GetWorldSetNo());
	}

	bool WorldSetKeyEqual::operator()(SNI_WorldSet* p_Left, SNI_WorldSet* p_Right) const
	{
		return p_Left->Equivalent(p_Right);
	}

	typedef std::unordered_map<SNI_WorldSet *, SN::SN_Value, WorldSetKeyHasher, WorldSetKeyEqual> ConflictMap;
	/*static*/ long SNI_WorldSet::m_NextWorldSetNo = 0;
	/*static*/ SNI_WorldSetList SNI_WorldSet::m_ChangedList;

	/*static*/ void SNI_WorldSet::WriteChangedJS(std::ostream &p_Stream, const std::string &tabs, SNI_DisplayOptions &p_DisplayOptions)
	{
		p_Stream << "\"worldsets\":[\n";
		std::string delimeter = " ";
		for (SNI_WorldSet *ws: m_ChangedList)
		{
			ws->WriteJSON(p_Stream, tabs+"\t", p_DisplayOptions);
		}
		p_Stream << "],\n";
	}

	SNI_WorldSet::SNI_WorldSet()
		: m_Mark(false)
		, m_Complete(false)
		, m_WorldSetNo(++m_NextWorldSetNo)
		, m_NextWorldNo(0)
		, m_ContextWorld(SNI_Thread::GetThread()->ContextWorld())
	{
	}

	SNI_WorldSet::SNI_WorldSet(const SN::SN_Expression &p_Expression)
		: m_Mark(false)
		, m_Complete(false)
		, m_WorldSetNo(++m_NextWorldSetNo)
		, m_NextWorldNo(0)
		, m_ContextWorld(SNI_Thread::GetThread()->ContextWorld())
	{
		AttachExpression(p_Expression);
	}

	SNI_WorldSet::~SNI_WorldSet()
	{

	}

	long SNI_WorldSet::GetWorldSetNo()
	{
		return m_WorldSetNo;
	}

	std::string SNI_WorldSet::DisplaySN() const
	{
		return "S" + std::to_string(m_WorldSetNo) + (IsEmpty() ? " empty" : "");
	}

	std::string SNI_WorldSet::DisplayShort() const
	{
		return "S" + std::to_string(m_WorldSetNo);
	}

	std::string SNI_WorldSet::DisplayLong() const
	{
		SNI_DisplayOptions displayOptions(doTextOnly);
		return "S" + std::to_string(m_WorldSetNo) + " " + DisplayWorlds(displayOptions);
	}

	std::string SNI_WorldSet::DisplayWorlds(SNI_DisplayOptions & p_DisplayOptions) const
	{
		std::string result;
		for (size_t j = 0; j < m_WorldList.size(); j++)
		{
			if (result.length())
			{
				result += ", ";
			}
			result += m_WorldList[j]->DisplaySN(p_DisplayOptions);
		}
		return "{" + result + "}";
	}

	std::string SNI_WorldSet::DisplayVariable(SNI_DisplayOptions & p_DisplayOptions) const
	{
		std::string result;
		if (m_ChildSetList.size())
		{
			for (SNI_WorldSet *ws: m_ChildSetList)
			{
				if (result.length())
				{
					result += ", ";
				}
				result += ws->DisplayVariable(p_DisplayOptions);
			}
			return "{" + result + "}";
		}
		if (m_Expression.IsVariable())
		{
			return m_Expression.DisplaySN(p_DisplayOptions);
		}
		return "";
	}

	std::string SNI_WorldSet::DisplayCondition(SNI_DisplayOptions & p_DisplayOptions, const SN::SN_Expression & p_Value) const
	{
		if (m_Expression.IsVariable())
		{
			return m_Expression.DisplaySN(p_DisplayOptions) + "=" + p_Value.DisplaySN(p_DisplayOptions);
		}
		return DisplayShort() + "=" + p_Value.DisplaySN(p_DisplayOptions);
	}

	void SNI_WorldSet::WriteJSON(std::ostream &p_Stream, const std::string &tabs, SNI_DisplayOptions & p_DisplayOptions) const
	{
		p_Stream << tabs << "\"id\" : \"" << DisplayShort() << "\",\n";
		p_Stream << tabs << "\"expression\" : \"" << EscapeStringToJSON(m_Expression.DisplaySN()) << "\",\n";
		p_Stream << tabs << "\"worlds\" : [";
		std::string delimeter;
		for (const SNI_World *w : m_WorldList)
		{
			SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
			if (!contextWorld || contextWorld->CompatibleWorld(const_cast<SNI_World *>(w)))
			{
				p_Stream << delimeter << "\n" << tabs << "\t{\n";
				w->WriteJSON(p_Stream, tabs + "\t", p_DisplayOptions);
				p_Stream << tabs << "\t}";
				delimeter = ",";
			}
		}
		p_Stream << "\n" << tabs << "],\n";
		p_Stream << tabs << "\"childsets\" : [";
		delimeter = "";
		for (const SNI_WorldSet *ws : m_ChildSetList)
		{
			p_Stream << delimeter << "\n" << tabs << "\t{\n";
			ws->WriteUnmarkedJS(p_Stream, tabs + "\t\t", p_DisplayOptions);
			p_Stream << tabs << "\t}";
			delimeter = ",";
		}
		p_Stream <<"\n" << tabs << "]\n";
	}
	
	void SNI_WorldSet::WriteUnmarkedJS(std::ostream &p_Stream, const std::string &tabs, SNI_DisplayOptions &p_DisplayOptions) const
	{
		p_Stream << tabs << "\"id\" : \"" << DisplayShort() << "\",\n";
		p_Stream << tabs << "\"expression\" : \"" << EscapeStringToJSON(m_Expression.DisplaySN()) << "\",\n";
		p_Stream << tabs << "\"worlds\" : [";
		std::string delimeter;
		for (const SNI_World *w : m_WorldList)
		{
			SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
			if (!contextWorld || contextWorld->CompatibleWorld(const_cast<SNI_World *>(w)))
			{
				p_Stream << delimeter << "\n" << tabs << "\t{\n";
				w->WriteJSON(p_Stream, tabs + "\t", p_DisplayOptions);
				p_Stream << tabs << "\t}";
				delimeter = ",";
			}
		}
		p_Stream << "\n" << tabs << "]\n";
	}

	std::string SNI_WorldSet::LogHeading(SN::LogContext &context, long p_Width) const
	{
		std::string heading;
#ifdef USE_LOGGING
		for (size_t j = 0; j < m_ChildSetList.size(); j++)
		{
			heading += context.PadWidth(m_ChildSetList[j]->DisplaySN(), p_Width, '.') + m_ChildSetList[j]->LogHeading(context, p_Width);
		}
#endif
		return heading;
	}

	std::string SNI_WorldSet::LogUnderline(SN::LogContext &context, long p_Width) const
	{
		std::string underline;
#ifdef USE_LOGGING
		for (size_t j = 0; j < m_ChildSetList.size(); j++)
		{
			underline += context.PadWidth("", p_Width, '-') + m_ChildSetList[j]->LogUnderline(context, p_Width);
		}
#endif
		return underline;
	}

	void SNI_WorldSet::LogSN() const
	{
#ifdef USE_LOGGING
		LOGGING(SN::LogContext context("Worldset " + DisplaySN()));
		long width = 10;
		LOGGING(context.LogText("World Set", LogHeading(context, width)));
		LOGGING(context.LogText("", LogUnderline(context, width)));
		for (size_t j = 0; j < m_WorldList.size(); j++)
		{
			LOGGING(context.LogText("", m_WorldList[j]->LogText(context, width)));
		}
#endif
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
		if (p_WorldSet->m_ContextWorld == m_ContextWorld)
		{
			p_WorldSet->m_ParentSetList.push_back(this);
		}
	}

	void SNI_WorldSet::PromoteMembers()
	{
		for (SNI_World *& w : m_WorldList)
		{
			REQUESTPROMOTION(w);
		}
		for (SNI_WorldSet *& ws : m_ChildSetList)
		{
			REQUESTPROMOTION(ws);
		}
	}

	size_t SNI_WorldSet::CountWorlds()
	{
		size_t result = 0;
		for (SNI_World *world : m_WorldList)
		{
			if (world->IsEmpty())
			{
				++result;
			}
		}
		return result;
	}

	SNI_World * SNI_WorldSet::JoinWorldsArgs(AddWorldType p_AddWorld, CreateWorldType p_CreateWorld, bool &exists, SNI_World *p_World1, SNI_World *p_World2, SNI_World *p_World3, SNI_World *p_World4)
	{
		SNI_World *p_WorldList[] = { p_World1, p_World2, p_World3, p_World4 };
		return SNI_WorldSet::JoinWorldsArray(p_AddWorld, p_CreateWorld, exists, 4, p_WorldList, NULL);
	}

	SNI_World * SNI_WorldSet::JoinWorldsArray(AddWorldType p_AddWorld, CreateWorldType p_CreateWorld, bool &exists, long p_NumWorlds, SNI_World *p_WorldList[], SNI_World * p_ExtraWorld)
	{
		bool active = (p_CreateWorld == AlwaysCreateWorld);
		SNI_World *contextWorld = NULL;
		if (p_CreateWorld != CreateIfActiveParentsIgnoreContext)
		{
			contextWorld = SNI_Thread::GetThread()->ContextWorld();
		}
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
			for (SNI_World *contextWorldLoop : SNI_Thread::GetThread()->ContextWorldList())
			{
				if (contextWorldLoop && contextWorldLoop->IsAnyActive())
				{
					if (!world->AddChildWorld(contextWorldLoop))
					{
						exists = false;
						return NULL; // There is conflict between the child worlds, so the world does not exist.
					}
					AddChildWorldSet(contextWorldLoop->GetWorldSet());
				}
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

	SNI_World * SNI_WorldSet::CreateWorldForValue(const SN::SN_Expression& p_Value)
	{
		if (!SN::SN_Transaction::InWebServer())
		{
			SNI_World * world = new SNI_World(this);
			world->Activate();
			world->AttachValue(p_Value);
			m_WorldList.push_back(world);
			return world;
		}
		return NULL;
	}

	SNI_WorldSet * SNI_WorldSet::Clone()
	{
		return new SNI_WorldSet(m_Expression);
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

	SN::SN_Error SNI_WorldSet::CheckDependentWorlds()
	{
		ScheduleCheckForFails();
		return SNI_Thread::GetThread()->CheckForFails();
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
	SNI_World * SNI_WorldSet::ContextWorld()
	{
		return m_ContextWorld;
	}

	void SNI_WorldSet::ClearFailMarks()
	{
		for (SNI_World *world : m_WorldList)
		{
			world->ClearFailMark();
		}
	}

	bool SNI_WorldSet::AllContextFailed()
	{
		for (SNI_World *world : m_WorldList)
		{
			if (!world->IsEmpty() && !world->IsFailMarked())
			{
				return false;
			}
		}
		return true;
	}

	void SNI_WorldSet::ScheduleCheckForFails()
	{
		SNI_Thread *thread = SNI_Thread::GetThread();
		thread->Lock();
		SNI_WorldSetList *changedList = SNI_Thread::GetThread()->GetWorldSetChanged();
		changedList->push_back(this);
		thread->Unlock();
	}

	void SNI_WorldSet::AddRelated(SNI_WorldSetMap *p_ProcessMap)
	{
		if (p_ProcessMap)
		{
			(*p_ProcessMap)[m_WorldSetNo] = this;
			for (SNI_WorldSet *ws : m_ChildSetList)
			{
				ws->AddCloselyRelated(p_ProcessMap);
			}
			for (SNI_WorldSet *ws : m_ParentSetList)
			{
				ws->AddCloselyRelated(p_ProcessMap);
			}
			if (m_ContextWorld)
			{
				SNI_WorldSet *ws = m_ContextWorld->GetWorldSet();
				if (ws)
				{
					ws->AddCloselyRelated(p_ProcessMap);
				}
			}
		}
	}

	void SNI_WorldSet::AddCloselyRelated(SNI_WorldSetMap *p_ProcessMap)
	{
		if (p_ProcessMap)
		{
			(*p_ProcessMap)[m_WorldSetNo] = this;
			if (m_ContextWorld)
			{
				SNI_WorldSet *ws = m_ContextWorld->GetWorldSet();
				if (ws)
				{
					ws->AddCloselyRelated(p_ProcessMap);
				}
			}
		}
	}

	void SNI_WorldSet::CheckEmptyChildren()
	{
		for (SNI_World* world: m_WorldList)
		{
			if (!world->IsEmpty() && world->HasEmptyChild())
			{
				world->MarkEmpty(EmptyChild);
			}
		}
	}

	void SNI_WorldSet::CheckMissingInResult()
	{
		for (SNI_WorldSet *ws: m_ChildSetList)
		{
			ws->ClearMarkInWorlds();
		}
		for (SNI_World *w : m_WorldList)
		{
			w->MarkChildWorlds2();
		}
		for (SNI_WorldSet *ws: m_ChildSetList)
		{
			ws->EmptyUnmarkedWorlds(m_ContextWorld);
		}
	}

	void SNI_WorldSet::CheckAllNegated()
	{
		bool first = true;
		// find the intersection of all the negated lists in the world.
		// If a world is negated in every world of the worldset then it has failed.
		SNI_WorldCount negatedMap;
		long count = 0;
		for (SNI_World *w : m_WorldList)
		{
			if (!w->IsEmpty())
			{
				count++;
				w->CountNegatedMap(negatedMap);
			}
		}
		for (const auto &pair : negatedMap)
		{
			if (pair.second == count)
			{
				SNI_World *w = pair.first;
				w->FailInContext(m_ContextWorld, NegatedInAllValues);
			}
		}
	}

	void SNI_WorldSet::BreakPointsForDeletedWorlds()
	{
		for (SNI_World *w : m_WorldList)
		{
			w->BreakPointIfDeleted();
		}
	}

	void SNI_WorldSet::ClearMarkInWorlds()
	{
		for (SNI_World *w: m_WorldList)
		{
			w->MarkWorld(false);
		}
	}

	void SNI_WorldSet::EmptyUnmarkedWorlds(SNI_World *p_ContextWorld)
	{
		for (SNI_World *world : m_WorldList)
		{
			if (!world->IsEmpty())
			{
				if (!world->HasMark(true))
				{
					world->FailInContext(p_ContextWorld, MissingInResult);
				}
				else if (world->HasEmptyChild())
				{
					world->FailInContext(p_ContextWorld, EmptyChild);
				}
			}
		}
	}

	SN::SN_Error SNI_WorldSet::RemoveFailures()
	{
		for (SNI_WorldList::iterator it = m_WorldList.begin(); it != m_WorldList.end();)
		{
			SNI_World *world = (*it);
			world->DeleteEmptyFromNegatedMap();
			if (world->IsEmpty())
			{
				it = m_WorldList.erase(it);
			}
			else
			{
				++it;
			}
		}
		if (m_WorldList.empty())
		{
			return SN::SN_Error(false, false, "SNI_WorldSet: World set is empty.");
		}
		return skynet::OK;
	}

	void SNI_WorldSet::AttachExpression(const SN::SN_Expression & p_Expression)
	{
		if (m_Expression.IsNull())
		{
			m_Expression = p_Expression;
			m_ExpressionString = m_Expression.DisplaySN();
		}
		ASSERTM(m_Expression.IsNull()|| m_Expression.IsVariable(), "Expected variable.");
	}
}
