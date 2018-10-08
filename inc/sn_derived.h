#if !defined(SN_DERIVED_H_INCLUDED)
#define SN_DERIVED_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_base.h"
#include "sn_class.h"

namespace SNI
{
	class SNI_Derived;
}

namespace SN
{
	class SN_Expression;
	class SN_Value;

	class SN_EXPORT SN_Derived : public SN_Base<SNI::SNI_Derived, SN_Expression, SN_Error>
	{
	public:
		static SN_Class Class();

		SN_Derived();
		virtual ~SN_Derived();

		SN_Expression IsA(const SN_Expression &p_Parent) const;

		SNI::SNI_Derived * GetSNI_Derived() const;
	};
}

#endif // !defined(SN_DERIVED_H_INCLUDED)
