#if !defined(SNI_WORLDSET_H_INCLUDED)
#define SNI_WORLDSET_H_INCLUDED

#pragma once

#include "sni_object.h"
#include <unordered_map>
using namespace std;

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
	struct WorldSetKeyHasher
	{
		std::size_t operator()(SNI_WorldSet * k) const;
	};

	struct WorldSetKeyEqual
	{
		bool operator()(SNI_WorldSet* p_Left, SNI_WorldSet* p_Right) const;
	};
	
	typedef unordered_map<long, SNI_WorldSet*> SNI_WorldSetMap;

	enum EnumAddWorld
	{
		AutoAddWorld,
		ManualAddWorld
	};
	typedef enum EnumAddWorld AddWorldType;
	
	
	enum EnumCreateWorld
	{
		AlwaysCreateWorld,
		CreateIfActiveParents,
		CreateIfActiveParentsIgnoreContext
	};
	typedef enum EnumCreateWorld CreateWorldType;

	class SNI_WorldSet :public SNI_Object
	{
		PGC_CLASS(SNI_WorldSet);
	public:
		static void WriteChangedJS(ostream &p_Stream, const string &tabs, SNI_DisplayOptions &p_DisplayOptions);

		SNI_WorldSet();
		SNI_WorldSet(const SN::SN_Expression &p_Expression);
		virtual ~SNI_WorldSet();

		long GetWorldSetNo();

		string DisplaySN() const;
		string DisplayShort() const;
		string DisplayLong() const;
		string DisplayWorlds(SNI_DisplayOptions & p_DisplayOptions) const;
		string DisplayVariable(SNI_DisplayOptions & p_DisplayOptions) const;
		string DisplayCondition(SNI_DisplayOptions & p_DisplayOptions, const SN::SN_Expression &p_Value) const;
		void WriteJSON(ostream & p_Stream, const string &tabs, SNI_DisplayOptions & p_DisplayOptions) const;
		void WriteUnmarkedJS(ostream & p_Stream, const string &tabs, SNI_DisplayOptions & p_DisplayOptions) const;
		string LogHeading(SN::LogContext &context, long p_Width) const;
		string LogUnderline(SN::LogContext &context, long p_Width) const;
		void LogSN() const;
		long NextWorldNo();
		size_t CountWorlds();

		SNI_World * JoinWorldsArgs(AddWorldType p_AddWorld, CreateWorldType p_CreateWorld, bool & exists, SNI_World * p_World1, SNI_World * p_World2 = NULL, SNI_World * p_World3 = NULL, SNI_World * p_World4 = NULL);
		SNI_World * JoinWorldsArray(AddWorldType p_AddWorld, CreateWorldType p_CreateWorld, bool & exists, long p_NumWorlds, SNI_World * p_WorldList[], SNI_World * p_ExtraWorld = NULL);
		void AddToSetList(SNI_World * p_world);
		SNI_World * CreateWorld();
		SNI_World * CreateWorldForValue(SN::SN_Expression& p_Value);
		SNI_WorldSet * Clone();
		SNI_World * CreateCloneWorld(SNI_World * p_Parentworld);
		bool IsEmpty() const;

		void Mark(bool p_Mark);
		bool HasMutualExclusion();
		SN::SN_Error CheckDependentWorlds();
		SNI_WorldSet * OneParent();
		bool IsComplete();
		void Complete();
		SNI_World *ContextWorld();
		void AttachExpression(const SN::SN_Expression & p_Value);

		bool AllContextFailed();

		void ScheduleCheckForFails();

		void AddRelated(SNI_WorldSetMap * p_ProcessMap);
		void CheckEmptyChildren();
		void CheckMissingInResult();
		void CheckAllNegated();
		void BreakPointsForDeletedWorlds();
		SN::SN_Error RemoveFailures();
		void ClearFailMarks();
	private:
		void EmptyUnmarkedWorlds(SNI_World *p_ContextWorld);

		void AddCloselyRelated(SNI_WorldSetMap * p_ProcessMap);

	protected:
		void ClearMarkInWorlds();

		void AddChildWorldSet(SNI_WorldSet *p_WorldSet);
		virtual void PromoteMembers();

	private:
		static long m_NextWorldSetNo;
		static SNI_WorldSetList m_ChangedList;

		SNI_WorldList m_WorldList;
		SNI_WorldSetList m_ChildSetList;
		SNI_WorldSetList m_ParentSetList;
		SNI_World *m_ContextWorld;

		bool m_Mark;
		bool m_Complete;

		long m_WorldSetNo;
		long m_NextWorldNo;
		SN::SN_Expression m_Expression;
		string m_ExpressionString;
	};
}

#endif // !defined(SNI_WORLDSET_H_INCLUDED)
