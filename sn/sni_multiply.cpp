#include "sni_multiply.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Multiply::SNI_Multiply()
	{
	}

	SNI_Multiply::~SNI_Multiply()
	{
	}

	/// @cond
	void SNI_Multiply::PromoteMembers()
	{
	}

	string SNI_Multiply::GetTypeName() const
	{
		return "Multiply";
	}

	string SNI_Multiply::DisplayCpp() const
	{
		return "SN::Multiply";
	}

	string SNI_Multiply::DisplaySN(long /*priority*/, SNI_VariablePointerList & /*p_DisplayVariableList*/) const
	{
		return "(*)";
	}

	long SNI_Multiply::GetPriority() const
	{
		return 2;
	}

	string SNI_Multiply::GetOperator() const
	{
		return "*";
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
	SN::SN_Value SNI_Multiply::PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Left.GetSNI_Value()->DoMultiply(p_Right.GetSNI_Value());
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
	SN::SN_Expression SNI_Multiply::PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return p_Left * p_Right;
	}

	/// @brief Implement the LeftInverseFunctionValue inherited from SNI_Binary
	///
	/// Given the result and one of the parameters give a value for the other parameter.
	/// * left = result / right
	/// * right = result / left
	/// 
	/// The common logic for the behaviour of addition is held in SNI_Binary.
	/// This method adapts SNI_Binary to use the addition function.
	///
	/// @param p_Result The result of multiplying left and right
	/// @param p_Right The left/right value
	/// @retval The right/left value.
	SN::SN_Value SNI_Multiply::LeftInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const
	{
		return p_Left.GetSNI_Value()->DoDivide(p_Right.GetSNI_Value());
	}

	/// @brief Implement the LeftInverseFunction inherited from SNI_Binary
	///
	/// Given the result and one of the parameters give an expression for the other parameter.
	/// * left = result / right
	/// * right = result / left
	/// 
	/// The common logic for the behaviour of addition is held in SNI_Binary.
	/// This method adapts SNI_Binary to use the addition function.
	///
	/// @param p_Result The result of multiplying left and right
	/// @param p_Right The left/right expression
	/// @retval The right/left expression.
	SN::SN_Expression SNI_Multiply::LeftInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
	{
		return p_Left / p_Right;
	}
}
