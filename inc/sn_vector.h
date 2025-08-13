#if !defined(SN_VECTOR_H_INCLUDED)
#define SN_VECTOR_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_base.h"
#include "sn_class.h"
#include "../sn/sni_bool.h"

namespace SNI
{
	class SNI_Vector;
}

namespace SN
{
	class SN_Expression;
	class SN_Value;

	class SN_EXPORT SN_Vector : public SN_Base
	{
	public:
		static SN_Class Class();

		SN_Vector();
		virtual ~SN_Vector();

		SN_Expression operator [](const SN_Expression &p_Index);
		SN_Expression ReverseSubscript(const SN_Expression & p_Value);
		SN_Expression IsA(const SN_Expression &p_Parent) const;
		SN_Expression CountIf(const SN_Expression & p_Value);
		SN_Expression CountAll();
		SN_Expression Sum();

		SNI::SNI_Vector * GetSNI_Vector() const;
	};
}

#endif // !defined(SN_VECTOR_H_INCLUDED)
