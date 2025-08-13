#if !defined(SNI_NOT_H_INCLUDED)
#define SNI_NOT_H_INCLUDED

#pragma once

#include "sni_unary.h"

namespace SNI
{
	class SNI_Not : public SNI_Unary
	{
		PGC_CLASS(SNI_Not)
	public:
		SNI_Not();
		virtual ~SNI_Not();

		virtual std::string GetTypeName() const;
		virtual std::string DisplayCpp() const;
		virtual std::string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;
		virtual std::string GetOperator() const;

		SN::SN_Value PrimaryFunctionValue(const SN::SN_Value &p_Param) const;

		SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel) const;
	};
}

#endif // !defined(SNI_NOT_H_INCLUDED)
