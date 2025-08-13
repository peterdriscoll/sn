#if !defined(SNI_UNESCAPE_H_INCLUDED)
#define SNI_UNESCAPE_H_INCLUDED

#pragma once

#include "sni_unary.h"

namespace SNI
{
	class SNI_Unescape : public SNI_Unary
	{
		PGC_CLASS(SNI_Unescape)
	public:
		SNI_Unescape(enum SN::EscapeType p_EscapeType);
		virtual ~SNI_Unescape();

		virtual std::string GetTypeName() const;
		virtual std::string DisplayCpp() const;
		virtual std::string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

		virtual SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Left) const;
		virtual SN::SN_Value InverseFunctionValue(const SN::SN_Value &p_Param) const;
		virtual SN::SN_Expression InverseFunctionExpression(const SN::SN_Expression &p_Param) const;

	private:
		enum SN::EscapeType m_EscapeType;
	};
}

#endif // !defined(SNI_UNESCAPE_H_INCLUDED)
