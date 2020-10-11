#include "sni_imperativetransform.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_ImperativeTransform::SNI_ImperativeTransform()
	{
	}

	SNI_ImperativeTransform::~SNI_ImperativeTransform()
	{
	}

	string SNI_ImperativeTransform::GetTypeName() const
	{
		return "ImperativeTransform";
	}

	string SNI_ImperativeTransform::DisplayCpp() const
	{
		return "ImperativeTransform";
	}

	string SNI_ImperativeTransform::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "ImperativeTransform";
	}

	long SNI_ImperativeTransform::GetPriority() const
	{
		return 1;
	}

	string SNI_ImperativeTransform::DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, size_t p_NumParams, SN::SN_Expression * p_ParamList, const SNI_Expression * p_DebugSource) const
	{
		return p_ParamList[PC1_First].GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions) + ".Assert()";
	}


	SN::SN_Value SNI_ImperativeTransform::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_ImperativeTransform::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.ImperativeTransform();
	}

	SN::SN_Expression SNI_ImperativeTransform::DoTransform(const SN::SN_Expression &p_Param, long p_MetaLevel) const
	{
		bool p_Changed = false;
		bool p_Imperative = false;
		SNI_State *stateInput = new SNI_State();
		SNI_Expression *stateOutput = NULL;
		return p_Param.GetSNI_Expression()->DoImperativeTransform(p_Changed, p_Imperative, stateInput, stateOutput, p_MetaLevel)->GetValue();
	}

	SN::SN_Expression SNI_ImperativeTransform::CallArray(SN::SN_Expression * p_ParamList, long p_MetaLevel /* = 0 */) const
	{
		SN::SN_Expression transform = DoTransform(p_ParamList[PU1_First], p_MetaLevel);
		return transform.DoEvaluate(p_MetaLevel);
	}

	SN::SN_Expression SNI_ImperativeTransform::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		SN::SN_Expression transform = DoTransform((*p_ParameterList)[0], 0);
		LOG(WriteClonedExpression(SN::DebugLevel, "Transform: ", transform));

		return transform.DoPartialEvaluate(p_MetaLevel);
	}
	
	SN::SN_Error SNI_ImperativeTransform::PartialUnifyInternal(SN::SN_Expression &p_value, SN::SN_Expression &p_Result)
	{
		SN::SN_Expression transform = DoTransform(p_value, 0);
		LOG(WriteClonedExpression(SN::DebugLevel, "Transform: ", transform));
		LOG(WriteExp(transform));

		Breakpoint(SN::DebugStop, SN::LeftId, GetTypeName(), "Partial assert imperative transform", this, SN::CallPoint);
		SN::SN_Error err = transform.PartialAssertValue(p_Result);
		Breakpoint(SN::DebugStop, SN::RightId, GetTypeName(), "Partial assert imperative transform return", this, SN::CallPoint);
		return err;
	}

	SN::SN_Expression SNI_ImperativeTransform::UnifyArray(SN::SN_Expression * p_ParamList, const SNI_Expression *p_Source)
	{
		SN::SN_Expression transform = DoTransform(p_ParamList[PU1_First], 0);
		SN::SN_Expression clone = transform.GetSNI_Expression()->Clone(this, NULL);

		LOG(WriteClonedExpression(SN::DebugLevel, "Transform: ", clone));
		Breakpoint(SN::DebugStop, SN::LeftId, GetTypeName(), "Imperative transform", this, SN::CallPoint);

		SN::SN_Error err = transform.AssertValue(p_ParamList[PU1_Result]);

		if (err.IsSignificantError())
		{
			SNI_CallRecord *callRecord = new SNI_CallRecord("Imperative transform call", clone.GetSNI_Expression());
			LOGGING(callRecord->SetLogContext(context));
			err.GetSNI_Error()->AddNote(callRecord);
		}
		SNI_Frame::Pop();

		Breakpoint(SN::DebugStop, SN::RightId, GetTypeName(), "Imperative transform return", this, SN::CallPoint);

		return err;
	}
}
