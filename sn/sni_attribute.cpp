#include "sni_attribute.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Attribute::SNI_Attribute()
	{
	}

	SNI_Attribute::~SNI_Attribute()
	{
	}

	string SNI_Attribute::GetTypeName() const
	{
		return "Attribute";
	}

	string SNI_Attribute::DisplayCpp() const
	{
		return "SN::Attribute";
	}

	string SNI_Attribute::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "Attribute";
	}

	long SNI_Attribute::GetPriority() const
	{
		return 4;
	}

	string SNI_Attribute::DisplayCall(long priority, SNI_DisplayOptions &p_DisplayOptions, size_t p_NumParams, SN::SN_Expression *p_ParamList, const SNI_Expression *p_DebugSource) const
	{
		return p_ParamList[PC1_First].GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions);
	}

	SN::SN_Expression SNI_Attribute::UnifyArray(SN::SN_Expression* p_ParameterList, const SNI_Expression* p_Source)
	{
		SNI_Frame::Push(this, NULL);
		SNI_Frame* topFrame = SNI_Frame::Top();
		Breakpoint(SN::DebugStop, SN::LeftId, GetTypeName(), "Unify before evaluating parameter", p_Source, SN::CallPoint);
		SNI_Variable *firstVar = topFrame->CreateParameter(1, p_ParameterList[PU2_First]);
		
		SN::SN_Expression first = p_ParameterList[PU2_First].DoEvaluate();
		firstVar->SetValue(first);
		
		if (first.IsKnownValue())
		{
			SN::SN_Expression attribute = PrimaryFunctionValue(first.GetVariableValue());
			if (attribute.IsError())
			{
				SN::SN_Error e1 = attribute.GetSNI_Error();
				SNI_CallRecord* callRecord = new SNI_CallRecord("Or of first parameter.", this);
				e1.GetSNI_Error()->AddNote(callRecord);
				return e1;
			}
			SN::SN_Expression* firstParamList = new SN::SN_Expression[2];
			firstParamList[0] = p_ParameterList[PU2_Result];
			firstParamList[1] = attribute;
			SN::SN_Error e2 = skynet::Same.GetSNI_FunctionDef()->UnifyArray(firstParamList, p_Source).GetError();
			if (e2.IsSignificantError())
			{
				SNI_CallRecord* callRecord = new SNI_CallRecord("Or of first parameter.", this);
				e2.GetSNI_Error()->AddNote(callRecord);
			}
			delete[] firstParamList;
			Breakpoint(SN::DebugStop, SN::LeftId, GetTypeName(), "Unify before evaluating parameter", p_Source, SN::CallPoint);
			SNI_Frame::Pop();
			return e2;
		}
		SNI_Thread::GetThread()->GetProcessor()->Delay(SN::SN_FunctionDef(dynamic_cast<SNI_FunctionDef*>(this)), GetNumParameters(), p_ParameterList, p_Source);
		SNI_Frame::Pop();
		return skynet::OK;
	}
}
