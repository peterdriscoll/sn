#include "sni_metalevel.h"

#include "sn.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_MetaLevel::SNI_MetaLevel()
	{
	}

	SNI_MetaLevel::~SNI_MetaLevel()
	{
	}

	string SNI_MetaLevel::GetTypeName() const
	{
		return "Meta";
	}

	string SNI_MetaLevel::DisplayCpp() const
	{
		return "Meta";
	}

	string SNI_MetaLevel::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "Meta";
	}

	long SNI_MetaLevel::GetPriority() const
	{
		return 1;
	}

	string SNI_MetaLevel::DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, size_t p_NumParams, SN::SN_Expression * p_ParamList, const SNI_Expression * p_DebugSource) const
	{
		return p_ParamList[PC2_First].GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions) + ".Meta(" + p_ParamList[PC2_Second].GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions) + ")";
	}

	SN::SN_Value SNI_MetaLevel::PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		long metaLevel = 0;
		SN::SN_Short shortValue = p_Right;
		if (!shortValue.IsNull())
		{
			metaLevel = shortValue.GetNumber();
		}
		else
		{
			SN::SN_Long longValue = p_Right;
			if (!longValue.IsNull())
			{
				metaLevel = longValue.GetNumber();
			}
			else
			{
				SN::SN_Short longlongValue = p_Right;
				if (!longlongValue.IsNull())
				{
					metaLevel = longlongValue.GetNumber();
				}
			}
		}
		return p_Left.GetSNI_Value()->DoBuildMeta(metaLevel);
	}

	SN::SN_Value SNI_MetaLevel::LeftInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_MetaLevel::LeftInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return skynet::Null;
	}

	SN::SN_Error SNI_MetaLevel::PartialUnifyInternal(SN::SN_Expression & p_left, SN::SN_Expression & p_right, SN::SN_Expression & p_Result)
	{
		SN::SN_Expression left = p_left.DoPartialEvaluate();
		SN::SN_Expression right = p_right.DoPartialEvaluate();
		if (left.IsKnownValue() && right.IsKnownValue())
		{
			return p_Result.PartialAssertValue(PrimaryFunctionValue(left, right));
		}
		return skynet::Fail;
	}

	SN::SN_Expression SNI_MetaLevel::CallArray(SN::SN_Expression* p_ParamList, long p_MetaLevel, const SNI_Expression* p_Source) const
	{
		return p_ParamList[PC1_First].DoEvaluate();
	}

	SN::SN_Expression SNI_MetaLevel::UnifyArray(SN::SN_Expression * p_ParamList, const SNI_Expression *p_Source)
	{
		SNI_Frame::Push(this, p_ParamList[PU2_Result].GetSNI_Expression());
		SNI_Frame* topFrame = SNI_Frame::Top();
		topFrame->CreateParameter(PU2_First, p_ParamList[PU2_First]);
		topFrame->CreateParameter(PU2_Second, p_ParamList[PU2_Second]);

		Breakpoint(SN::DebugStop, SN::CallId, GetTypeName(), "Unify start", p_Source, SN::CallPoint);

		long metaLevel = 0;
		SN::SN_Short shortValue = p_ParamList[PU2_Second];
		if (!shortValue.IsNull())
		{
			metaLevel = shortValue.GetNumber();
		}
		else
		{
			SN::SN_Long longValue = p_ParamList[PU2_Second];
			if (!longValue.IsNull())
			{
				metaLevel = longValue.GetNumber();
			}
			else
			{
				SN::SN_Short longlongValue = p_ParamList[PU2_Second];
				if (!longlongValue.IsNull())
				{
					metaLevel = longlongValue.GetNumber();
				}
			}
		}
		topFrame->GetVariable(PU2_Second)->SetValue(SN::SN_Long(metaLevel));

		SN::SN_Expression result = p_ParamList[PU2_First].GetVariableValue().DoBuildMeta(metaLevel);

		SN::SN_Error err = p_ParamList[PU2_Result].AssertValue(result);

		topFrame->GetResult()->SetValue(p_ParamList[PU2_Result]);

		Breakpoint(SN::DebugStop, SN::ReturnId, GetTypeName(), "Unify end", p_Source, SN::CallPoint);

		SNI_Frame::Pop();
		return err;
	}
}
