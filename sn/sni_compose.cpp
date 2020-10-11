#include "sni_compose.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Compose::SNI_Compose()
	{
	}

	SNI_Compose::~SNI_Compose()
	{
	}

	string SNI_Compose::GetTypeName() const
	{
		return "Compose";
	}

	string SNI_Compose::DisplayCpp() const
	{
		return "SN::Compose";
	}

	string SNI_Compose::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return ";";
	}

	long SNI_Compose::GetPriority() const
	{
		return 1000;
	}

	string SNI_Compose::GetOperator() const
	{
		return ";";
	}

	/// @endcond

	/// @brief Implement the PrimaryFunctionValue inherited from SNI_Binary
	///
	/// The PrimaryFunctionValue returns a value for a commutative expression on two parameters.
	///
	/// The common logic for the behaviour of addition is held in SNI_Binary.
	/// This method adapts SNI_Binary to use the addition function.
	///
	/// @param p_Left The evaluate value before the *
	/// @param p_Right The evaluate value after the *
	/// @retval The value product of p_Left and p_Right.
	SN::SN_Value SNI_Compose::PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Right;
	}

	/// @brief Implement the PrimaryFunctionExpression inherited from SNI_Binary
	///
	/// The PrimaryFunctionExpression is a commutative expression on two parameters.
	/// * left * right
	///
	/// The common logic for the behaviour of addition is held in SNI_Binary.
	/// This method adapts SNI_Binary to use the addition function.
	///
	/// @param p_Left The partially evaluate expression before the *
	/// @param p_Right The partially evaluate expression after the *
	/// @retval The expression representing the product of p_Left and p_Right.
	SN::SN_Expression SNI_Compose::PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return p_Left.Compose(p_Right);
	}

	SN::SN_Value SNI_Compose::LeftInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_Compose::LeftInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return skynet::Null;
	}

	SN::SN_Value SNI_Compose::RightInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Right;
	}

	SN::SN_Expression SNI_Compose::RightInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return p_Right;
	}
}
