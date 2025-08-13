#include "sni_evaluate.h"

#include "sn.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Evaluate::SNI_Evaluate()
	{
	}

	SNI_Evaluate::~SNI_Evaluate()
	{
	}

	std::string SNI_Evaluate::GetTypeName() const
	{
		return "Evaluate";
	}

	std::string SNI_Evaluate::DisplayCpp() const
	{
		return "Evaluate";
	}

	std::string SNI_Evaluate::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "Evaluate";
	}

	long SNI_Evaluate::GetPriority() const
	{
		return 1;
	}

	std::string SNI_Evaluate::DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, size_t p_NumParams, SN::SN_Expression * p_ParamList, const SNI_Expression * p_DebugSource) const
	{

		return p_ParamList[PC1_First].GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions) + ".Evaluate()";
	}


	SN::SN_Value SNI_Evaluate::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoEvaluate();
	}

	SN::SN_Expression SNI_Evaluate::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.DoEvaluate();
	}

	SN::SN_Error SNI_Evaluate::PartialUnifyInternal(SN::SN_Expression &p_value, SN::SN_Expression &p_Result)
	{
		return p_Result.AssertValue(p_value.DoPartialEvaluate());
	}

	SN::SN_Expression SNI_Evaluate::CallArray(SN::SN_Expression* p_ParamList, long p_MetaLevel, const SNI_Expression* p_Source) const
	{
		SN::SN_Expression input = p_ParamList[PC1_First].DoEvaluate(p_MetaLevel);

		if (0 < p_MetaLevel)
		{
			return PrimaryFunctionExpression(input.GetVariableValue());
		}

		return input;
	}

	SN::SN_Expression SNI_Evaluate::UnifyArray(SN::SN_Expression * p_ParamList, const SNI_Expression *p_Source)
	{
		SN::SN_Expression clone = p_ParamList[PU1_First].GetSNI_Expression()->Clone(this, NULL);
		LOG(WriteClonedExpression(SN::DebugLevel, "Eval: ", clone));
		Breakpoint(SN::DebugStop, SN::CallId, GetTypeName(), "Evaluate call", this, SN::CallPoint);

		SN::SN_Expression result = clone.DoEvaluate();

		SNI_Frame* topFrame = SNI_Frame::Top();
		SNI_Variable* resultVar = topFrame->GetResult();
		resultVar->SetValue(result);

		Breakpoint(SN::DebugStop, SN::ReturnId, GetTypeName(), "Evaluate return", this, SN::CallPoint);

		SNI_Frame::Pop();

		return p_ParamList[PU1_Result].AssertValue(result);
	}
}
