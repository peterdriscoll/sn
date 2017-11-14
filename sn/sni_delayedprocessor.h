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

	class SNI_DelayedProcessor : public PGCX::Task
	{
	public:
		static SNI_DelayedProcessor *GetProcessor();

		SNI_DelayedProcessor();
		~SNI_DelayedProcessor();

		void Delay(SN::SN_FunctionDef p_Function, SN::SN_ParameterList *p_ParameterList, SNI_World *p_World = NULL);
		void DelayCall(SNI_DelayedCall *p_Call, SNI_World *p_World);
		bool Process();
		void Request(SNI_DelayedCall *p_Call);
		virtual void Run();
		virtual bool Complete();
		virtual void Promote(PGCX::Transaction *p_Transaction);
		unordered_map<string, SN::SN_String> &GetPreventReread();
	private:
		SNI_DelayedCallList  m_DelayedCallList;
		SNI_DelayedCallList  m_FailedList;
		bool                 m_Processing;
		mutex                m_SearchLock;
		unordered_map<string, SN::SN_String> m_PreventReread;
	};
}

#endif //SNI_DELAYEDPROCESSOR_H_INCLUDED
