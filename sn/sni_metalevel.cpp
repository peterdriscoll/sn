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
		return "Assert";
	}

	long SNI_MetaLevel::GetPriority() const
	{
		return 1;
	}

	string SNI_MetaLevel::DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, SN::SN_ExpressionList * p_ParameterList, const SNI_Expression * p_DebugSource) const
	{
		if (p_ParameterList->size() == 2)
		{
			return (*p_ParameterList)[1].GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions) + ".Meta(" + (*p_ParameterList)[0].GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions) + ")";
		}
		return "";
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
		return p_Left.GetSNI_Value()->DoMeta(metaLevel);
	}

	SN::SN_Expression SNI_MetaLevel::PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return p_Left.Meta(p_Right);
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
		return false;
	}

	SN::SN_Expression SNI_MetaLevel::CallArray(SN::SN_Expression * p_ParamList, long p_MetaLevel /* = 0 */) const
	{
		return p_ParamList[PC1_First].DoEvaluate();
	}

	SN::SN_Expression SNI_MetaLevel::UnifyArray(SN::SN_Expression * p_ParamList)
	{
		SNI_Thread::GetThread()->SetDebugId("meta");
		SNI_Thread::GetThread()->DebugCommand(SN::StaticPoint, "Meta", SN::CallId);

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
		SN::SN_Error result = p_ParamList[PU2_First].DoEvaluate().DoMeta(metaLevel);

		SNI_Thread::GetThread()->SetDebugId("meta");
		SNI_Thread::GetThread()->DebugCommand(SN::StaticPoint, "Meta", SN::ReturnId);

		return p_ParamList[PU2_Result].AssertValue(result);
	}
}
