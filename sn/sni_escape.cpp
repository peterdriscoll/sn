#include "sni_escape.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Escape::SNI_Escape(enum SN::EscapeType p_EscapeType)
		: m_EscapeType(p_EscapeType)
	{
	}

	SNI_Escape::~SNI_Escape()
	{

	}

	std::string SNI_Escape::GetTypeName() const
	{
		return "Escape";
	}

	std::string SNI_Escape::DisplayCpp() const
	{
		return "SN::Escape";
	}

	std::string SNI_Escape::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "Escape";
	}

	long SNI_Escape::GetPriority() const
	{
		return 1;
	}

	SN::SN_Value SNI_Escape::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoEscape(m_EscapeType);
	}

	SN::SN_Value SNI_Escape::InverseFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoUnescape(m_EscapeType);
	}

	SN::SN_Expression SNI_Escape::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		SN::SN_FunctionDef def(skynet::UnescapeCPP);
		switch (m_EscapeType)
		{
		case SN::CPP:
			def = SN::UnescapeCPP;
			break;
		case SN::JSON:
			def = SN::UnescapeJSON;
			break;
		}
		return def.PrimaryFunctionExpression(p_Param);
	}
}
