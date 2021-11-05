#include "sni_getvalue.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_GetValue::SNI_GetValue()
	{
	}

	SNI_GetValue::~SNI_GetValue()
	{
	}

	string SNI_GetValue::GetTypeName() const
	{
		return "GetValue";
	}

	string SNI_GetValue::DisplayCpp() const
	{
		return "SN::GetValue";
	}

	string SNI_GetValue::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "GetValue";
	}

	long SNI_GetValue::GetPriority() const
	{
		return 4;
	}

	string SNI_GetValue::DisplayCall(long priority, SNI_DisplayOptions &p_DisplayOptions, size_t p_NumParams, SN::SN_Expression *p_ParamList, const SNI_Expression *p_DebugSource) const
	{
		return p_ParamList[PC1_First].GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions)+".Value()";
	}

	SN::SN_Value SNI_GetValue::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Expression()->DoGetValue();
	}

	SN::SN_Expression SNI_GetValue::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return SN::SN_Function(this, p_Param);
	}
}
