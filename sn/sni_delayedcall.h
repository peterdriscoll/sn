#if !defined(SNI_DELAYEDCALL_H_INCLUDED)
#define SNI_DELAYEDCALL_H_INCLUDED

#pragma once

#include <list>
using namespace std;

#include "sni_expression.h"

#include "sn_expression.h"
#include "sn_parameter.h"
#include "sn_functiondef.h"
#include "sn_error.h"

#undef PGC_ACTION_OVER_VALUE_MEMBERS
#undef PGC_ACTION_OVER_MEMBER_REFS
#undef PGC_ACTION_OVER_CONTAINERS

#define PGC_ACTION_OVER_VALUE_MEMBERS(ACTION) \
        ACTION(Manager, SNI_Manager*, nullptr) \
        ACTION(Processing, bool, false)

#define PGC_ACTION_OVER_MEMBER_REFS(ACTION)

#define PGC_ACTION_OVER_MEMBER_CONTAINER_REFS(ACTION) \
		ACTION(DelayedCallList, DelayedCallList, SNI_DelayedCall) \
		ACTION(FailedList, DelayedCallList, SNI_DelayedCall) \
		ACTION(PreventReread, PreventRereadList, SNI_String)


namespace SNI
{
	class SNI_DelayedCall : public SNI_Expression
	{
		PGC_CLASS(SNI_DelayedCall)

		// PGC_MEMBER_DEFINITIONS_NOINIT(SNI_DelayedCall, SNI_Expression);
		// PGC_MEMBER_DEFINITIONS_NOINIT(SNI_DelayedCall, SNI_Expression);
		// PGC_MEMBER_DEFINITIONS_NOINIT(SNI_DelayedCall, SNI_Expression);
	public:
		SNI_DelayedCall(SN::SN_FunctionDef p_Function, size_t p_NumParams, SN::SN_Expression *p_ParamList, const SNI_Expression *p_Source, SNI_Frame *p_Frame, SNI_World * p_World = NULL);
		virtual ~SNI_DelayedCall();

		virtual std::string GetTypeName() const;
		virtual std::string DisplaySN(long, SNI_DisplayOptions &) const;
		virtual void AddVariables(long p_MetaLevel, SNI_VariablePointerMap& p_Map);

		virtual size_t CallCardinality() const;
		virtual void ExpandedBooleanResult();
		virtual bool IsNull() const;
		SN::SN_Error GetError() const;

		SN::SN_Error Run();
		bool IsCallRequested() const;
		void SetWorld(SNI_World *p_World);
		bool IsLocked() const;
		void Lock();
		void Unlock();
		void Display();
		bool IsScheduled();
		void MarkScheduled();
		bool EmptyWorld();
		void WriteJSON(ostream & p_Stream, SNI::SNI_DisplayOptions & p_DisplayOptions);

		virtual void PromoteMembers() override;

	private:
		SN::SN_FunctionDef    m_Function;
		size_t                m_NumParams;
		SN::SN_Expression     *m_ParamList;
		const SNI_Expression  *m_Source;
		SNI_World             *m_World;
		bool                  m_Locked;
		SN::SN_Error          m_Error;
		bool				  m_Scheduled;
		SNI_Frame             *m_Frame;

	public:
		void LinkToVariables();
	private:
		void LinkVariable(SN::SN_Expression &p_Parameter);
	};

	typedef list<SNI_DelayedCall*> SNI_DelayedCallList;
}

#endif // !defined(SNI_DELAYEDCALL_H_INCLUDED)
