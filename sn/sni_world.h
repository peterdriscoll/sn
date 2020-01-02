#if !defined(SNI_WORLD_H_INCLUDED)
#define SNI_WORLD_H_INCLUDED

#pragma once

#include "sni_object.h"

#include <vector>
#include <unordered_map>
using namespace std;

namespace SN
{
	class LogContext;
}

namespace SNI
{
	enum FailReason
	{
		IncompatibleValue,
		EmptyChild,
		MissingInResult,
		NegatedInAllValues
	};

	class SNI_WorldSet;
	typedef vector<SNI_WorldSet*> SNI_WorldSetList;

	class SNI_World;
	typedef vector<SNI_World*> SNI_WorldList;
	typedef unordered_map<long, SNI_World*> SNI_WorldMap;
	typedef unordered_map<SNI_World*, long> SNI_WorldCount;

	class SNI_World :public SNI_Object
	{
	private:
		PGC_CLASS(SNI_World);
	public:
		static SNI_World *ContextWorld();
		static SNI_WorldList & ContextWorldList();
		static void PushContextWorld(SNI_World *p_Context);
		static void PopContextWorld();
		static string ReasonString(enum FailReason p_Reason);

		SNI_World(SNI_WorldSet * p_WorldSet, SNI_World *p_CloneParent = NULL);
		virtual ~SNI_World();

		string DisplaySN() const;
		string DisplayShort() const;
		string DisplaySN(SNI_DisplayOptions & p_DisplayOptions);
		string DisplayCondition() const;
		string DisplayConditionSN(SNI_DisplayOptions & p_DisplayOptions) const;
		string DisplaySNChildWorlds() const;
		string SetBreakPoint(const string & p_Caption, SNI_DisplayOptions & p_DisplayOptions) const;
		string LogText(SN::LogContext & context, long p_Width) const;
		void WriteJS(ostream &p_Stream, const string &tabs) const;

		SNI_WorldSet *GetWorldSet();
		bool AddChildWorld(SNI_World *p_World);
		void AddToSetList();
		bool AddFailedContext(SNI_World *p_World);
		bool CompatibleWorld(SNI_World * p_World);

		bool CheckForWorldSetFails();
		void BuildFailedWorldSets(SNI_WorldSetList &p_FailedWorldSetList);
		void ClearContextFailMarks(SNI_WorldSetList &p_FailedWorldSetList);
		void MarkContextFailMarks();
		bool CheckFailedWorldSets(SNI_WorldSetList &p_FailedWorldSetList);

		void ClearFailMark();
		void MarkFail();
		bool IsFailMarked() const;

		bool Contains(SNI_World * p_World) const;

		void Mark(bool p_Mark);
		bool HasMark(bool p_Mark);
		void MarkWorld(bool p_Mark);
		void MarkChildWorlds(bool p_Mark);
		void MarkChildWorlds2();
		bool HasMutualExclusion();

		void Activate();
		bool IsActive() const;
		bool IsAnyActive() const;

		string Reason() const;

		bool IsEmpty() const;
		bool HasEmptyChild() const;
		SNI_World * GetContextWorld() const;
		bool IsProperSubWorld(SNI_World * p_World) const;
		bool IsSubWorld(SNI_World * p_World) const;
		void Negate(SNI_World * p_World);
		bool FailNoRemove();
		bool FailNoRemoveInContext(SNI_World *p_ContextWorld);
		void MarkEmpty(enum FailReason p_Reason);
		SN::SN_Error Fail(enum FailReason p_Reason);
		SN::SN_Error FailInContext(SNI_World *p_ContextWorld, enum FailReason p_Reason);

		void CountNegatedMap(SNI_WorldCount &negatedMap) const;
		void DeleteEmptyFromNegatedMap();
		
		SNI_World * OneParent(SNI_WorldSet * parentWorldSet);
		void AttachValue(const SN::SN_Expression &p_Value);

		void BreakPointIfDeleted();

	protected:
		virtual void PromoteMembers();

	private:
		SNI_WorldSet  * m_WorldSet;
		SNI_WorldList m_ChildList;
		SNI_World     *m_CloneParent;
		SNI_WorldList m_FailedContextList;
		SNI_WorldList m_NegatedList;

		bool m_Mark;
		bool m_IsEmpty;
		bool m_Active;
		enum FailReason m_Reason;

		bool m_FailMark;

		long m_WorldNo;
		SN::SN_Expression m_Value;
		string m_ValueString;
	};
}

#endif // !defined(SNI_WORLD_H_INCLUDED)
