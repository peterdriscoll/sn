#include "sni_concat.h"

#include "sni_delayedprocessor.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Concat::SNI_Concat()
	{
	}

	SNI_Concat::~SNI_Concat()
	{
	}

	std::string SNI_Concat::GetTypeName() const
	{
		return "Concat";
	}

	std::string SNI_Concat::DisplayCpp() const
	{
		return "SN::Concat";
	}

	std::string SNI_Concat::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "(+)";
	}

	long SNI_Concat::GetPriority() const
	{
		return 1;
	}

	/// @endcond

	SN::SN_Value SNI_Concat::PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Left.GetSNI_Value()->DoConcat(p_Right.GetSNI_Value());
	}

	SN::SN_Value SNI_Concat::LeftInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Left) const
	{
		return p_Result.GetSNI_Value()->DoSubtractLeft(p_Left.GetSNI_Value());
	}

	SN::SN_Expression SNI_Concat::LeftInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Left) const
	{
		return skynet::SubtractLeft.PrimaryFunctionExpression(p_Result, p_Left);
	}

	SN::SN_Value SNI_Concat::RightInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Right) const
	{
		return p_Result.GetSNI_Value()->DoSubtractRight(p_Right.GetSNI_Value());
	}

	SN::SN_Expression SNI_Concat::RightInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Right) const
	{
		return skynet::SubtractRight.PrimaryFunctionExpression(p_Result, p_Right);
	}
}
