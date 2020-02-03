#include "sni_assert.h"

#include "sn.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Assert::SNI_Assert()
	{
	}

	SNI_Assert::~SNI_Assert()
	{
	}

	string SNI_Assert::GetTypeName() const
	{
		return "Assert";
	}

	string SNI_Assert::DisplayCpp() const
	{
		return "Assert";
	}

	string SNI_Assert::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "Assert";
	}

	long SNI_Assert::GetPriority() const
	{
		return 1;
	}

	string SNI_Assert::DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, SN::SN_ExpressionList * p_ParameterList, const SNI_Expression * p_DebugSource) const
	{

		return (*p_ParameterList)[0].GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions) + ".Assert()";
	}


	SN::SN_Value SNI_Assert::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoAssert();
	}

	SN::SN_Expression SNI_Assert::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.Assert();
	}

	SN::SN_Error SNI_Assert::PartialUnifyInternal(SN::SN_Expression &p_value, SN::SN_Expression &p_Result)
	{
		SN::SN_Expression value = p_value.PartialEvaluate();
		if (value.IsKnownValue())
		{
			return p_Result.PartialAssertValue(PrimaryFunctionValue(value));
		}
		return false;
	}

	SN::SN_Expression SNI_Assert::CallArray(SN::SN_Expression * p_ParamList, long p_MetaLevel /* = 0 */) const
	{
		return p_ParamList[PC1_First].DoEvaluate();
	}

	SN::SN_Expression SNI_Assert::UnifyArray(SN::SN_Expression * p_ParamList)
	{
		SN::SN_Expression clone = p_ParamList[PU1_First].GetSNI_Expression()->Clone(this, NULL);
		LOG(WriteLine(SN::DebugLevel, "Assert " + clone.DisplayValueSN()));
		SNI_Thread::GetThread()->SetDebugId("assert");
		SNI_Thread::GetThread()->DebugCommand(SN::StaticPoint, "Assert", SN::CallId);

		SN::SN_Error result = clone.DoAssert();
		SNI_Frame::Pop();

		SNI_Thread::GetThread()->SetDebugId("assert");
		SNI_Thread::GetThread()->DebugCommand(SN::StaticPoint, "Assert", SN::ReturnId);

		return p_ParamList[PU1_Result].AssertValue(result);
	}
}
