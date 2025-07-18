#include "sni_lessequals.h"

#include "sn_operators.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_LessEquals::SNI_LessEquals()
	{
	}

	SNI_LessEquals::~SNI_LessEquals()
	{
	}

	string SNI_LessEquals::GetTypeName() const
	{
		return "LessEquals";
	}

	string SNI_LessEquals::DisplayCpp() const
	{
		return "SN::LessEquals";
	}

	string SNI_LessEquals::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return ">=";
	}

	long SNI_LessEquals::GetPriority() const
	{
		return 0;
	}

	string SNI_LessEquals::GetOperator() const
	{
		return ">=";
	}

	SN::SN_Value SNI_LessEquals::PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Left.GetSNI_Value()->DoLessEquals(p_Right.GetSNI_Value());
	}
}
