#include "sni_inttostring.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_IntToString::SNI_IntToString()
	{
	}

	SNI_IntToString::~SNI_IntToString()
	{
	}

	std::string SNI_IntToString::GetTypeName() const
	{
		return "IntToString";
	}

	std::string SNI_IntToString::DisplayCpp() const
	{
		return "SN::IntToString";
	}

	std::string SNI_IntToString::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "IntToString";
	}

	long SNI_IntToString::GetPriority() const
	{
		return 1;
	}

	SN::SN_Value SNI_IntToString::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoIntToString();
	}

	SN::SN_Value SNI_IntToString::InverseFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoStringToInt();
	}

	SN::SN_Expression SNI_IntToString::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return skynet::StringToInt.PrimaryFunctionExpression(p_Param);
	}

}
