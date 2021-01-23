#if !defined(SN_DEFINE_H_INCLUDED)
#define SN_DEFINE_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_expression.h"

namespace SNI
{
	class SNI_Define;
}

namespace SN
{
	class SN_Expression;
	class SN_Variable;

	class SN_EXPORT SN_Define : public SN_Expression
	{
	public:
		SN_Define();
		SN_Define(const SN_Variable &p_Variable, enum skynet::DefineType p_DefineType = skynet::Inname);
		virtual ~SN_Define();

		SNI::SNI_Define * GetSNI_Define() const;
	};
}

#endif // !defined(SN_DEFINE_H_INCLUDED)
