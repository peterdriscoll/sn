#include "sn_class.h"

#include "sni_class.h"

#include "sn_pch.h"

namespace SN
{
	SN_Class::SN_Class()
		: SN_Base(NULL)
	{
	}

    SN_Class::SN_Class(const std::string &p_Name, const std::string &p_DomainName)
 		: SN_Base(new SNI_Class(p_Name, p_DomainName))
    {
    }
	SN_Class::SN_Class(const SN_Value &p_Value)
		: SN_Base(dynamic_cast<SNI::SNI_Class *>(p_Value.GetSNI_Value()))
	{
	}

	SN_Class::SN_Class(SNI::SNI_Class* p_Class)
		: SN_Base(p_Class)
	{
	}

	SN_Class::SN_Class(const SN_Class &p_Class)
		: SN_Base(p_Class.m_Expression)
	{
	}

	SN_Class::~SN_Class()
	{
	}

	SNI::SNI_Value* SN_Class::GetSNI_Value()
	{
		return dynamic_cast<SNI::SNI_Value*>(m_Expression);
	}
 	SNI::SNI_Value* SN_Class::GetSNI_Value() const
	{
		return dynamic_cast<SNI::SNI_Value*>(const_cast<SNI::SNI_Base*>(m_Expression));
    }
	SN_Expression SN_Class::operator ==(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator ==(*this, p_Other);
	}
	SN_Expression SN_Class::operator !=(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator !=(*this, p_Other);
	}
	SN_Expression SN_Class::operator <(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator <(*this, p_Other);
	}
	SN_Expression SN_Class::operator >(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator >(*this, p_Other);
	}
	SN_Expression SN_Class::operator <=(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator <=(*this, p_Other);
	}
	SN_Expression SN_Class::operator >=(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator >=(*this, p_Other);
	}
    void SN_Class::SetValue(const SNI_Class *pClass)
    {
        m_Expression = dynamic_cast<SNI::SNI_Base*>(const_cast<SNI_Class *>(pClass));
    }
	SN_Expression SN_Class::IsA(const SN_Expression &p_Parent) const
	{
		return SN_Operators::IsA(*this, p_Parent);
	}

	SNI::SNI_Class* SN_Class::GetSNI_Class()
	{
		return dynamic_cast<SNI::SNI_Class*>(m_Expression);
	}

	SNI::SNI_Class* SN_Class::GetSNI_Class() const
	{
		return dynamic_cast<SNI::SNI_Class*>(m_Expression);
	}
}
