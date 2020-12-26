#include "sni_lessthan.h"

#include "sn_operators.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_LessThan::SNI_LessThan()
	{
	}

	SNI_LessThan::~SNI_LessThan()
	{
	}

	string SNI_LessThan::GetTypeName() const
	{
		return "LessThan";
	}

	string SNI_LessThan::DisplayCpp() const
	{
		return "SN::LessThan";
	}

	string SNI_LessThan::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "<";
	}

	long SNI_LessThan::GetPriority() const
	{
		return 0;
	}

	string SNI_LessThan::GetOperator() const
	{
		return "<";
	}

	SN::SN_Value SNI_LessThan::PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Left.GetSNI_Value()->DoLessThan(p_Right.GetSNI_Value());
	}
}
