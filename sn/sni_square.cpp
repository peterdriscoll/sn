#include "sni_Square.h"

#include "logcontext.h"
#include "sn.h"

#include "sni_value.h"
#include "sni_null.h"
#include "sni_error.h"
#include "sni_helpers.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Square::SNI_Square()
	{
	}

	SNI_Square::~SNI_Square()
	{
	}

	string SNI_Square::GetTypeName() const
	{
		return "Square";
	}

	string SNI_Square::DisplayCpp() const
	{
		return "SN::Square";
	}

	string SNI_Square::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "^-^";
	}

	long SNI_Square::GetPriority() const
	{
		return 0;
	}

	SN::SN_Value SNI_Square::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoSquare();
	}

	SN::SN_Expression SNI_Square::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.Square();
	}

	SN::SN_Value SNI_Square::InverseFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoSquareRoot();
	}

	SN::SN_Expression SNI_Square::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return p_Param.SquareRoot();
	}
}
