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
		return "PartialAssert";
	}

	string SNI_PartialAssert::DisplayCpp() const
	{
		return "PartialAssert";
	}

	string SNI_PartialAssert::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "PartialAssert";
	}

	long SNI_PartialAssert::GetPriority() const
	{
		return 1;
	}

	string SNI_PartialAssert::DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, size_t p_NumParams, SN::SN_Expression * p_ParamList, const SNI_Expression * p_DebugSource) const
	{

		return p_ParamList[PC1_First].GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions) + ".PartialAssert()";
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
		SN::SN_Expression value = p_value.DoPartialEvaluate();
		if (value.IsKnownValue())
		{
			return p_Result.PartialAssertValue(PrimaryFunctionValue(value));
		}
		return skynet::Fail;
	}

	SN::SN_Expression SNI_PartialAssert::CallArray(SN::SN_Expression * p_ParamList, long p_MetaLevel /* = 0 */) const
	{
		return p_ParamList[PU1_First].DoEvaluate();
	}

	SN::SN_Expression SNI_PartialAssert::UnifyArray(SN::SN_Expression * p_ParamList, const SNI_Expression *p_Source)
	{
		LOG(WriteLine(SN::DebugLevel, "Partial assert " + p_ParamList[PU1_First].DisplayValueSN()));

		SNI_Thread::GetThread()->UpdateIncrementId();

		Breakpoint(SN::DebugStop, SN::CallId, GetTypeName(), "Unify start", this, SN::CallPoint);

		SN::SN_Error result = p_ParamList[PU1_First].DoPartialAssert();
		if (result.IsSignificantError())
		{
			SNI_CallRecord *callRecord = new SNI_CallRecord("Partial assert failure: ", p_ParamList[PU1_First].GetSNI_Expression());
			LOGGING(callRecord->SetLogContext(context));
			result.GetSNI_Error()->AddNote(callRecord);
		}

		Breakpoint(SN::DebugStop, SN::ReturnId, GetTypeName(), "Unify end", this, SN::CallPoint);

		return p_ParamList[PU1_Result].AssertValue(result);
	}
}
