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

namespace SNI
{
	class SNI_DelayedCall : public SNI_Expression
	{
		PGC_CLASS(SNI_DelayedCall)
	public:
		SNI_DelayedCall();
		SNI_DelayedCall(SN::SN_FunctionDef p_Function, SN::SN_Expression*p_ParameterList, SNI_World * p_World = NULL);
		virtual ~SNI_DelayedCall();

		virtual size_t CallCardinality() const;
		virtual bool IsNull() const;

		bool Run();
		bool IsCallRequested() const;
		void SetWorld(SNI_World *p_World);
		bool IsLocked() const;
		void Lock();
		void Unlock();

	private:
		SN::SN_FunctionDef    m_Function;
		SN::SN_Expression     *m_ParameterList;
		bool                  m_Requested;
		SNI_World             *m_World;
		bool                  m_Locked;
		SN::SN_Error          m_Error;
	public:
		void LinkToVariables();
	private:
		void LinkVariable(SN::SN_Expression &p_Parameter);
	};

	typedef list<SNI_DelayedCall*> SNI_DelayedCallList;
}

#endif // !defined(SNI_DELAYEDCALL_H_INCLUDED)
