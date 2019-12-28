#include "sni_vardef.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_VarDef::SNI_VarDef()
	{
	}

	SNI_VarDef::SNI_VarDef(const SNI_VarDef & p_VarDef)
		: SNI_Expression(p_VarDef)
	{
	}

	SNI_VarDef::SNI_VarDef(unsigned long p_Id)
		: SNI_Expression(p_Id)
	{
	}

	SNI_VarDef::~SNI_VarDef()
	{
	}

	string SNI_VarDef::GetTypeName() const
	{
		return "VarDef";
	}

	string SNI_VarDef::DisplayCpp() const
	{
		return "VarDef";
	}

	string SNI_VarDef::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "VarDef";
	}

	long SNI_VarDef::GetPriority() const
	{
		return 100;
	}
}
