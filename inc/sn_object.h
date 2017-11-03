#if !defined(SN_OBJECT_H_INCLUDED)
#define SN_OBJECT_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"

namespace SNI
{
	class SNI_Object;
}

namespace SN
{
	class SN_EXPORT SN_Object
	{
	private:
		SNI::SNI_Object * m_Object;

	public:
		SN_Object();
		SN_Object(const SN_Object &p_Other);
		virtual ~SN_Object();

		bool Equivalent(const SN_Object &p_Other) const;
	};
}

#endif // !defined(SN_OBJECT_H_INCLUDED)
