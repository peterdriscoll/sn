#if !defined(SNI_WORLD_H_INCLUDED)
#define SNI_WORLD_H_INCLUDED

#pragma once

#include "sni_object.h"

#include <vector>
using namespace std;

namespace SN
{
	class LogContext;
}

namespace SNI
{
	class SNI_WorldSet;

	class SNI_World;
	typedef vector<SNI_World*> SNI_WorldList;

	class SNI_World :public SNI_Object
	{
	private:
		PGC_CLASS(SNI_World);
	public:
		static SNI_World *ContextWorld();
		static void PushContextWorld(SNI_World *p_Context);
		static void PopContextWorld();

		SNI_World(SNI_WorldSet * p_WorldSet);
		virtual ~SNI_World();

		string DisplaySN() const;
		string DisplayShort() const;
		string DisplaySNChildWorlds() const;
		string LogText(SN::LogContext & context, long p_Width) const;

		SNI_WorldSet *GetWorldSet();
		bool AddChildWorld(SNI_World *p_World);
		void AddToSetList();

		bool CompatibleWorld(SNI_World * p_World);

		bool Contains(SNI_World * p_World) const;

		void Mark(bool p_Mark);
		bool HasMark(bool p_Mark);
		void MarkWorld(bool p_Mark);
		void MarkChildWorlds(bool p_Mark);
		bool HasMutualExclusion();

		void Activate();
		bool IsActive() const;
		bool IsAnyActive() const;

		bool IsEmpty() const;
		bool Fail();
		void MarkEmpty();
		SNI_World * OneParent(SNI_WorldSet * parentWorldSet);

	protected:
		virtual void PromoteMembers();

	private:
		SNI_WorldSet * m_WorldSet;
		SNI_WorldList m_ChildList;

		bool m_Mark;
		bool m_IsEmpty;
		bool m_Active;

		long m_WorldNo;
	};
}

#endif // !defined(SNI_WORLD_H_INCLUDED)
