#include "sni_greaterthan.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_GreaterThan::SNI_GreaterThan()
	{

	}

	SNI_GreaterThan::~SNI_GreaterThan()
	{

	}

	string SNI_GreaterThan::GetTypeName() const
	{
		return "GreaterThan";
	}

	string SNI_GreaterThan::DisplayCpp() const
	{
		return "SN::GreaterThan";
	}

	string SNI_GreaterThan::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return ">";
	}

	long SNI_GreaterThan::GetPriority() const
	{
		return 0;
	}

	string SNI_GreaterThan::GetOperator() const
	{
		return ">";
	}

	SN::SN_Value SNI_GreaterThan::PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Left.GetSNI_Value()->DoGreaterThan(p_Right.GetSNI_Value());
	}
}
