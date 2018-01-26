#include "sni_lookaheadright.h"

#include "sn.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_LookaheadRight::SNI_LookaheadRight()
	{
	}

	SNI_LookaheadRight::~SNI_LookaheadRight()
	{
	}

	void SNI_LookaheadRight::PromoteMembers()
	{
	}

	string SNI_LookaheadRight::GetTypeName() const
	{
		return "LookaheadRight";
	}

	string SNI_LookaheadRight::DisplayCpp() const
	{
		return "SN::LookaheadRight";
	}

	string SNI_LookaheadRight::DisplaySN(long /*priority*/, SNI_VariableConstPointerList & /*p_DisplayVariableList*/) const
	{
		return "LookaheadRight";
	}

	long SNI_LookaheadRight::GetPriority() const
	{
		return 1;
	}

	SN::SN_Value SNI_LookaheadRight::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoLookaheadRight();
	}

	SN::SN_Expression SNI_LookaheadRight::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.LookaheadRight();
	}

	SN::SN_Value SNI_LookaheadRight::InverseFunctionValue(const SN::SN_Value &p_Param) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_LookaheadRight::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return skynet::Null;
	}
}
