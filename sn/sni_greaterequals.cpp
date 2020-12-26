#include "sni_greaterequals.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_GreaterEquals::SNI_GreaterEquals()
	{
	}

	SNI_GreaterEquals::~SNI_GreaterEquals()
	{
	}

	string SNI_GreaterEquals::GetTypeName() const
	{
		return "GreaterEquals";
	}

	string SNI_GreaterEquals::DisplayCpp() const
	{
		return "SN::GreaterEquals";
	}

	string SNI_GreaterEquals::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return ">=";
	}

	long SNI_GreaterEquals::GetPriority() const
	{
		return 0;
	}

	string SNI_GreaterEquals::GetOperator() const
	{
		return ">=";
	}

	SN::SN_Value SNI_GreaterEquals::PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Left.GetSNI_Value()->DoGreaterEquals(p_Right.GetSNI_Value());
	}
}
