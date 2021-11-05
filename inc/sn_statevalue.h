#if !defined(SN_STATEVALUE_H_INCLUDED)
#define SN_STATEVALUE_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_base.h"
#include "sn_class.h"
#include "../sn/sni_statevalue.h"

namespace SN
{
	class SN_EXPORT SN_StateValue : public SN_Base<SNI::SNI_StateValue, SN_Expression, SN_Error>
	{
	public:
		static SN_Class Class();

		SN_StateValue();
		SN_StateValue(const SN_StateValue &p_Other);
		SN_StateValue(const SN_Expression& p_Value, const SN_Expression &p_State);
		virtual ~SN_StateValue();

		SN_Expression operator ==(const SN_Expression &p_Other) const;
		SN_Expression operator !=(const SN_Expression &p_Other) const;

		SN_Expression Assert() const;
		SN_Expression PartialAssert() const;
		SN_Expression Evaluate() const;
		SN_Expression PartialEvaluate() const;


		SN_Expression IsA(const SN_Expression &p_Parent) const;

		SNI::SNI_StateValue *GetSNI_StateValue() const;
		SNI::SNI_Expression *GetSNI_Expression() const;
	};
}

#endif // !defined(SN_STATEVALUE_H_INCLUDED)
