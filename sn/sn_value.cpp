#include "sn_value.h"

#include "sn_variable.h"
#include "sn_bool.h"
#include "sn_set.h"
#include "sn_number.h"
#include "sn_real.h"
#include "sn_char.h"
#include "sn_string.h"
#include "sn_stringref.h"
#include "sn_null.h"
#include "sn_meta.h"
#include "sn_valueset.h"
#include "sn_error.h"

#include "sni_bool.h"
#include "sni_set.h"
#include "sni_number.h"
#include "sni_real.h"
#include "sni_char.h"
#include "sni_string.h"
#include "sni_stringref.h"
#include "sni_variable.h"
#include "sni_local.h"
#include "sni_valueset.h"
#include "sni_taggedvalue.h"
#include "sni_variable.h"
#include "sni_meta.h"
#include "sni_error.h"
#include "sni_null.h"

#include "sn_pch.h"

namespace SN
{
	SN_Value::SN_Value()
		: SN_Expression()
	{
	}

	SN_Value::SN_Value(const SN_Value &p_Value)
		: SN_Expression(p_Value.GetSNI_Value())
	{
	}

	SN_Value::SN_Value(const SN_Expression &p_Expression)
		: SN_Expression(dynamic_cast<SNI::SNI_Value *>(p_Expression.GetSNI_Expression()))
	{
	}

	SN_Value::SN_Value(const SN_String &p_String)
		: SN_Expression(dynamic_cast<SNI::SNI_Value *>(p_String.GetSNI_String()))
	{
	}

	SN_Value::SN_Value(const SN_StringRef &p_String)
		: SN_Expression(dynamic_cast<SNI::SNI_Value *>(p_String.GetSNI_StringRef()))
	{
	}


	SN_Value::SN_Value(const SN_Bool &p_Bool)
		: SN_Expression(dynamic_cast<SNI::SNI_Value *>(p_Bool.GetSNI_Bool()))
	{
	}

	SN_Value::SN_Value(const SN_Char &p_Char)
		: SN_Expression(dynamic_cast<SNI::SNI_Value *>(p_Char.GetSNI_Char()))
	{
	}

	SN_Value::SN_Value(const SN_Variable &p_Variable)
		: SN_Expression(dynamic_cast<SNI::SNI_Value *>(p_Variable.GetSNI_Variable()))
	{
	}

	SN_Value::SN_Value(const SN_Meta &p_Meta)
		: SN_Expression(dynamic_cast<SNI::SNI_Value *>(p_Meta.GetSNI_Meta()))
	{
	}

	SN_Value::SN_Value(const SN_ValueSet &p_ValueSet)
		: SN_Expression(dynamic_cast<SNI::SNI_Value *>(p_ValueSet.GetSNI_ValueSet()))
	{
	}

	SN_Value::SN_Value(const SN_Null &p_Null)
		: SN_Expression(dynamic_cast<SNI::SNI_Value *>(p_Null.GetSNI_Null()))
	{
	}

	SN_Value::SN_Value(const SN_Error &p_Error)
		: SN_Expression(dynamic_cast<SNI::SNI_Value *>(p_Error.GetSNI_Error()))
	{
	}

	SN_APPLY_TYPES(Value, SN_Expression, SN_CONSTRUCTOR_IMPL)

	SN_Value::SN_Value(const SN_Number &p_Number)
		: SN_Expression(dynamic_cast<SNI::SNI_Value *>(p_Number.GetSNI_Number()))
	{
	}

	SN_Value::SN_Value(const SNI::SNI_Expression *p_Expression)
		: SN_Expression(dynamic_cast<SNI::SNI_Value *>(const_cast<SNI::SNI_Expression *>(p_Expression)))
	{
	}

	SN_Value::~SN_Value()
	{
	}

	SNI::SNI_Value * SN_Value::GetSNI_Value() const
	{
		return dynamic_cast<SNI::SNI_Value *>(m_Expression);
	}
}
