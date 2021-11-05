#include "sni_getstate.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_GetState::SNI_GetState()
	{
	}

	SNI_GetState::~SNI_GetState()
	{
	}

	string SNI_GetState::GetTypeName() const
	{
		return "GetState";
	}

	string SNI_GetState::DisplayCpp() const
	{
		return "SN::GetState";
	}

	string SNI_GetState::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "GetState";
	}

	long SNI_GetState::GetPriority() const
	{
		return 4;
	}

	string SNI_GetState::DisplayCall(long priority, SNI_DisplayOptions &p_DisplayOptions, size_t p_NumParams, SN::SN_Expression *p_ParamList, const SNI_Expression *p_DebugSource) const
	{
		return p_ParamList[PC1_First].GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions) + ".State()";
	}

	SN::SN_Value SNI_GetState::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Expression()->DoGetState();
	}

	SN::SN_Expression SNI_GetState::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return SN::SN_Function(this, p_Param);
	}
}
