#if !defined(SNI_DELAYEDPROCESSOR_H_INCLUDED)
#define SNI_DELAYEDPROCESSOR_H_INCLUDED

#pragma once

#include <list>
#include <mutex>
#include <unordered_map>
using namespace std;

#include "pgc.h"

#include "sn_expression.h"
#include "sn_functiondef.h"
#include "sn_parameter.h"

namespace SNI
{
	class SNI_DelayedCall;
	typedef list<SNI_DelayedCall*> SNI_DelayedCallList;

	class SNI_DelayedProcessor : public SNI_Expression, public PGCX::Task
	{
		PGC_CLASS(SNI_DelayedProcessor);
	public:
		static SNI_DelayedProcessor *GetProcessor();

		SNI_DelayedProcessor(SNI_Manager *p_Manager);
		~SNI_DelayedProcessor();

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
		unordered_map<string, SN::SN_String> &GetPreventReread();
		virtual void Display();

		void RemoveEmptyCalls();

		void WriteJSON(ostream & p_Stream, SNI::SNI_DisplayOptions & p_DisplayOptions);
		size_t CountDelayedCalls();
	private:
		SNI_DelayedCallList  m_DelayedCallList;
		SNI_DelayedCallList  m_FailedList;
		bool                 m_Processing;
		mutex                m_SearchLock;
		unordered_map<string, SN::SN_String> m_PreventReread;
		SNI_Manager         *m_Manager;
	};
}

#endif //SNI_DELAYEDPROCESSOR_H_INCLUDED
