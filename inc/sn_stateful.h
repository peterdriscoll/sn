#if !defined(SN_STATEFUL_H_INCLUDED)
#define SN_STATEFUL_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_expression.h"

namespace SNI
{
	class SNI_Stateful;
}

namespace SN
{
	class SN_EXPORT SN_Stateful : public SN_Expression
	{
	public:
		static SN_Class Class();

		SN_Stateful(enum skynet::Mutability p_Mutability, const SN_Expression &p_Expression);
		SN_Stateful(const SN_Expression &p_Expression);
		virtual ~SN_Stateful();

		SN_Expression IsA(const SN_Expression &p_Parent) const;

		SNI::SNI_Stateful * GetSNI_Stateful() const;
		SNI::SNI_Expression * GetSNI_Expression() const;
	};
}

#endif // !defined(SN_STATEFUL_H_INCLUDED)
