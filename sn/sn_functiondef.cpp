#include "sn_functiondef.h"

#include "sn_parameter.h"
#include "sn_error.h"

#include "sni_functiondef.h"
#include "sni_binary.h"
#include "sni_unary.h"
#include "sni_object.h"
#include "sni_value.h"
#include "sni_variable.h"

#include "sn_pch.h"

namespace SN
{
	SN_FunctionDef::SN_FunctionDef(const SN_FunctionDef &p_FunctionDef)
		: m_FunctionDef(p_FunctionDef.m_FunctionDef)
	{
	}

	SN_FunctionDef::SN_FunctionDef(const SNI::SNI_FunctionDef *p_FunctionDef)
		: m_FunctionDef(const_cast<SNI::SNI_FunctionDef *>(p_FunctionDef))
	{
	}

	SN_FunctionDef::~SN_FunctionDef()
	{
	}

	// Comparison
	SN_Expression SN_FunctionDef::operator==(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator==(*this, p_Other);
	}

	SN_Expression SN_FunctionDef::operator!=(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator!=(*this, p_Other);
	}

	SN_Expression SN_FunctionDef::operator()(const SN_Expression &p_Parameter) const
	{
		return SN_Operators::FunctionCall(*this, p_Parameter);
	}

	SN::SN_Expression SN_FunctionDef::operator||(const SN::SN_Expression & p_Other) const
	{
		return SN_Operators::operator ||(*this, p_Other);
	}

	SN::SN_Expression SN_FunctionDef::PrimaryFunctionExpressionOp(const SN::SN_Expression& p_Result) const
	{
		return m_FunctionDef->PrimaryFunctionExpressionOp(p_Result);
	}

	SN::SN_Expression SN_FunctionDef::PrimaryFunctionExpression(const SN::SN_Expression& p_Result) const
	{
		return m_FunctionDef->PrimaryFunctionExpression(p_Result);
	}

	SN::SN_Expression SN_FunctionDef::PrimaryFunctionExpressionOp(const SN::SN_Expression& p_Left, const SN::SN_Expression& p_Right) const
	{
		return m_FunctionDef->PrimaryFunctionExpressionOp(p_Left, p_Right);
	}

	SN::SN_Expression SN_FunctionDef::PrimaryFunctionExpression(const SN::SN_Expression& p_Left, const SN::SN_Expression& p_Right) const
	{
		return m_FunctionDef->PrimaryFunctionExpression(p_Left, p_Right);
	}

	SN::SN_Expression SN_FunctionDef::PrimaryFunctionExpressionOp(const SN::SN_Expression& p_Condition, const SN::SN_Expression& p_PositiveCase, const SN::SN_Expression& p_NegativeCase) const
	{
		return m_FunctionDef->PrimaryFunctionExpressionOp(p_Condition, p_PositiveCase, p_NegativeCase);
	}

	SN::SN_Expression SN_FunctionDef::PrimaryFunctionExpression(const SN::SN_Expression& p_Condition, const SN::SN_Expression& p_PositiveCase, const SN::SN_Expression& p_NegativeCase) const
	{
		return m_FunctionDef->PrimaryFunctionExpression(p_Condition, p_PositiveCase, p_NegativeCase);
	}
	SNI::SNI_FunctionDef * SN_FunctionDef::GetSNI_FunctionDef() const
	{
		return m_FunctionDef;
	}

	SNI::SNI_Binary * SN_FunctionDef::GetSNI_Binary() const
	{
		return dynamic_cast<SNI::SNI_Binary *>(m_FunctionDef);
	}

	SNI::SNI_Unary * SN_FunctionDef::GetSNI_Unary() const
	{
		return dynamic_cast<SNI::SNI_Unary *>(m_FunctionDef);
	}
}
