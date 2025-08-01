#if !defined(SNI_VALUESET_H_INCLUDED)
#define SNI_VALUESET_H_INCLUDED

#pragma once

#include "sni_value.h"
#include "sni_taggedvalue.h"

#include <string>
using namespace std;

namespace SN
{
	class SN_Expression;
	class SN_Error;
}

namespace SNI
{
	class SNI_Binary;
	class SNI_Unary;

	class SNI_ValueSet : public SNI_Value
	{
		PGC_CLASS(SNI_ValueSet);
	public:
		SNI_ValueSet();
		SNI_ValueSet(const SNI_ValueSet &p_other);
		SNI_ValueSet(const SN::SN_Value &p_Value1);
		SNI_ValueSet(const SN::SN_Value &p_Value1, const SN::SN_Value &p_Value2);

		virtual ~SNI_ValueSet();

		virtual SNI_Expression *Copy() const;

		// Logging
		string GetTypeName() const;
		string DisplayCpp() const;
		string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		long GetPriority() const;

		// Queries
		bool CompareValue(const SN::SN_Value & p_Value) const;

		virtual bool IsValueHolder() const;
		virtual bool IsValueSet() const;
		virtual bool IsString() const;
		virtual bool IsKnownValue() const;
		virtual bool IsLeftKnownValue() const;
		virtual bool IsRightKnownValue() const;
		virtual bool IsReferableValue() const;
		virtual bool IsComplete() const;
		virtual void Complete();

		SN::SN_Expression GetVariableValue(bool p_IfComplete);
		virtual bool AllValuesEqual(const SN::SN_Expression & p_Value) const;
		virtual bool AllValues() const;
		virtual bool AllValuesBoolean() const;
		virtual size_t Length() const;
		SNI_TaggedValue &operator[](size_t p_index);
		SNI_TaggedValue At(const long &p_Value);
		virtual bool GetBool() const;
		void Append(const SN::SN_Value &p_Value);
		virtual bool DoIsEmpty() const;
		void AddTaggedValue(const SN::SN_Expression &p_Value, SNI_World *p_World);
		SN::SN_Error AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World **p_WorldList, SNI_WorldSet *p_WorldSet);
		virtual SN::SN_ValueSet DoRemove(const SN::SN_Value &p_Other);
		virtual SNI_Expression * Clone(long p_MetaLevel, SNI_Frame *p_Frame, bool &p_Changed);
		void AssignToVariable(SNI_Variable * p_Variable);
		virtual void Simplify();
		virtual void Validate();
		string GenerateTempVariableName();
		virtual SN::SN_Expression SimplifyValue();
		virtual SN::SN_Error AssertValue(const SN::SN_Expression &p_Value);
		virtual SNI_WorldSet *GetWorldSet();
		virtual void SetWorldSet(SNI_WorldSet *p_WorldSet);
		virtual void AddVariables(long p_MetaLevel, SNI_VariablePointerMap& p_Map);

		virtual SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long  p_MetaLevel = 0) const;
		virtual SN::SN_Expression Unify(SN::SN_ExpressionList * p_ParameterList);

		virtual SN::SN_Error ForEach(std::function<SN::SN_Error(const SN::SN_Expression&p_Param, SNI_World*p_World)> p_Action);
		virtual SN::SN_Error ForEachCart(long p_Depth, SNI_Cart * p_Cart);
		virtual void ForEachSplit(SNI_Splitter * p_Splitter);

		// Cardinality
		virtual size_t Cardinality(size_t p_MaxCardinality = CARDINALITY_MAX) const;

		// Base    
		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual SN::SN_Expression DoEvaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Expression DoPartialEvaluate(long p_MetaLevel = 0) const;

		// General
		static SN::SN_Value GeneralDoOr(const SNI_Value *p_Left, SNI_Expression *p_Right);

		// Logic
		virtual SN::SN_Value DoCollapse();

		// Logic
		virtual SN::SN_Value DoOr(SNI_Expression * p_Other) const;
		virtual SN::SN_Value DoIf(SNI_Expression * p_PositiveCase, SNI_Expression * p_NegativeCase) const;

		// sets
		virtual SN::SN_Value DoBuildSet() const;

	protected:
		virtual void PromoteMembers();

	private:
		SN::SN_Expression CommonValue();
		virtual bool ExtractBooleanValue(bool & p_Value);
		virtual void RemoveFailedWorlds();
		bool NeedsFlattening();
		void FlattenValueSets();
		void CheckWorldSetConsistency();

		SNI_WorldSet        *m_WorldSet;
		SNI_TaggedValueList m_ValueList;
		SNI_Variable        *m_Variable;

		static long			m_MaxTempNum;
	};
}

#endif // !defined(SNI_VALUESET_H_INCLUDED)
