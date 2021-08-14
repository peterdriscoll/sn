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
		SNI_DelayedCall(SN::SN_FunctionDef p_Function, size_t p_NumParams, SN::SN_Expression *p_ParamList, const SNI_Expression *p_Source, SNI_Frame *p_Frame, SNI_World * p_World = NULL);
		virtual ~SNI_DelayedCall();

		virtual string GetTypeName() const;
		virtual string DisplaySN(long, SNI_DisplayOptions &) const;

		virtual size_t CallCardinality() const;
		virtual void ExpandedBooleanResult();
		virtual bool IsNull() const;

		bool Run();
		bool IsCallRequested() const;
		void SetWorld(SNI_World *p_World);
		bool IsLocked() const;
		void Lock();
		void Unlock();
		void Display();
		bool IsScheduled();
		void MarkScheduled();
		void WriteJSON(ostream & p_Stream, SNI::SNI_DisplayOptions & p_DisplayOptions);

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
