#if !defined(SNI_NULL_H_INCLUDED)
#define SNI_NULL_H_INCLUDED

#pragma once

#include <string>
using namespace std;

#include "sni_value.h"

namespace SNI
{
	class SNI_Null : public SNI_Value
	{
		PGC_CLASS(SNI_Null);
	public:
		static SNI_Class *m_Class;
		static SNI_Class *Class();
		virtual SNI_Class *VClass();

		SNI_Null();
		SNI_Null(const SNI_Null &p_Other);
		virtual ~SNI_Null();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

		virtual bool Equivalent(SNI_Object * p_Other) const;

		virtual size_t Cardinality(size_t p_MaxCardinality = CARDINALITY_MAX);

		virtual bool IsNull() const;
		virtual bool IsNullValue() const;
		virtual bool IsKnownValue() const;
		virtual bool IsReferableValue() const;

		virtual SN::SN_Error AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World ** p_WorldList, SNI_WorldSet * p_WorldSet);
	};

	typedef vector<SNI_Null> SNI_NullList;
}

#endif // !defined(SNI_NULL_H_INCLUDED)
