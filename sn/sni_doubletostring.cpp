#include "sni_doubletostring.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_DoubleToString::SNI_DoubleToString()
	{
	}

	SNI_DoubleToString::~SNI_DoubleToString()
	{
	}

	string SNI_DoubleToString::GetTypeName() const
	{
		return "DoubleToString";
	}

	string SNI_DoubleToString::DisplayCpp() const
	{
		return "SN::DoubleToString";
	}

	string SNI_DoubleToString::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "DoubleToString";
	}

	long SNI_DoubleToString::GetPriority() const
	{
		return 1;
	}

	SN::SN_Value SNI_DoubleToString::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoDoubleToString();
	}

	SN::SN_Expression SNI_DoubleToString::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.DoubleToString();
	}

	SN::SN_Value SNI_DoubleToString::InverseFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoStringToDouble();
	}

	SN::SN_Expression SNI_DoubleToString::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return p_Param.StringToDouble();
	}

}
