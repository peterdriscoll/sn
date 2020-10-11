#if !defined(SN_STATE_H_INCLUDED)
#define SN_STATE_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_base.h"
#include "sn_class.h"
#include "../sn/sni_state.h"

namespace SN
{
	class SN_EXPORT SN_State : public SN_Base<SNI::SNI_State, SN_Expression, SN_Error>
	{
	public:
		static SN_Class Class();

		SN_State();
		SN_State(const SN_State &p_Other);
		SN_State(const SN_Value &p_Other);
		virtual ~SN_State();

		SN_Expression IsA(const SN_Expression &p_Parent) const;

		SN_State AssignValue(const SN_Expression &p_Left, const SN_Expression &p_Right);

		SNI::SNI_State *GetSNI_State() const;
		SNI::SNI_Expression *GetSNI_Expression() const;
	};
}

#endif // !defined(SN_STATEVALUE_H_INCLUDED)
