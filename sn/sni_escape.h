#if !defined(SNI_ESCAPE_H_INCLUDED)
#define SNI_ESCAPE_H_INCLUDED

#pragma once

#include "sni_unary.h"

namespace SNI
{
	class SNI_Escape : public SNI_Unary
	{
		PGC_CLASS(SNI_Escape)
	public:
		SNI_Escape(enum skynet::EscapeType p_EscapeType);
		virtual ~SNI_Escape();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left) const;
		virtual SN::SN_Value InverseFunctionValue(const SN::SN_Value &p_Param) const;
		virtual SN::SN_Expression InverseFunctionExpression(const SN::SN_Expression &p_Param) const;

	private:
		enum skynet::EscapeType m_EscapeType;
	};
}

#endif // !defined(SNI_ESCAPE_H_INCLUDED)
