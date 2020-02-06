#if !defined(SN_DOMAIN_H_INCLUDED)
#define SN_DOMAIN_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_base.h"
#include "sn_class.h"

namespace SNI
{
	class SNI_Domain;
}

namespace SN
{
	class SN_Expression;
	class SN_Value;

	class SN_EXPORT SN_Domain : public SN_Base<SNI::SNI_Domain, SN_Expression, SN_Error>
	{
	public:
		static SN_Class Class();

		SN_Domain(const string &p_Name);
		virtual ~SN_Domain();

		SN_Expression operator [](const string &p_Index);
		SN_Expression CreateMetaVariable(const SN_Expression &p_Index);
		SN_Expression IsA(const SN_Expression &p_Parent) const;

		SNI::SNI_Domain * GetSNI_Domain() const;
	};
}

#endif // !defined(SN_DOMAIN_H_INCLUDED)
