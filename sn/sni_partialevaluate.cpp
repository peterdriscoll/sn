#include "sni_partialevaluate.h"

#include "sn.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_PartialEvaluate::SNI_PartialEvaluate()
	{
	}

	SNI_PartialEvaluate::~SNI_PartialEvaluate()
	{
	}

	std::string SNI_PartialEvaluate::GetTypeName() const
	{
		return "Evaluate";
	}

	std::string SNI_PartialEvaluate::DisplayCpp() const
	{
		return "Evaluate";
	}

	std::string SNI_PartialEvaluate::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "Evaluate";
	}

	long SNI_PartialEvaluate::GetPriority() const
	{
		return 1;
	}

	std::string SNI_PartialEvaluate::DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, size_t p_NumParams, SN::SN_Expression * p_ParamList, const SNI_Expression * p_DebugSource) const
	{

		return p_ParamList[PC1_First].GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions) + ".Evaluate()";
	}


	SN::SN_Value SNI_PartialEvaluate::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoPartialEvaluate(0);
	}

	SN::SN_Expression SNI_PartialEvaluate::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.DoPartialEvaluate();
	}

	SN::SN_Error SNI_PartialEvaluate::PartialUnifyInternal(SN::SN_Expression &p_value, SN::SN_Expression &p_Result)
	{
		return p_Result.AssertValue(p_value.DoPartialEvaluate(0));
	}

	SN::SN_Expression SNI_PartialEvaluate::CallArray(SN::SN_Expression* p_ParamList, long p_MetaLevel, const SNI_Expression* p_Source) const
	{
		SN::SN_Expression input = p_ParamList[PC1_First].DoPartialEvaluate(p_MetaLevel);

		if (0 < p_MetaLevel)
		{
			return PrimaryFunctionExpression(input.GetVariableValue());
		}

		return input;
	}

	SN::SN_Expression SNI_PartialEvaluate::UnifyArray(SN::SN_Expression * p_ParamList, const SNI_Expression *p_Source)
	{
		SNI::SNI_DisplayOptions l_DisplayOptions(doTextOnly);

		LOG(WriteLine(SN::DebugLevel, "Partial evaluate " + DisplayValueSN(0, l_DisplayOptions)));
		Breakpoint(SN::DebugStop, SN::CallId, GetTypeName(), "Call", this, SN::CallPoint);

		SN::SN_Error result = p_ParamList[PC1_First].DoPartialEvaluate().GetError();

		Breakpoint(SN::DebugStop, SN::ReturnId, GetTypeName(), "Return", this, SN::CallPoint);

		return p_ParamList[PU1_Result].AssertValue(result);
	}
}
