#if !defined(SNI_COMPOSE_H_INCLUDED)
#define SNI_COMPOSE_H_INCLUDED

#pragma once

#include "sni_binary.h"

namespace SNI
{
	class SNI_Compose : public SNI_Binary
	{
		PGC_CLASS(SNI_Compose)
	public:
		SNI_Compose();
		virtual ~SNI_Compose();

		/// @cond
		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;
		virtual string GetOperator() const;
		/// @endcond

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Expression PrimaryFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const;
		virtual SN::SN_Value LeftInverseFunctionValue(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right) const;
		virtual SN::SN_Expression LeftInverseFunctionExpression(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const;
		virtual SN::SN_Value RightInverseFunctionValue(const SN::SN_Value & p_Left, const SN::SN_Value & p_Right) const;
		virtual SN::SN_Expression RightInverseFunctionExpression(const SN::SN_Expression & p_Left, const SN::SN_Expression & p_Right) const;
	};
}

#endif // !defined(SNI_COMPOSE_H_INCLUDED)
