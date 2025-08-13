#if !defined(SN_CLASS_H_INCLUDED)
#define SN_CLASS_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_base.h"

namespace SNI
{
	class SNI_Class;
}

namespace SN
{
	class SN_EXPORT SN_Class : public SN_Base
	{
	public:
		SN_Class();
		SN_Class(const SN_Class &p_Other);
		SN_Class(const SN_Value& p_Other);
		SN_Class(SNI::SNI_Class* p_Other);
		virtual ~SN_Class();

		SN_Expression IsA(const SN_Expression & p_Parent) const;

		SNI::SNI_Class* GetSNI_Class();

		SNI::SNI_Class *GetSNI_Class() const;
	};
}

#endif // !defined(SN_CLASS_H_INCLUDED)
