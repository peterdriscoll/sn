#include "SN_MetaType.h"

#include "sn_value.h"

#include "sni_metatype.h"
#include "sni_value.h"

#include "sn_pch.h"

namespace SN
{
	/*static*/ SN_Class SN_MetaType::ExprClass()
    {
        return SNI_MetaType::ExprClass();
    }

    /*static*/ SN_Class SN_MetaType::Class()
	{
		return SNI_MetaType::Class();
	}

	SN_MetaType::SN_MetaType(long p_MetaLevel, const SN_Expression &p_Expression)
		: SN_Expression(new SNI::SNI_MetaType(p_MetaLevel, p_Expression.GetSNI_Expression()))
	{
	}

	SN_MetaType::SN_MetaType(const SN_Expression &p_Expression)
		: SN_Expression(new SNI::SNI_Meta(1, p_Expression.GetSNI_Expression()))
	{
	}

	SN_MetaType::~SN_MetaType()
	{
	}

	SNI::SNI_MetaType * SN_MetaType::GetSNI_MetaType() const
	{
		return dynamic_cast<SNI::SNI_MetaType *>(m_Expression);
	}

	SN_Expression SN_MetaType::IsA(const SN_Expression &p_Parent) const
	{
		return SN_Operators::IsA(*this, p_Parent);
	}
}
