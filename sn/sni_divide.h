#if !defined(SNI_DIVIDE_H_INCLUDED)
#define SNI_DIVIDE_H_INCLUDED

#pragma once


#include "sni_binary.h"

namespace SNI
{
	class SNI_Divide : public SNI_Binary
	{
		PGC_CLASS(SNI_Divide)
	public:
		SNI_Divide();
		virtual ~SNI_Divide();

		virtual std::string GetTypeName() const;
		virtual std::string DisplayCpp() const;
		virtual std::string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;
		virtual std::string GetOperator() const;

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Value LeftInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Left) const;
		virtual SN::SN_Expression LeftInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Left) const;
		virtual SN::SN_Value RightInverseFunctionValue(const SN::SN_Value &p_Result, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Expression RightInverseFunctionExpression(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Right) const;
	};
}

#endif // !defined(SNI_REV_DIVIDE_H_INCLUDED)
