#if !defined(SN_VALUESET_H_INCLUDED)
#define SN_VALUESET_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_expression.h"

#include <string>
#include <vector>
using namespace std;

namespace SNI
{
	class SNI_ValueSet;
	class SNI_Expression;
	class SNI_Variable;
	class SNI_WorldSet;
	class SNI_World;
	class SNI_TaggedValue;
}

namespace SN
{
	class SN_Value;
	class SN_Expression;
	class SN_Error;
	class SN_Null;
	class SN_Meta;

	class SN_EXPORT SN_ValueSet : public SN_Expression
	{
	public:
		SN_ValueSet();

		SN_ValueSet(const SN_Expression &p_other);
		SN_ValueSet(const SN_Value &p_other, long);
		SN_ValueSet(const SN_Value &p_other);
		SN_ValueSet(const SN_Value &p_left, const SN_Value &p_right);

		SN_ValueSet(const SN_Null &p_Null);
		SN_ValueSet(const SN_Error &p_Error);

		SN_ValueSet(const SN_Meta &p_Meta);
		SN_ValueSet(const SN_ValueSet &p_Value, const SN_Value &p_right);
		SN_ValueSet(const vector<SNI::SNI_TaggedValue> &p_other);

		SN_ValueSet(const SNI::SNI_ValueSet *p_other);

		virtual ~SN_ValueSet();

		SN_ValueSet DoRemove(const SN_Value &p_Other);

		size_t Length() const;
		SNI::SNI_TaggedValue &operator[](long p_index);

		
		void Simplify();

		SNI::SNI_ValueSet * GetSNI_ValueSet() const;
		SNI::SNI_WorldSet * GetWorldSet();
		void SetWorldSet(SNI::SNI_WorldSet *p_WorldSet);

		void AddTaggedValue(const SN_Expression &p_Value, SNI::SNI_World *p_World);
		SN_Error AddValue(SN_Expression p_Value, long p_NumWorlds, SNI::SNI_World **p_WorldList, SNI::SNI_WorldSet *p_WorldSet);
		string GenerateTempVariableName();
	};

	typedef vector<SN_ValueSet> SN_ValueSetList;
}

#endif // !defined(SN_VALUESET_H_INCLUDED)
