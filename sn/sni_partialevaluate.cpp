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

	string SNI_PartialEvaluate::GetTypeName() const
	{
		return "Evaluate";
	}

	string SNI_PartialEvaluate::DisplayCpp() const
	{
		return "Evaluate";
	}

	string SNI_PartialEvaluate::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "Evaluate";
	}

	long SNI_PartialEvaluate::GetPriority() const
	{
		return 1;
	}

	string SNI_PartialEvaluate::DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, SN::SN_ExpressionList * p_ParameterList, const SNI_Expression * p_DebugSource) const
	{

		return (*p_ParameterList)[0].GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions) + ".Evaluate()";
	}


	SN::SN_Value SNI_PartialEvaluate::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoPartialEvaluate();
	}

	SN::SN_Expression SNI_PartialEvaluate::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.DoPartialEvaluate();
	}

	SN::SN_Error SNI_PartialEvaluate::PartialUnifyInternal(SN::SN_Expression &p_value, SN::SN_Expression &p_Result)
	{
		return p_Result.AssertValue(p_value.PartialEvaluate());
	}

	SN::SN_Expression SNI_PartialEvaluate::CallArray(SN::SN_Expression * p_ParamList, long p_MetaLevel /* = 0 */) const
	{
		return p_ParamList[PC1_First].DoPartialEvaluate();
	}

	SN::SN_Expression SNI_PartialEvaluate::UnifyArray(SN::SN_Expression * p_ParamList)
	{
		SN::SN_Expression clone = p_ParamList[PU1_First].GetSNI_Expression()->Clone(this, NULL);
		LOG(WriteLine(SN::DebugLevel, "Evaluate " + clone.DisplayValueSN()));
		SNI_Thread::GetThread()->SetDebugId("evaluate");
		SNI_Thread::GetThread()->DebugCommand(SN::StaticPoint, "evaluate", SN::CallId);

		SN::SN_Error result = clone.DoPartialEvaluate();
		SNI_Frame::Pop();

		SNI_Thread::GetThread()->SetDebugId("evaluate");
		SNI_Thread::GetThread()->DebugCommand(SN::StaticPoint, "Evaluate", SN::ReturnId);

		return p_ParamList[PU1_Result].AssertValue(result);
	}
}
