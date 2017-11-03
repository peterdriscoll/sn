#if !defined(SN_DEBUG_H_INCLUDED)
#define SN_DEBUG_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_expression.h"

namespace SNI
{
	class SNI_Debug;
}

namespace SN
{
	class SN_EXPORT SN_Debug : public SN_Expression
	{
	public:
		SN_Debug(const SN_Expression &p_other);
		virtual ~SN_Debug();

	protected:
		SNI::SNI_Debug *GetSNI_Debug() const;
	};
}

#endif // !defined(SN_DEBUG_H_INCLUDED)
