#include "sni_stringtoint.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_StringToInt::SNI_StringToInt()
	{

	}

	SNI_StringToInt::~SNI_StringToInt()
	{

	}

	string SNI_StringToInt::GetTypeName() const
	{
		return "StringToInt";
	}

	string SNI_StringToInt::DisplayCpp() const
	{
		return "SN::StringToInt";
	}

	string SNI_StringToInt::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "StringToInt";
	}

	long SNI_StringToInt::GetPriority() const
	{
		return 1;
	}

	SN::SN_Value SNI_StringToInt::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoStringToInt();
	}

	SN::SN_Value SNI_StringToInt::InverseFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoIntToString();
	}

	SN::SN_Expression SNI_StringToInt::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return skynet::IntToString.PrimaryFunctionExpression(p_Param);
	}
}
