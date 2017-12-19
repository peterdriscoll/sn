#include "sni_selectrightchar.h"

#include "sn.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_SelectRightChar::SNI_SelectRightChar()
	{
	}

	SNI_SelectRightChar::~SNI_SelectRightChar()
	{
	}

	void SNI_SelectRightChar::PromoteMembers()
	{
	}

	string SNI_SelectRightChar::GetTypeName() const
	{
		return "SelectRightChar";
	}

	string SNI_SelectRightChar::DisplayCpp() const
	{
		return "SN::SelectRightChar";
	}

	string SNI_SelectRightChar::DisplaySN(long /*priority*/, SNI_VariablePointerList & /*p_DisplayVariableList*/) const
	{
		return "SelectRightChar";
	}

	long SNI_SelectRightChar::GetPriority() const
	{
		return 1;
	}

	SN::SN_Value SNI_SelectRightChar::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoSelectRightChar();
	}

	SN::SN_Expression SNI_SelectRightChar::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.SelectRightChar();
	}

	SN::SN_Value SNI_SelectRightChar::InverseFunctionValue(const SN::SN_Value &p_Param) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_SelectRightChar::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return skynet::Null;
	}
}
