#if !defined(SN_ASSIGNMENT_H_INCLUDED)
#define SN_ASSIGNMENT_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_expression.h"

namespace SNI
{
	class SNI_Assignment;
}

namespace SN
{
	class SN_Expression;
	class SN_Value;

	class SN_EXPORT SN_Assignment : public SN_Expression
	{
	public:
		SN_Assignment(const SN_Assignment &p_Assignment);
		SN_Assignment(const SN_Expression &p_LeftHandSide, const SN_Expression &p_RightHandSide);
		virtual ~SN_Assignment();

		SN_Expression LeftHandSide() const;
		SN_Expression RightHandSide() const;

		SNI::SNI_Assignment * GetSNI_Assignment() const;
	};
}

#endif // !defined(SN_ASSIGNMENT_H_INCLUDED)
