#if !defined(SNI_DELAYEDPROCESSOR_H_INCLUDED)
#define SNI_DELAYEDPROCESSOR_H_INCLUDED

#pragma once

#include <mutex>

#include "pgc.h"

#include "sni_expression.h"
#include "sn_functiondef.h"
#include "sn_parameter.h"

namespace SNI
{
	class SNI_DelayedCall;
	class SNI_String;

	using DelayedCallList = PGCX::vector_ref<SNI_DelayedCall>;
	using PreventRereadList = PGCX::unordered_map_ref<std::string, SNI_String>;

	#undef PGC_ACTION_OVER_MEMBERS
	#undef PGC_ACTION_OVER_CONTAINERS

	#define PGC_ACTION_OVER_MEMBERS(ACTION) \
		ACTION(Manager SNI_Manager)

	#define PGC_ACTION_OVER_CONTAINERS(ACTION) \
		ACTION(DelayedCallList, DelayedCallList, SNI_DelayedCall) \
		ACTION(FailedList, DelayedCallList, SNI_DelayedCall) \
		ACTION(PreventReread, PreventRereadList, SNI_String)

	class SNI_DelayedProcessor : public SNI_Expression, public PGCX::Task
	{
		PGC_CLASS(SNI_DelayedProcessor);

		//PGC_MEMBER_DEFINITIONS(SNI_DelayedProcessor);
	private:
		PGCX::MemberRef<SNI_Manager> m_Manager;
		DelayedCallList m_DelayedCallList;
		DelayedCallList m_FailedList;
		PreventRereadList m_PreventReread;

	public:
		SNI_Manager* GetManager()
		{
			return m_Manager.Get();
		}
		void SetManager(SNI_Manager* p_Pointer)
		{
			m_Manager.Set(p_Pointer, GetTransaction());
		}

		void PromoteMembers()
		{
			m_Manager.RequestPromotion(GetTransaction());
			m_DelayedCallList.PromoteAll();
			m_FailedList.PromoteAll();
			m_PreventReread.PromoteAll();
		}

		SNI_DelayedProcessor(const SNI_DelayedProcessor& other)
			: SNI_Expression(other)
			, m_Manager(other.m_Manager)
			, m_DelayedCallList(other.m_DelayedCallList)
			, m_FailedList(other.m_FailedList)
			, m_PreventReread(other.m_PreventReread)
		{
			m_Manager.RequestPromotion(GetTransaction());
			m_DelayedCallList.PromoteAll();
			m_FailedList.PromoteAll();
			m_PreventReread.PromoteAll();
		}

		virtual PGC::PGC_Base* CloneTo(void* memory) const override
		{
			return new (memory) SNI_DelayedProcessor(*this);
		}

	public:
		SNI_DelayedProcessor();
		SNI_DelayedProcessor(SNI_Manager* p_Manager);
		~SNI_DelayedProcessor();

		virtual void AddVariables(long p_MetaLevel, SNI_VariablePointerMap& p_Map);

		void Delay(SN::SN_FunctionDef p_Function, size_t p_NumParams, SN::SN_Expression *p_ParamList, const SNI_Expression *p_Source, SNI_World *p_World = NULL);
		SN::SN_Error Run(SN::SN_FunctionDef p_Function, size_t p_NumParams, SN::SN_Expression* p_ParamList, const SNI_Expression* p_Source, SNI_World* p_World);
		void DelayCall(SNI_DelayedCall *p_Call, SNI_World *p_World);
		SN::SN_Error Process();
		SN::SN_Error Check() const;
		void Request(SNI_DelayedCall *p_Call);
		virtual SN::SN_Error DoAssert();
		virtual SN::SN_Expression DoEvaluate(long p_MetaLevel = 0) const;
		virtual void Run();
		virtual bool Finish();
		virtual void Promote(PGC::PGC_Transaction *p_Transaction);
		PreventRereadList &GetPreventReread();
		virtual void Display();

		void RemoveEmptyCalls();

		void WriteJSON(ostream & p_Stream, SNI::SNI_DisplayOptions & p_DisplayOptions);
		size_t CountDelayedCalls();

	private:
		//SNI_DelayedCallList  m_DelayedCallList;
		//SNI_DelayedCallList  m_FailedList;
		bool                 m_Processing;
		mutex                m_SearchLock;
		//unordered_map<std::string, SN::SN_String> m_PreventReread;
		//PGCX::MemberRef<SNI_Manager> m_Manager;
	};
}

#endif //SNI_DELAYEDPROCESSOR_H_INCLUDED
