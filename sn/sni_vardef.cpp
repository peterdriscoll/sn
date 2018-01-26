#include "sni_vardef.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_VarDef::SNI_VarDef()
	{
	}

	SNI_VarDef::~SNI_VarDef()
	{
	}

	void SNI_VarDef::PromoteMembers()
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

	string SNI_VarDef::DisplaySN(long /*priority*/, SNI_VariableConstPointerList & /*p_DisplayVariableList*/) const
	{
		return "VarDef";
	}

	long SNI_VarDef::GetPriority() const
	{
		return 100;
	}
}
