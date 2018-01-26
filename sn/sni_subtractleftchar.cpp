#include "sni_subtractleftchar.h"

#include "sn.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_SubtractLeftChar::SNI_SubtractLeftChar()
	{
	}

	SNI_SubtractLeftChar::~SNI_SubtractLeftChar()
	{
	}

	void SNI_SubtractLeftChar::PromoteMembers()
	{
	}

	string SNI_SubtractLeftChar::GetTypeName() const
	{
		return "SubtractLeftChar";
	}

	string SNI_SubtractLeftChar::DisplayCpp() const
	{
		return "SN::SubtractLeftChar";
	}

	string SNI_SubtractLeftChar::DisplaySN(long /*priority*/, SNI_VariableConstPointerList & /*p_DisplayVariableList*/) const
	{
		return "SubtractLeftChar";
	}

	long SNI_SubtractLeftChar::GetPriority() const
	{
		return 1;
	}

	SN::SN_Value SNI_SubtractLeftChar::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoSubtractLeftChar();
	}

	SN::SN_Expression SNI_SubtractLeftChar::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.SubtractLeftChar();
	}

	SN::SN_Value SNI_SubtractLeftChar::InverseFunctionValue(const SN::SN_Value &p_Param) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_SubtractLeftChar::InverseFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return skynet::Null;
	}
}
