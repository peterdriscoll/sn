#include "sni_unescape.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Unescape::SNI_Unescape(enum skynet::EscapeType p_EscapeType)
		: m_EscapeType(p_EscapeType)
	{
	}

	SNI_Unescape::~SNI_Unescape()
	{

	}

	string SNI_Unescape::GetTypeName() const
	{
		return "Unescape";
	}

	string SNI_Unescape::DisplayCpp() const
	{
		return "SN::Unescape";
	}

	string SNI_Unescape::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "Unescape";
	}

	long SNI_Unescape::GetPriority() const
	{
		return 1;
	}

	SN::SN_Value SNI_Unescape::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoUnescape(m_EscapeType);
	}

	SN::SN_Value SNI_Unescape::InverseFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoEscape(m_EscapeType);
	}

	SN::SN_Expression SNI_Unescape::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		SN::SN_FunctionDef def(skynet::EscapeCPP);
		switch (m_EscapeType)
		{
		case skynet::CPP:
			def = skynet::EscapeCPP;
			break;
		case skynet::JSON:
			def = skynet::EscapeJSON;
			break;
		}
		return def.PrimaryFunctionExpression(p_Param);
	}
}
