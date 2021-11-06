#if !defined(SN_LOCAL_H_INCLUDED)
#define SN_LOCAL_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_expression.h"

class SNI::SNI_Local;

namespace SN
{
	class SN_Value;

	class SN_EXPORT SN_Local : public SN_Expression
	{
	public:
		static SN_Class Class();

		SN_Local(SNI::SNI_Local * p_Local);
		SN_Local(const SN_Variable &p_LocalVariable, const SN_Expression &p_Expression);
		virtual ~SN_Local();

		SNI::SNI_Local * GetSNI_Local() const;
	};
}

#endif // !defined(SN_LOCAL_H_INCLUDED)

