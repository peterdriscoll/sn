#if !defined(SNI_STRINGREF_H_INCLUDED)
#define SNI_STRINGREF_H_INCLUDED

#pragma once

#include <string>

#include "sni_string.h"

namespace SN
{
	class SN_Error;
}

namespace SNI
{
	class SNI_WorldSet;

	class SNI_StringRef : public SNI_String
	{
		PGC_CLASS(SNI_StringRef);
	public:
		SNI_StringRef();
		SNI_StringRef(const SN::SN_Value &p_Source, const SN::SN_Expression &p_Start, const SN::SN_Expression &p_End);
		virtual ~SNI_StringRef();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual size_t Cardinality(size_t p_MaxCardinality = CARDINALITY_MAX) const;
		virtual SN::SN_Error ForEachCart(long p_Depth, SNI_Cart * p_Cart);
		virtual SN::SN_Error ForEach(std::function<SN::SN_Error(const SN::SN_Expression&p_Param, SNI_World*p_World)> p_Action);
		virtual string DisplaySN(long, SNI_DisplayOptions &) const;
		virtual long GetPriority() const;

		virtual bool IsNull() const;
		virtual bool IsNullValue() const;
		virtual bool IsKnownValue() const;
		virtual bool IsReferableValue() const;
		virtual SNI_WorldSet * GetWorldSet();

		virtual SN::SN_String GetSource() const;
		virtual	SN::SN_Expression GetStart() const;
		virtual	SN::SN_Expression GetEnd() const;

		virtual const string & GetSourceString() const;
		virtual long GetLeftMostPos() const;
		virtual long GetRightMostPos() const;

		virtual	string GetString() const;
		virtual	size_t Length() const;

		virtual SN::SN_Expression SNI_StringRef::Evaluate(long p_MetaLevel /* = 0 */) const;
		virtual SN::SN_Expression SNI_StringRef::PartialEvaluate(long p_MetaLevel /* = 0 */) const;

		virtual bool Equivalent(SNI_Object * p_Other) const;

		virtual SN::SN_Value DoAdd(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoConcat(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoSubtractLeft(SNI_Value * p_Part) const;
		virtual SN::SN_Value DoSubtractRight(SNI_Value * p_Part) const;
		virtual SN::SN_Value DoSubtractLeftChar() const;
		virtual SN::SN_Value DoSubtractRightChar() const;
		virtual	SN::SN_Value DoSelectLeftChar() const;
		virtual	SN::SN_Value DoSelectRightChar() const;
		virtual	SN::SN_Value DoLookaheadLeft() const;
		virtual	SN::SN_Value DoLookaheadRight() const;
		virtual	SN::SN_Value DoFile() const;
		virtual	SN::SN_Value DoEquals(SNI_Value * p_Other) const;

		virtual SN::SN_Error AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World ** p_World, SNI_WorldSet * p_WorldSet);
		virtual SN::SN_Error AssertValue(const SN::SN_Expression &p_Value);

	protected:
		virtual void PromoteMembers();

	private:
		SNI_WorldSet *m_WorldSet;
		SN::SN_String m_Source;
		SN::SN_Expression m_Start;
		SN::SN_Expression m_End;
	};

	typedef vector<SNI_StringRef> SNI_StringRefList;
}

#endif // !defined(SNI_STRINGREF_H_INCLUDED)
