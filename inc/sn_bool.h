#if !defined(SN_BOOL_H_INCLUDED)
#define SN_BOOL_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_base.h"
#include "sn_class.h"
#include "../sn/sni_bool.h"

namespace SN
{
	class SN_EXPORT SN_Bool : public SN_Base
	{
	public:
		static SN_Class Class();

		SN_Bool();
		SN_Bool(bool p_other);
		SN_Bool(const SN_Bool &p_Other);
		SN_Bool(const SN_Value& p_Other);
		SN_Bool(const SN_Expression& p_Other);
		virtual ~SN_Bool();

		SNI::SNI_Bool* GetSNI_Bool();
		SNI::SNI_Bool* GetSNI_Bool() const;

		SN_Expression operator ==(const SN_Expression &p_Other) const;
		SN_Expression operator !=(const SN_Expression &p_Other) const;

		SN_Expression Assert() const;
		SN_Expression PartialAssert() const;
		SN_Expression Evaluate() const;
		SN_Expression PartialEvaluate() const;

		SN_Expression operator!() const;
		SN_Expression operator &&(const SN_Expression &p_Other) const;
		SN_Expression operator &&(const bool p_Other) const;
		SN_Expression operator ||(const SN_Expression &p_Other) const;
		SN_Expression operator ||(const bool p_Other) const;

		SN_Expression If(const SN_Expression &p_PositiveCase, const SN_Expression &p_NegativeCase) const;
		SN_Expression Implies(const SN_Expression &p_PositiveCase) const;
		SN_Expression RevAnd(const SN_Expression &p_PositiveCase) const;
		SN_Expression RevOr(const SN_Expression &p_Condition) const;
		SN_Expression UnaryAnd() const;
		SN_Expression UnaryOr() const;
		SN_Expression Collapse() const;

		SN_Expression IsA(const SN_Expression &p_Parent) const;

		bool GetBool() const;
	};
}

#endif // !defined(SN_BOOL_H_INCLUDED)
