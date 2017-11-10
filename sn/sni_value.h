#if !defined(SNI_VALUE_H_INCLUDED)
#define SNI_VALUE_H_INCLUDED

#pragma once

#include <string>
using namespace std;

#include "sni_expression.h"

namespace SNI
{
	class SNI_Value : public SNI_Expression
	{
		PGC_CLASS(SNI_Value);
	public:
		SNI_Value();
		SNI_Value(const SNI_Value &p_other);
		virtual ~SNI_Value();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_VariablePointerList &p_DisplayVariableList) const;
		virtual string DisplayValueSN(long priority, SNI_VariablePointerList & p_DisplayVariableList) const;
		virtual long GetPriority() const;

		virtual bool IsKnownValue() const;
		virtual bool IsReferableValue() const;
		SN::SN_Expression GetVariableValue(bool p_IfComplete = true);

		virtual size_t Cardinality(size_t p_MaxCardinality = CARDINALITY_MAX) const;

		virtual SN::SN_Error ForEach(function<SN::SN_Error(const SN::SN_Expression &p_Param, SNI_World *p_World)> p_Action);
		virtual void ForEachCall(SNI_Cartesian * p_Cart, long p_Depth);
		virtual SN::SN_Error ForEachCart(long p_Depth, SNI_Cart * p_Cart);
		virtual void ForEachSplit(SNI_Splitter * p_Splitter);

		virtual SN::SN_Error AssertValue(const SN::SN_Expression & p_Value);

		virtual SN::SN_Expression Evaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialEvaluate(long p_MetaLevel) const;

		virtual SN::SN_Value DoOr(SNI_Expression * p_Other) const;
		virtual SN::SN_Value DoUnaryOr() const;
		virtual SN::SN_Value DoBuildSet() const;

	protected:
		virtual void PromoteMembers();
	};

	typedef vector<SNI_Value> SNI_ValueList;
}

#endif // !defined(SNI_VALUE_H_INCLUDED)
