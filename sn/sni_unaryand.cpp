#include "sni_unaryand.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_UnaryAnd::SNI_UnaryAnd()
	{
	}

	SNI_UnaryAnd::~SNI_UnaryAnd()
	{
	}

	string SNI_UnaryAnd::GetTypeName() const
	{
		return "UnaryAnd";
	}

	string SNI_UnaryAnd::DisplayCpp() const
	{
		return "SN::UnaryAnd";
	}

	string SNI_UnaryAnd::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "UnaryAnd";
	}

	long SNI_UnaryAnd::GetPriority() const
	{
		return 4;
	}

	bool SNI_UnaryAnd::IgnoreNoConstraint() const
	{
		return true;
	}

	bool SNI_UnaryAnd::AllowDelay() const
	{
		return false;
	}

	SN::SN_Value SNI_UnaryAnd::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoUnaryAnd();
	}

	SN::SN_Expression SNI_UnaryAnd::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.UnaryAnd();
	}

	SN::SN_Value SNI_UnaryAnd::InverseFunctionValue(const SN::SN_Value & p_Param) const
	{
		return p_Param.GetSNI_Value()->DoUnaryOr();
	}

	SN::SN_Expression SNI_UnaryAnd::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return p_Param.UnaryAnd();
	}

	size_t SNI_UnaryAnd::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (p_TotalCalc <= 1)
		{
			if (p_ParamList[PU1_Result].IsKnownValue())
			{
				if (p_ParamList[PU1_Result].AllValuesEqual(skynet::True))
				{
					if (!p_ParamList[PU1_First].IsKnownValue())
					{
						return p_ParamList[PU1_Result].Cardinality();
					}
				}
				if (p_ParamList[PU1_Result].AllValuesEqual(skynet::False))
				{
					return 0;
				}
			}
			if (p_ParamList[PU1_First].IsKnownValue())
			{
				if (p_ParamList[PU1_First].AllValuesEqual(skynet::False))
				{
					if (!p_ParamList[PU1_Result].IsKnownValue())
					{
						return p_ParamList[PU1_First].Cardinality();
					}
				}
				if (p_ParamList[PU1_First].AllValuesEqual(skynet::True))
				{
					return 0;
				}
			}
			return MultiplyCardinality(p_ParamList[PU1_First].Cardinality(), p_ParamList[PU1_Result].Cardinality());
		}
		return CARDINALITY_MAX;
	}
}
