#if !defined(SNI_META_H_INCLUDED)
#define SNI_META_H_INCLUDED

#pragma once

#include "sni_value.h"

#include <string>
using namespace std;

namespace SNI
{
	class SNI_Meta : public SNI_Value
	{
		PGC_CLASS(SNI_Meta);
	public:
		static SNI_Class *Class();
		virtual SNI_Class *VClass();

		SNI_Meta();
		SNI_Meta(long p_DeltaMetaLevel, SNI_Expression *p_Expression);
		virtual ~SNI_Meta();

		SNI_Expression * GetExpression();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

		virtual bool IsKnownValue() const;
		virtual size_t Cardinality(size_t p_MaxCardinality) const;

		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual size_t Hash() const;
		virtual SNI_Expression *Clone(long p_MetaLevel, SNI_Frame * p_Frame, bool &);
		virtual SN::SN_Expression DoEvaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Expression DoPartialEvaluate(long p_MetaLevel = 0) const;

		virtual SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Error PartialAssertValue(const SN::SN_Expression &p_Expression, bool p_Define = false);
		virtual SN::SN_Expression Unify(SN::SN_ExpressionList * p_ParameterList);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Expression, bool p_Define = false);

	private:
		static SNI_Class *m_Class;

		long m_DeltaMetaLevel;
		SNI_Expression * m_Expression;
	};
}

#endif // !defined(SNI_META_H_INCLUDED)
