#if !defined(SN_INSTANCE_H_INCLUDED)
#define SN_INSTANCE_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_base.h"

namespace SNI
{
	class SNI_Instance;
}

namespace SN
{
	class SN_EXPORT SN_Instance : public SN_Base<SNI::SNI_Instance, SN_Expression, SN_Error>
	{
	public:
		SN_Instance();
		SN_Instance(const SN_Instance &p_Other);
		SN_Instance(const SN_Value &p_Other);
		virtual ~SN_Instance();

		SN_Expression IsA(const SN_Expression & p_Parent) const;

		SNI::SNI_Instance *GetSNI_Instance() const;
	};
}

#endif // !defined(SN_INSTANCE_H_INCLUDED)
