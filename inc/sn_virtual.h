#if !defined(SN_VIRTUAL_H_INCLUDED)
#define SN_VIRTUAL_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_base.h"
#include "sn_class.h"

namespace SNI
{
	class SNI_Virtual;
}

namespace SN
{
	class SN_Expression;
	class SN_Value;

	class SN_EXPORT SN_Virtual : public SN_Base<SNI::SNI_Virtual, SN_Expression, SN_Error>
	{
	public:
		static SN_Class Class();

		SN_Virtual();
		virtual ~SN_Virtual();

		SN_Expression IsA(const SN_Expression &p_Parent) const;

		SNI::SNI_Virtual * GetSNI_Virtual() const;
	};
}

#endif // !defined(SN_VIRTUAL_H_INCLUDED)
