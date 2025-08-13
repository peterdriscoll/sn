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
		
		static SNI_Class *Class();
		virtual SNI_Class *VClass();

		SNI_StringRef();
		SNI_StringRef(const SNI_StringRef & p_Other);
		SNI_StringRef(const SN::SN_Value &p_Source, const SN::SN_Expression &p_Start, const SN::SN_Expression &p_End);
		virtual ~SNI_StringRef();

		virtual SNI_Expression *Copy() const;

		virtual std::string GetTypeName() const;
		virtual std::string DisplayCpp() const;
		virtual std::string DisplaySN(long, SNI_DisplayOptions &) const;
		virtual void AddVariables(long p_MetaLevel, SNI_VariablePointerMap& p_Map);
		virtual long GetPriority() const;

		virtual size_t Cardinality(size_t p_MaxCardinality = CARDINALITY_MAX) const;
		virtual size_t LeftCardinality(size_t p_MaxCardinality) const;
		virtual size_t RightCardinality(size_t p_MaxCardinality) const;

		virtual SN::SN_Error ForEachCart(long p_Depth, SNI_Cart * p_Cart);
		virtual SN::SN_Error ForEach(std::function<SN::SN_Error(const SN::SN_Expression&p_Param, SNI_World*p_World)> p_Action);
		virtual SN::SN_Expression SimplifyValue();

		virtual bool IsNull() const;
		virtual bool IsNullValue() const;
		virtual bool IsKnownValue() const;
		virtual bool IsKnownTypeValue() const;
		virtual bool IsLeftKnownValue() const;
		virtual bool IsRightKnownValue() const;
		virtual bool IsReferableValue() const;
	private:
		virtual SNI_WorldSet * GetWorldSet();
	public:

		virtual SN::SN_String GetSource() const;
		virtual	SN::SN_Expression GetStart() const;
		virtual	SN::SN_Expression GetEnd() const;

		virtual const std::string & GetSourceString() const;
		virtual long GetLeftMostPos() const;
		virtual long GetRightMostPos() const;

		virtual	std::string GetString() const;
		virtual	size_t Length() const;

		virtual SN::SN_Expression SNI_StringRef::DoEvaluate(long p_MetaLevel /* = 0 */) const;
		virtual SN::SN_Expression SNI_StringRef::DoPartialEvaluate(long p_MetaLevel /* = 0 */) const;

		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual size_t Hash() const;

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
		virtual	SN::SN_Value DoLookStringLeft(SNI_Value * p_Other) const;
		virtual	SN::SN_Value DoLookStringRight(SNI_Value * p_Other) const;
		virtual	SN::SN_Value DoFile() const;
		virtual	SN::SN_Value DoEquals(SNI_Value * p_Other) const;

		virtual SN::SN_Error DoAssertEqualsValue(SNI_Value * p_Other, SNI_Value * p_Result);

		void SetStart(SN::SN_Expression p_Expression);
		void SetEnd(SN::SN_Expression p_Expression);

		virtual SN::SN_Error AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World ** p_World, SNI_WorldSet * p_WorldSet);
		virtual SN::SN_Error AssertValue(const SN::SN_Expression &p_Value);

		virtual SN::SN_Value DoEscape(enum SN::EscapeType p_EscapeType) const;
		virtual SN::SN_Value DoUnescape(enum SN::EscapeType p_EscapeType) const;

		virtual SN::SN_Value DoStringToInt() const;
		virtual SN::SN_Value DoStringToDouble() const;

	protected:
		virtual void PromoteMembers();

	private:
		SN::SN_Error FindString(const std::string& part_text, SN::SN_ValueSet& vs_start, SN::SN_ValueSet& vs_end) const;
		SNI::SNI_String* GetSNI_String() const;

		SNI_WorldSet *m_WorldSet;
		SN::SN_String m_Source;
		SN::SN_Expression m_Start;
		SN::SN_Expression m_End;
	};

	typedef std::vector<SNI_StringRef> SNI_StringRefList;
}

#endif // !defined(SNI_STRINGREF_H_INCLUDED)
