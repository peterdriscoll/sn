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
		static SNI_Class* Class();
		SNI_Class* VClass();

		SNI_Value();
		SNI_Value(unsigned long p_Id);
		SNI_Value(const SNI_Value &p_other);
		virtual ~SNI_Value();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual string DisplayValueSN(long priority, SNI_DisplayOptions & p_DisplayOptions) const;
		virtual long GetPriority() const;

		virtual bool IsValue() const;
		virtual bool IsKnownValue() const;
		virtual bool IsReferableValue() const;
		SN::SN_Expression GetVariableValue(bool p_IfComplete = true);

		virtual size_t Cardinality(size_t p_MaxCardinality = CARDINALITY_MAX) const;

		virtual SN::SN_Error ForEach(function<SN::SN_Error(const SN::SN_Expression &p_Param, SNI_World *p_World)> p_Action);
		virtual SN::SN_Error ForEachCart(long p_Depth, SNI_Cart * p_Cart);
		virtual void ForEachSplit(SNI_Splitter * p_Splitter);

		virtual bool AllValuesEqual(const SN::SN_Expression & p_Value) const;

		virtual SN::SN_Error AssertValue(const SN::SN_Expression & p_Value);

		virtual SN::SN_Expression DoEvaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Expression DoPartialEvaluate(long p_MetaLevel) const;

		virtual SN::SN_Value DoOr(SNI_Expression * p_Other) const;
		virtual SN::SN_Value DoUnaryOr() const;
		virtual SN::SN_Value DoBuildSet() const;
		
		virtual SN::SN_Error AssertIsAValue(const SNI_Value * p_Parent, SN::SN_Expression p_Result);
		virtual SN::SN_Value DoIsA(const SNI_Value * p_Parent) const;

	private:
		static SNI_Class* m_Class;
	};

	typedef vector<SNI_Value> SNI_ValueList;
}

#endif // !defined(SNI_VALUE_H_INCLUDED)
