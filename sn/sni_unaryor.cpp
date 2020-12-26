#include "sni_unaryor.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_UnaryOr::SNI_UnaryOr()
	{
	}

	SNI_UnaryOr::~SNI_UnaryOr()
	{
	}

	string SNI_UnaryOr::GetTypeName() const
	{
		return "UnaryOr";
	}

	string SNI_UnaryOr::DisplayCpp() const
	{
		return "SN::UnaryOr";
	}

	string SNI_UnaryOr::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "UnaryOr";
	}

	long SNI_UnaryOr::GetPriority() const
	{
		return 4;
	}

	bool SNI_UnaryOr::IgnoreNoConstraint() const
	{
		return true;
	}

	bool SNI_UnaryOr::AllowDelay() const
	{
		return false;
	}

	SN::SN_Value SNI_UnaryOr::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoUnaryOr();
	}

	SN::SN_Value SNI_UnaryOr::InverseFunctionValue(const SN::SN_Value & p_Param) const
	{
		return p_Param.GetSNI_Value()->DoUnaryAnd();
	}

	SN::SN_Expression SNI_UnaryOr::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return skynet::UnaryAnd.PrimaryFunctionExpression(p_Param);
	}

	size_t SNI_UnaryOr::CardinalityOfUnify(long p_Depth, SN::SN_Expression * p_ParamList, long p_CalcPos, long p_TotalCalc) const
	{
		if (p_TotalCalc <= 1)
		{
			if (p_ParamList[PU1_Result].IsKnownValue())
			{
				if (p_ParamList[PU1_Result].AllValuesEqual(skynet::False))
				{
					if (!p_ParamList[PU1_First].IsKnownValue())
					{
						return p_ParamList[PU1_Result].Cardinality();;
					}
				}
				if (p_ParamList[PU1_Result].AllValuesEqual(skynet::True))
				{
					return 0;
				}
			}
			if (p_ParamList[PU1_First].IsKnownValue())
			{
				if (p_ParamList[PU1_First].AllValuesEqual(skynet::True))
				{
					if (!p_ParamList[PU1_Result].IsKnownValue())
					{
						return p_ParamList[PU1_First].Cardinality();
					}
				}
				else if (p_ParamList[PU1_First].AllValuesEqual(skynet::False))
				{
					return 0;
				}
			}
			return MultiplyCardinality(p_ParamList[PU1_First].Cardinality(), p_ParamList[PU1_Result].Cardinality());
		}
		return CARDINALITY_MAX;
	}
}
