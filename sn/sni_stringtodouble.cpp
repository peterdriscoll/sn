#include "sni_stringtodouble.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_StringToDouble::SNI_StringToDouble()
	{
	}

	SNI_StringToDouble::~SNI_StringToDouble()
	{
	}

	std::string SNI_StringToDouble::GetTypeName() const
	{
		return "StringToDouble";
	}

	std::string SNI_StringToDouble::DisplayCpp() const
	{
		return "SN::StringToDouble";
	}

	std::string SNI_StringToDouble::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "StringToDouble";
	}

	long SNI_StringToDouble::GetPriority() const
	{
		return 1;
	}

	SN::SN_Value SNI_StringToDouble::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoStringToDouble();
	}

	SN::SN_Value SNI_StringToDouble::InverseFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoDoubleToString();
	}

	SN::SN_Expression SNI_StringToDouble::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return skynet::DoubleToString.PrimaryFunctionExpression(p_Param);
	}
}
