#include "sni_partialassert.h"

#include "sn.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_PartialAssert::SNI_PartialAssert()
	{
	}

	SNI_PartialAssert::~SNI_PartialAssert()
	{
	}

	string SNI_PartialAssert::GetTypeName() const
	{
		return "Assert";
	}

	string SNI_PartialAssert::DisplayCpp() const
	{
		return "Assert";
	}

	string SNI_PartialAssert::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "Assert";
	}

	long SNI_PartialAssert::GetPriority() const
	{
		return 1;
	}

	string SNI_PartialAssert::DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, SN::SN_ExpressionList * p_ParameterList, const SNI_Expression * p_DebugSource) const
	{

		return (*p_ParameterList)[0].GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions) + ".PartialAssert()";
	}


	SN::SN_Value SNI_PartialAssert::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoAssert();
	}

	SN::SN_Expression SNI_PartialAssert::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.Assert();
	}

	SN::SN_Error SNI_PartialAssert::PartialUnifyInternal(SN::SN_Expression &p_value, SN::SN_Expression &p_Result)
	{
		SN::SN_Expression value = p_value.PartialEvaluate();
		if (value.IsKnownValue())
		{
			return p_Result.PartialAssertValue(PrimaryFunctionValue(value));
		}
		return false;
	}

	SN::SN_Expression SNI_PartialAssert::CallArray(SN::SN_Expression * p_ParamList, long p_MetaLevel /* = 0 */) const
	{
		return p_ParamList[PU1_First].Evaluate();
	}

	SN::SN_Expression SNI_PartialAssert::UnifyArray(SN::SN_Expression * p_ParamList)
	{
		LOG(WriteLine(SN::DebugLevel, "Partial assert " + p_ParamList[PU1_First].DisplayValueSN()));

		SNI_Thread::GetThread()->UpdateIncrementId();

		SNI_Thread::GetThread()->SetDebugId("partialassert");
		SNI_Thread::GetThread()->DebugCommand(SN::StaticPoint, "Partial assert", SN::CallId);

		SN::SN_Error result = p_ParamList[PU1_First].DoPartialAssert();

		SNI_Thread::GetThread()->SetDebugId("partialassert");
		SNI_Thread::GetThread()->DebugCommand(SN::StaticPoint, "Partial assert", SN::ReturnId);

		return p_ParamList[PU1_Result].AssertValue(result);
	}
}
