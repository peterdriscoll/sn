#include "sni_hasmember.h"

#include "logcontext.h"
#include "sn_parameter.h"
#include "sn_error.h"

#include "sni_helpers.h"
#include "sni_null.h"
#include "sni_error.h"
#include "sni_result.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_HasMember::SNI_HasMember()
	{
	}

	SNI_HasMember::~SNI_HasMember()
	{
	}

	string SNI_HasMember::GetTypeName() const
	{
		return "HasMember";
	}

	string SNI_HasMember::DisplayCpp() const
	{
		return "SN::HasMember";
	}

	string SNI_HasMember::DisplaySN(long /*priority*/, SNI_VariablePointerList & /*p_DisplayVariableList*/) const
	{
		return "in";
	}

	long SNI_HasMember::GetPriority() const
	{
		return 0;
	}

	SN::SN_Value SNI_HasMember::PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Left.GetSNI_Value()->DoHasMember(p_Right.GetSNI_Value());
	}

	SN::SN_Expression SNI_HasMember::PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return p_Left.HasMember(p_Right);
	}

	SN::SN_Value SNI_HasMember::LeftInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Left) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_HasMember::LeftInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return skynet::Null;
	}
}
