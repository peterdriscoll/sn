#include "sni_subtractrightchar.h"
#include "sn.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_SubtractRightChar::SNI_SubtractRightChar()
	{
	}

	SNI_SubtractRightChar::~SNI_SubtractRightChar()
	{
	}

	void SNI_SubtractRightChar::PromoteMembers()
	{
	}

	string SNI_SubtractRightChar::GetTypeName() const
	{
		return "SubtractRightChar";
	}

	string SNI_SubtractRightChar::DisplayCpp() const
	{
		return "SN::SubtractRightChar";
	}

	string SNI_SubtractRightChar::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "SubtractRightChar";
	}

	long SNI_SubtractRightChar::GetPriority() const
	{
		return 1;
	}

	SN::SN_Value SNI_SubtractRightChar::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoSubtractRightChar();
	}

	SN::SN_Expression SNI_SubtractRightChar::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.SubtractRightChar();
	}

	SN::SN_Value SNI_SubtractRightChar::InverseFunctionValue(const SN::SN_Value &p_Param) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_SubtractRightChar::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return skynet::Null;
	}

}
