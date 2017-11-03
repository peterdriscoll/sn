#if !defined(SN_NULL_H_INCLUDED)
#define SN_NULL_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"

namespace SNI
{
	class SNI_Null;
}

namespace SN
{
	class SN_Expression;
	class SN_Value;

	class SN_Null
	{
	public:
		SN_Null();
		SN_Null(const SN_Null &p_Other);
		virtual ~SN_Null();

		SNI::SNI_Null * GetSNI_Null() const;

	private:
		SNI::SNI_Null * m_Null;
	};
}

#endif // !defined(SN_NULL_H_INCLUDED)
