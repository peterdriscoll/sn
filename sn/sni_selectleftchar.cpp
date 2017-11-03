#include "sni_selectleftchar.h"

#include "sn.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_SelectLeftChar::SNI_SelectLeftChar()
	{
	}

	SNI_SelectLeftChar::~SNI_SelectLeftChar()
	{
	}

	void SNI_SelectLeftChar::PromoteMembers()
	{
	}

	string SNI_SelectLeftChar::GetTypeName() const
	{
		return "SelectLeftChar";
	}

	string SNI_SelectLeftChar::DisplayCpp() const
	{
		return "SN::SelectLeftChar";
	}

	string SNI_SelectLeftChar::DisplaySN(long /*priority*/, SNI_VariablePointerList & /*p_DisplayVariableList*/) const
	{
		return "SelectLeftChar";
	}

	long SNI_SelectLeftChar::GetPriority() const
	{
		return 1;
	}

	string SNI_SelectLeftChar::GetOperator() const
	{
		return "SelectLeftChar";
	}

	SN::SN_Value SNI_SelectLeftChar::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoSelectLeftChar();
	}

	SN::SN_Expression SNI_SelectLeftChar::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.SelectLeftChar();
	}

	SN::SN_Value SNI_SelectLeftChar::InverseFunctionValue(const SN::SN_Value &p_Param) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_SelectLeftChar::InverseFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return skynet::Null;
	}
}
