#if !defined(SN_METATYPE_H_INCLUDED)
#define SN_METATYPE_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_value.h"

namespace SNI
{
	class SNI_MetaType;
}

namespace SN
{
	class SN_EXPORT SN_MetaType : public SN_Expression
	{
	public:
        static SN_Class ExprClass(); 
		static SN_Class Class();

		SN_MetaType(long p_MetaLevel, const SN_Expression &p_Expression);
		SN_MetaType(const SN_Expression &p_Expression);
		virtual ~SN_MetaType();

		void AddMetaLevel(long p_DeltaMetaLevel);

		SN_Expression IsA(const SN_Expression &p_Parent) const;

		SNI::SNI_MetaType * GetSNI_MetaType() const;
	};
}

#endif // !defined(SN_METATYPE_H_INCLUDED)
