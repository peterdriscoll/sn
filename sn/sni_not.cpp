#include "sni_not.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Not::SNI_Not()
	{
	}

	SNI_Not::~SNI_Not()
	{
	}

	void SNI_Not::PromoteMembers()
	{
	}

	string SNI_Not::GetTypeName() const
	{
		return "Not";
	}

	string SNI_Not::DisplayCpp() const
	{
		return "SN::Not";
	}

	string SNI_Not::DisplaySN(long /*priority*/, SNI_VariableConstPointerList & /*p_DisplayVariableList*/) const
	{
		return "!";
	}

	long SNI_Not::GetPriority() const
	{
		return 4;
	}

	string SNI_Not::GetOperator() const
	{
		return "!";
	}

	SN::SN_Value SNI_Not::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.DoNot();
	}

	SN::SN_Expression SNI_Not::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return !p_Param;
	}
}
