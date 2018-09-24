#if !defined(SN_META_H_INCLUDED)
#define SN_META_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_expression.h"

namespace SNI
{
	class SNI_Meta;
}

namespace SN
{
	class SN_EXPORT SN_Meta : public SN_Expression
	{
	public:
		static SN_Class Class();

		SN_Meta(long p_MetaLevel, const SN_Expression &p_Expression);
		SN_Meta(const SN_Expression &p_Expression);
		virtual ~SN_Meta();

		void AddMetaLevel(long p_DeltaMetaLevel);

		SN_Expression IsA(const SN_Expression &p_Parent) const;

		SNI::SNI_Meta * GetSNI_Meta() const;
	};
}

#endif // !defined(SN_META_H_INCLUDED)
