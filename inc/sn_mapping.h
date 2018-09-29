#if !defined(SN_MAPPING_H_INCLUDED)
#define SN_MAPPING_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_base.h"
#include "sn_class.h"
#include "../sn/sni_bool.h"

namespace SNI
{
	class SNI_Mapping;
}

namespace SN
{
	class SN_Expression;
	class SN_Value;

	class SN_EXPORT SN_Mapping : public SN_Base<SNI::SNI_Mapping, SN_Expression, SN_Error>
	{
	public:
		static SN_Class Class();

		SN_Mapping();
		virtual ~SN_Mapping();

		SN_Expression operator [](const SN_Expression &p_Index);
		SN_Expression ReverseSubscript(const SN_Expression & p_Value);
		SN_Expression IsA(const SN_Expression &p_Parent) const;

		SNI::SNI_Mapping * GetSNI_Mapping() const;
	};
}

#endif // !defined(SN_MAPPING_H_INCLUDED)
