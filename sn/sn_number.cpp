#include "sn_number.h"

#include "sn_expression.h"
#include "sn_value.h"
#include "sn_string.h"
#include "sn_null.h"
#include "sn_error.h"
#include "sn_real.h"
#include "sn_bool.h"
#include "sn_value.h"

#include "sni_number.h"
#include "sni_expression.h"
#include "sni_value.h"
#include "sni_string.h"
#include "sni_null.h"
#include "sni_error.h"
#include "sni_real.h"
#include "sni_value.h"
#include "sni_bool.h"

#include "sn_pch.h"

namespace SN
{
	SN_Number::SN_Number()
		: m_Number(NULL)
	{
	}

	SN_Number::SN_Number(const SN_Number &p_Number)
		: m_Number(p_Number.m_Number)
	{
	}

	SN_Number::SN_Number(const SN_Expression &p_Expression)
		: m_Number(dynamic_cast<SNI::SNI_Number *>(p_Expression.GetSNI_Expression()))
	{
	}

	SN_Number::SN_Number(const SN_Value &p_Value)
		: m_Number(dynamic_cast<SNI::SNI_Number *>(p_Value.GetSNI_Value()))
	{
	}

	SN_APPLY_TYPES(Number, m_Number, SN_CONSTRUCTOR_IMPL)

	SN_Number::SN_Number(const long &p_long)
		: m_Number(dynamic_cast<SNI::SNI_Number *>(new SNI::SNI_Long(p_long)))
	{
	}


	SN_Number::SN_Number(const int &p_int)
		: m_Number(dynamic_cast<SNI::SNI_Number *>(new SNI::SNI_Double(p_int)))
	{
	}


	SN_Number::SN_Number(SNI::SNI_Number *p_Number)
		: m_Number(p_Number)
	{
	}

	SN_Number::SN_Number(SNI::SNI_Expression *p_Expression)
		: m_Number(dynamic_cast<SNI::SNI_Number *>(p_Expression))
	{
	}


	SN_Number::SN_Number(const bool &p_Bool)
		: m_Number(dynamic_cast<SNI::SNI_Number *>(new SNI::SNI_Bool(p_Bool)))
	{
	}

	SN_Number::SN_Number(const SN_Error &p_Error)
		: m_Number(dynamic_cast<SNI::SNI_Number *>(p_Error.GetSNI_Error()))
	{
	}

	SN_Number::~SN_Number()
	{
	}

	SNI::SNI_Number * SN_Number::GetSNI_Number() const
	{
		return m_Number;
	}

	SN_Expression SN_Number::operator ==(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator ==(*this, p_Other);
	}

	SN_Expression SN_Number::operator !=(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator !=(*this, p_Other);
	}

	SN_Expression SN_Number::operator <(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator <(*this, p_Other);
	}

	SN_Expression SN_Number::operator >(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator >(*this, p_Other);
	}

	SN_Expression SN_Number::operator <=(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator <=(*this, p_Other);
	}

	SN_Expression SN_Number::operator >=(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator >=(*this, p_Other);
	}

	SN_Expression SN_Number::operator +(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator +(*this, p_Other);
	}

	//-----------------

	bool SN_Number::Equivalent(const SN_Expression &p_Other) const
	{
		return m_Number->Equivalent(dynamic_cast<SNI::SNI_Object *>(p_Other.GetSNI_Expression()));
	}

	string SN_Number::GetString() const
	{
		return m_Number->GetString();
	}

	SN_Expression SN_Number::operator&&(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator &&(*this, p_Other);
	}

	SN_Expression SN_Number::operator||(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator ||(*this, p_Other);
	}

}
