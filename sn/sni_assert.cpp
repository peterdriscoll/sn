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

	string SNI_Assert::DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, size_t p_NumParams, SN::SN_Expression * p_ParamList, const SNI_Expression * p_DebugSource) const
	{
		return p_ParamList[PC1_First].GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions) + ".Assert()";
	}


	SN::SN_Value SNI_Assert::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoAssert();
	}

	SN::SN_Expression SNI_Assert::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.Assert().GetSNI_Expression();
	}

	SN::SN_Error SNI_Assert::PartialUnifyInternal(SN::SN_Expression &p_value, SN::SN_Expression &p_Result)
	{
		SN::SN_Expression value = p_value.DoPartialEvaluate();
		if (value.IsKnownValue())
		{
			return p_Result.PartialAssertValue(PrimaryFunctionValue(value));
		}
		return skynet::Fail;
	}

	SN::SN_Expression SNI_Assert::CallArray(SN::SN_Expression* p_ParamList, long p_MetaLevel, const SNI_Expression* p_Source) const
	{
		SN::SN_Expression input = p_ParamList[PC1_First].DoEvaluate(p_MetaLevel);

		if (0 < p_MetaLevel)
		{
			return PrimaryFunctionExpression(input);
		}

		return input;
	}

	SN::SN_Expression SNI_Assert::UnifyArray(SN::SN_Expression * p_ParamList, const SNI_Expression *p_Source)
	{
		SN::SN_Expression clone = p_ParamList[PU1_First].GetSNI_Expression()->Clone(this, NULL);
		LOG(WriteClonedExpression(SN::DebugLevel, "Assert: ", clone));
		Breakpoint(SN::DebugStop, SN::CallId, GetTypeName(), "Assert", this, SN::CallPoint);

		SN::SN_Error result = clone.DoAssert();
		if (result.IsSignificantError())
		{
			SNI_CallRecord *callRecord = new SNI_CallRecord("Assert call", clone.GetSNI_Expression());
			LOGGING(callRecord->SetLogContext(context));
			result.GetSNI_Error()->AddNote(callRecord);
		}
		SNI_Frame::Pop();

		Breakpoint(SN::DebugStop, SN::ReturnId, GetTypeName(), "Assert return", this, SN::CallPoint);

		return p_ParamList[PU1_Result].AssertValue(result);
	}
}
