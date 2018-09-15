#include "sni_hasa.h"

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
	SNI_HasA::SNI_HasA()
	{
	}

	SNI_HasA::~SNI_HasA()
	{
	}

	string SNI_HasA::GetTypeName() const
	{
		return "HasMember";
	}

	string SNI_HasA::DisplayCpp() const
	{
		return "SN::HasMember";
	}

	string SNI_HasA::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "in";
	}

	long SNI_HasA::GetPriority() const
	{
		return 0;
	}

	SN::SN_Value SNI_HasA::PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Left.GetSNI_Value()->DoHasMember(p_Right.GetSNI_Value());
	}

	SN::SN_Expression SNI_HasA::PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return p_Left.HasMember(p_Right);
	}

	SN::SN_Value SNI_HasA::LeftInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Left) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_HasA::LeftInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return skynet::Null;
	}
}
