#include "sni_lookaheadleft.h"

#include "sn.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_LookaheadLeft::SNI_LookaheadLeft()
	{
	}

	SNI_LookaheadLeft::~SNI_LookaheadLeft()
	{
	}

	void SNI_LookaheadLeft::PromoteMembers()
	{
	}

	string SNI_LookaheadLeft::GetTypeName() const
	{
		return "LookaheadLeft";
	}

	string SNI_LookaheadLeft::DisplayCpp() const
	{
		return "SN::LookaheadLeft";
	}

	string SNI_LookaheadLeft::DisplaySN(long /*priority*/, SNI_VariableConstPointerList & /*p_DisplayVariableList*/) const
	{
		return "LookaheadLeft";
	}

	long SNI_LookaheadLeft::GetPriority() const
	{
		return 1;
	}

	SN::SN_Value SNI_LookaheadLeft::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoLookaheadLeft();
	}

	SN::SN_Expression SNI_LookaheadLeft::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.LookaheadLeft();
	}

	SN::SN_Value SNI_LookaheadLeft::InverseFunctionValue(const SN::SN_Value &p_Param) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_LookaheadLeft::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return skynet::Null;
	}

}
