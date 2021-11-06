#if !defined(SN_LET_H_INCLUDED)
#define SN_LET_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"

#include "sn_expression.h"

namespace SNI
{
	class SNI_Let;
}

namespace SN
{
	class SN_Value;

	class SN_EXPORT SN_Let : public SN_Expression
	{
	public:
		static SN_Class Class();

		SN_Let(SNI::SNI_Let *p_Let);
		SN_Let(const SN_Expression &p_Condition, const SN_Expression &p_Expression);
		virtual ~SN_Let();

		SNI::SNI_Let * GetSNI_Let() const;
	};
};

#endif // !defined(SN_LET_H_INCLUDED)
