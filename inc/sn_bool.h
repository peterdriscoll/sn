#if !defined(SN_BOOL_H_INCLUDED)
#define SN_BOOL_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_base.h"

namespace SNI
{
	class SNI_Bool;
	class SNI_FunctionDef;
}

namespace SN
{
	class SN_EXPORT SN_Bool : public SN_Base<SNI::SNI_Bool, SN_Expression, SN_Error>
	{
	public:
		SN_Bool();
		SN_Bool(bool p_other);
		SN_Bool(const SN_Bool &p_Other);
		SN_Bool(const SN_Value &p_Other);
		virtual ~SN_Bool();

		SN_Expression operator ==(const SN_Expression &p_Other) const;

		SN_Expression operator!() const;
		SN_Expression operator &&(const SN_Expression &p_Other) const;
		SN_Expression operator &&(const bool p_Other) const;
		SN_Expression operator ||(const SN_Expression &p_Other) const;
		SN_Expression operator ||(const bool p_Other) const;

		SN_Expression If(const SN_Expression &p_PositiveCase, const SN_Expression &p_NegativeCase) const;
		SN_Expression Implies(const SN_Expression &p_PositiveCase) const;
		SN_Expression RevAnd(const SN_Expression &p_PositiveCase) const;
		SN_Expression RevOr(const SN_Expression &p_Condition) const;
		SN_Expression UnaryRevAnd() const;
		SN_Expression UnaryRevOr() const;
		SN_Expression Collapse() const;

		bool GetBool() const;

		SNI::SNI_Bool *GetSNI_Bool() const;
	};
}

#endif // !defined(SN_BOOL_H_INCLUDED)
