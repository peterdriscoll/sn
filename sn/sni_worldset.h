#if !defined(SNI_WORLDSET_H_INCLUDED)
#define SNI_WORLDSET_H_INCLUDED

#pragma once

#include "sni_object.h"

namespace SN
{
	class LogContext;
}

namespace SNI
{
	class SNI_World;
	typedef vector<SNI_World*> SNI_WorldList;

	class SNI_WorldSet;
	typedef vector<SNI_WorldSet*> SNI_WorldSetList;

	enum EnumAddWorld
	{
		AutoAddWorld,
		ManualAddWorld
	};
	typedef enum EnumAddWorld AddWorldType;
	
	enum EnumCreateWorld
	{
		AlwaysCreateWorld,
		CreateIfActiveParents
	};
	typedef enum EnumCreateWorld CreateWorldType;

	class SNI_WorldSet :public SNI_Object
	{
		PGC_CLASS(SNI_WorldSet);
	public:
		SNI_WorldSet();
		virtual ~SNI_WorldSet();

		string DisplaySN() const;
		string DisplayShort() const;
		string DisplayLong() const;
		string DisplayWorlds() const;
		string LogHeading(SN::LogContext &context, long p_Width) const;
		string LogUnderline(SN::LogContext &context, long p_Width) const;
		void LogSN() const;
		long NextWorldNo();

		SNI_World * JoinWorldsArgs(AddWorldType p_AddWorld, CreateWorldType p_CreateWorld, bool & exists, SNI_World * p_World1, SNI_World * p_World2 = NULL, SNI_World * p_World3 = NULL, SNI_World * p_World4 = NULL);
		SNI_World * JoinWorldsArray(AddWorldType p_AddWorld, CreateWorldType p_CreateWorld, bool & exists, long p_NumWorlds, SNI_World * p_WorldList[], SNI_World * p_ExtraWorld = NULL);
		void AddToSetList(SNI_World * p_world);
		SNI_World * CreateWorld();
		SNI_WorldSet * Clone();
		SNI_World * CreateCloneWorld(SNI_World * p_Parentworld);
		bool IsEmpty() const;

		void Mark(bool p_Mark);
		bool HasMutualExclusion();
		void RemoveWorld(SNI_World *world);
		void CheckDependentWorlds();
		SNI_WorldSet * OneParent();
		bool IsComplete();
		void Complete();
	protected:
		void AddChildWorldSet(SNI_WorldSet *p_WorldSet);
		virtual void PromoteMembers();

	private:
		void CheckRelatedWorlds(SNI_WorldSetList &p_ChangedList);
		void CheckDependencies(SNI_WorldSetList & p_ChangedList);
		void MarkAllWorldInChildSets(bool p_Mark);
		void FailUnmarkedWorldsInChildSets(bool p_Mark, SNI_WorldSetList &p_ChangedList);
		void MarkWorlds(bool p_Mark);
		void FailUnmarkedWorlds(bool p_Mark, SNI_WorldSetList &p_ChangedList);
		void FailWorldsWithEmptyChildren(SNI_WorldSetList & p_ChangedList);
		void MarkChildWorlds(bool p_Mark);

		static long m_NextWorldSetNo;

		SNI_WorldList m_WorldList;
		SNI_WorldSetList m_ChildSetList;
		SNI_WorldSetList m_ParentSetList;

		bool m_Mark;
		bool m_Complete;

		long m_WorldSetNo;
		long m_NextWorldNo;
	};
}

#endif // !defined(SNI_WORLDSET_H_INCLUDED)
