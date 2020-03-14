#include "sni_stringref.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sn.h"

#include "sni_delayedprocessor.h"
#include "sni_string.h"
#include "sni_char.h"
#include "sni_real.h"
#include "sni_error.h"
#include "sni_null.h"
#include "sni_valueset.h"
#include "sni_world.h"
#include "sni_worldset.h"
#include "sni_functiondef.h"
#include "sni_helpers.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ SNI_Class *SNI_StringRef::m_Class = NULL;

	/*static*/ SNI_Class *SNI_StringRef::Class()
	{
		if (!m_Class)
		{
			m_Class = new SNI_Class("StringRef");
		}
		return m_Class;
	}

	SNI_Class *SNI_StringRef::VClass()
	{
		return Class();
	}

	SNI_StringRef::SNI_StringRef()
		: m_WorldSet(NULL)
	{
	}

	SNI_StringRef::SNI_StringRef(const SNI_StringRef &p_Other)
		: m_WorldSet(p_Other.m_WorldSet)
		, m_Source(p_Other.m_Source)
		, m_Start(p_Other.m_Start.GetSNI_Expression()->Copy())
		, m_End(p_Other.m_End.GetSNI_Expression()->Copy())
	{
	}

	SNI_StringRef::SNI_StringRef(const SN::SN_Value & p_Source, const SN::SN_Expression & p_Start, const SN::SN_Expression & p_End)
		: m_WorldSet(NULL)
	{
		ASSERTM(p_Source.GetSNI_StringRef() != this, "String ref cannot point to itself.");
		m_Source = p_Source;
		m_Start = p_Start;
		m_End = p_End;
		REQUESTPROMOTION(m_Source.GetSNI_ExpressionRef());
		REQUESTPROMOTION(m_Start.GetSNI_ExpressionRef());
		REQUESTPROMOTION(m_End);
	}

	SNI_StringRef::~SNI_StringRef()
	{
	}

	SNI_Expression *SNI_StringRef::Copy() const
	{
		return new SNI_StringRef(*this);
	}

	//-----------------------------------------------------------------------
	// Logging
	//-----------------------------------------------------------------------

	string SNI_StringRef::GetTypeName() const
	{
		return "StringRef";
	}

	string SNI_StringRef::DisplayCpp() const
	{
		if (IsNull())
		{
			const string & source = GetSourceString();
			SN::SN_Expression end = GetEnd().GetVariableValue();
			SN::SN_Long l_long = end.DoEvaluate();
			if (l_long.IsNull())
			{
				end = GetEnd();
			}
			else if (l_long.GetNumber() == -1)
			{
				end = SN::SN_Long((long) source.length());
			}
			return "\"" + EscapeStringToCPP(source) + "\", " + GetStart().DisplayCpp() + ", " + end.DisplayCpp();
		}
		return "\"" + EscapeStringToCPP(GetString()) + "\"";
	}

	string SNI_StringRef::DisplaySN(long /*priority*/, SNI_DisplayOptions & p_DisplayOptions) const
	{
		if (IsKnownValue() && Cardinality() == 1)
		{
			return "\"" + EscapeStringToCPP(GetSourceString().substr(GetLeftMostPos(), Length())) + "\"";
		}
		SN::SN_Long longStart = GetStart().GetVariableValue();
		if (!longStart.IsNull())
		{
			long startPos = longStart.GetNumber();
			return "\"" + EscapeStringToCPP(GetSourceString().substr(startPos)) + "\"[0.." + GetEnd().GetSNI_Expression()->DisplayValueSN(GetPriority(), p_DisplayOptions) + "-" + to_string(startPos) + "]";
		}
		SN::SN_Long longEnd = GetEnd().GetVariableValue();
		if (!longEnd.IsNull())
		{
			long endPos = longEnd.GetNumber();
			if (endPos != -1)
			{
				return "\"" + EscapeStringToCPP(GetSourceString().substr(0, endPos)) + "\"[" + GetStart().GetSNI_Expression()->DisplayValueSN(GetPriority(), p_DisplayOptions) + "..]";
			}
		}
		return "\"" + EscapeStringToCPP(GetSourceString()) + "\"[" + GetStart().GetSNI_Expression()->DisplayValueSN(GetPriority(), p_DisplayOptions) + ".." + GetEnd().GetSNI_Expression()->DisplayValueSN(GetPriority(), p_DisplayOptions) + "]";
	}

	bool SNI_StringRef::IsKnownTypeValue() const
	{
		return true;
	}

	size_t SNI_StringRef::Cardinality(size_t p_MaxCardinality) const
	{
		if (IsNull())
		{
			return 1; // Slightly dubious. Needed for TestStringRefDefinition.
		}
		return SNI_FunctionDef::MultiplyCardinality(GetStart().Cardinality(p_MaxCardinality), GetEnd().Cardinality(p_MaxCardinality));
	}

	size_t SNI_StringRef::LeftCardinality(size_t p_MaxCardinality) const
	{
		if (IsNull())
		{
			return 1; // Slightly dubious. Needed for TestStringRefDefinition.
		}
		return GetStart().Cardinality(p_MaxCardinality);
	}

	size_t SNI_StringRef::RightCardinality(size_t p_MaxCardinality) const
	{
		if (IsNull())
		{
			return 1; // Slightly dubious. Needed for TestStringRefDefinition.
		}
		return GetEnd().Cardinality(p_MaxCardinality);
	}

	SN::SN_Error SNI_StringRef::ForEachCart(long p_Depth, SNI_Cart *p_Cart)
	{
		SN::SN_Expression start = GetStart().DoEvaluate();
		SN::SN_Expression end = GetEnd().DoEvaluate();
		if (Cardinality() == 1)
		{
			return p_Cart->ProcessValue(p_Depth, this, NULL);
		}
		else
		{
			ASSERTM(false, "Shouldn't be here. Value should be already simplified. See SimplifyValue.");
			const string &source_text = GetSourceString();
			SN::SN_String source = m_Source;
			SNI_WorldSet *worldSet = new SNI_WorldSet();
			return start.ForEach(
				[&end, &source, &source_text, &p_Cart, worldSet, p_Depth](const SN::SN_Expression &p_Param, SNI::SNI_World *p_World) -> SN::SN_Error
			{
				SN::SN_Expression start_exp = p_Param;
				SNI::SNI_World *startWorld = p_World;
				return end.ForEach(
					[&source, &source_text, &start_exp, &p_Cart, startWorld, worldSet, p_Depth](const SN::SN_Expression &p_Param, SNI::SNI_World *p_World) -> SN::SN_Error
				{
					SN::SN_Expression end_exp = p_Param;
					SN::SN_Long start_long = start_exp;
					SN::SN_Long end_long = end_exp;
					SNI::SNI_World *endWorld = p_World;
					SN::SN_Value value;
					if (!start_long.IsNullValue() && !end_long.IsNullValue())
					{
						size_t start_pos = start_long.GetNumber();
						size_t end_pos = end_long.GetNumber();
						string s = source_text.substr(start_pos, end_pos - start_pos);
						value = SN::SN_String(s);
					}
					else
					{
						value = SN::SN_StringRef(source, start_exp, end_exp);
					}
					bool exists = false;
					SNI_World *world = worldSet->JoinWorldsArgs(AutoAddWorld, CreateIfActiveParents, exists, startWorld, endWorld);
					if (exists)
					{
						return p_Cart->ProcessValue(p_Depth, value, world);
					}
					return skynet::OK;
				});
			});
			worldSet->Complete();
		}
	}

	SN::SN_Error SNI_StringRef::ForEach(std::function<SN::SN_Error(const SN::SN_Expression &p_Param, SNI_World *p_World)> p_Action)
	{
		return p_Action(this, NULL);
		SN::SN_Expression start = GetStart().DoEvaluate();
		SN::SN_Expression end = GetEnd().DoEvaluate();
		if (Cardinality() == 1)
		{
			return p_Action(this, NULL);
		}
		else
		{
			ASSERTM(false, "Shouldn't be here. Value should be already simplified. See SimplifyValue.");

			// If you resurrect this, fix error handling. See SNI_ValueSet::ForEach.
			const string &source_text = GetSourceString();
			SN::SN_String source = m_Source;
			SNI_WorldSet *worldSet = new SNI_WorldSet();
			return start.ForEach(
				[&end, &source, &source_text, &p_Action, worldSet](const SN::SN_Expression &p_Param, SNI::SNI_World *p_World) -> SN::SN_Error
			{
				SN::SN_Expression start_exp = p_Param;
				SNI::SNI_World *startWorld = p_World;
				return end.ForEach(
					[&source, &source_text, &start_exp, &p_Action, startWorld, worldSet](const SN::SN_Expression &p_Param, SNI::SNI_World *p_World) -> SN::SN_Error
				{
					SN::SN_Expression end_exp = p_Param;
					SN::SN_Long start_long = start_exp;
					SN::SN_Long end_long = end_exp;
					SNI::SNI_World *endWorld = p_World;
					SN::SN_Value value;
					if (!start_long.IsNullValue() && !end_long.IsNullValue())
					{
						size_t start_pos = start_long.GetNumber();
						size_t end_pos = end_long.GetNumber();
						string s = source_text.substr(start_pos, end_pos - start_pos);
						value = SN::SN_String(s);
					}
					else
					{
						value = SN::SN_StringRef(source, start_exp, end_exp);
					}
					bool exists = false;
					SNI_World *world = worldSet->JoinWorldsArgs(AutoAddWorld, CreateIfActiveParents, exists, startWorld, endWorld);
					if (exists)
					{
						return p_Action(value, world);
					}
					return skynet::OK;
				});
				return skynet::OK;
			});
			worldSet->Complete();
		}
	}

	SN::SN_Expression SNI_StringRef::SimplifyValue()
	{
		if (  (GetStart().Cardinality() == 1 || GetStart().IsNullValue())
		   && (GetEnd().Cardinality() == 1 || GetEnd().IsNullValue()))
		{
			return this;
		}
		else
		{
			SN::SN_ValueSet valueSet;
			SNI_WorldSet *worldSet = valueSet.GetWorldSet();
			SN::SN_Expression start = GetStart().DoEvaluate();
			SN::SN_Expression end = GetEnd().DoEvaluate();
			const string &source_text = GetSourceString();
			SN::SN_String source = m_Source;

			start.ForEach(
				[&end, &source, &source_text, &valueSet, worldSet](const SN::SN_Expression &p_Param, SNI::SNI_World *p_World) -> SN::SN_Error
			{
				SN::SN_Expression start_exp = p_Param;
				SNI::SNI_World *startWorld = p_World;
				end.ForEach(
					[&start_exp, &source, &source_text, startWorld, &valueSet, worldSet](const SN::SN_Expression &p_Param, SNI::SNI_World *p_World) -> SN::SN_Error
				{
					SN::SN_Expression end_exp = p_Param;
					SN::SN_Long start_long = start_exp;
					SN::SN_Long end_long = end_exp;
					SNI::SNI_World *endWorld = p_World;
					SN::SN_Value value;
					if (!start_long.IsNullValue() && !end_long.IsNullValue())
					{
						size_t start_pos = start_long.GetNumber();
						size_t end_pos = end_long.GetNumber();
						string s = source_text.substr(start_pos, end_pos - start_pos);
						value = SN::SN_String(s);
					}
					else
					{
						value = SN::SN_StringRef(source, start_exp, end_exp);
					}
					bool exists = false;
					SNI_World *world = worldSet->JoinWorldsArgs(AutoAddWorld, CreateIfActiveParentsIgnoreContext, exists, startWorld, endWorld);
					if (exists)
					{
						valueSet.AddTaggedValue(value, world);
					}
					return skynet::OK;
				});
				return skynet::OK;
			});
			worldSet->Complete();
			return valueSet;
		}
	}

	long SNI_StringRef::GetPriority() const
	{
		return 100;
	}
	//-----------------------------------------------------------------------
	// Members
	//-----------------------------------------------------------------------

	bool SNI_StringRef::IsNull() const
	{
		return m_Start.IsNullValue() || m_End.IsNullValue();
		if (!SN::SN_Transaction::InWebServer())
		{
			SN::SN_Value start_value = m_Start.DoEvaluate();
			SN::SN_Value end_value = m_End.DoEvaluate();
			return start_value.IsNullValue() || end_value.IsNullValue();
		}
		else
		{
			return m_Start.IsNullValue() || m_End.IsNullValue();
		}
	}

	bool SNI_StringRef::IsNullValue() const
	{
		return IsNull();
	}

	bool SNI_StringRef::IsKnownValue() const
	{
		if (!SN::SN_Transaction::InWebServer())
		{
			SN::SN_Value start_value = m_Start.DoEvaluate();
			SN::SN_Value end_value = m_End.DoEvaluate();
			return start_value.IsKnownValue() && end_value.IsKnownValue();
		}
		else
		{
			return m_Start.IsKnownValue() && m_End.IsKnownValue();
		}
	}


	bool SNI_StringRef::IsLeftKnownValue() const
	{
		SN::SN_Value start_value = m_Start.DoEvaluate();
		return start_value.IsKnownValue();
	}

	bool SNI_StringRef::IsRightKnownValue() const
	{
		SN::SN_Value end_value = m_End.DoEvaluate();
		return end_value.IsKnownValue();
	}

	bool SNI_StringRef::IsReferableValue() const
	{
		return true;
	}

	SNI_WorldSet * SNI_StringRef::GetWorldSet()
	{
		if (!m_WorldSet)
		{
			m_WorldSet = new SNI_WorldSet();
			REQUESTPROMOTION(m_WorldSet);
		}
		return m_WorldSet;
	}

	void SNI_StringRef::PromoteMembers()
	{
		REQUESTPROMOTION(m_Source.GetSNI_ExpressionRef());
		REQUESTPROMOTION(m_Start.GetSNI_ExpressionRef());
		REQUESTPROMOTION(m_End);
	}

	SN::SN_String SNI_StringRef::GetSource() const
	{
		return m_Source.GetSNI_String()->GetSource();
	}

	const string &SNI_StringRef::GetSourceString() const
	{
		SNI_StringRef *source = m_Source.GetSNI_StringRef();
		SNI_String *s = m_Source.GetSNI_String();
		while (source)
		{
			s = source->m_Source.GetSNI_String();
			source = source->m_Source.GetSNI_StringRef();
		}
		return s->GetSourceString();
	}

	long SNI_StringRef::GetLeftMostPos() const
	{
		SN::SN_Long start = m_Start.DoEvaluate();
		if (start.IsNullValue())
		{
			return m_Source.GetSNI_String()->GetLeftMostPos();
		}
		return start.GetNumber();
	}

	long SNI_StringRef::GetRightMostPos() const
	{
		SN::SN_Long end = m_End.DoEvaluate();
		if (end.IsNullValue())
		{
			return m_Source.GetSNI_String()->GetRightMostPos();
		}
		return end.GetNumber();
	}

	SN::SN_Expression SNI_StringRef::GetStart() const
	{
		return m_Start;
	}

	SN::SN_Expression SNI_StringRef::GetEnd() const
	{
		return m_End;
	}

	string SNI_StringRef::GetString() const
	{
		SN::SN_Value start = m_Start.DoEvaluate();
		if (start.IsNullValue())
		{
			return "";
		}
		long start_pos = SN::SN_Long(start).GetNumber();
		SN::SN_Value end = m_End.DoEvaluate();
		if (end.IsNullValue())
		{
			return "";
		}
		long end_pos = SN::SN_Long(end).GetNumber();
		return GetSourceString().substr(start_pos, end_pos - start_pos);
	}

	size_t SNI_StringRef::Length() const
	{
		SN::SN_Value start = m_Start.DoEvaluate();
		if (start.IsNullValue())
		{
			return 0;
		}
		long start_pos = SN::SN_Long(start).GetNumber();
		SN::SN_Value end = m_End.DoEvaluate();
		if (end.IsNullValue())
		{
			return 0;
		}
		long end_pos = SN::SN_Long(end).GetNumber();
		return end_pos - start_pos;
	}

	//-----------------------------------------------------------------------
	// Base
	//-----------------------------------------------------------------------

	SN::SN_Error SNI_StringRef::AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World ** p_World, SNI_WorldSet *p_WorldSet)
	{
		bool exists = true;
		// SNI_World *world = p_WorldSet->JoinWorlds(ManualAddWorld, AlwaysCreateWorld,, exists, p_NumWorlds, p_World);
		if (exists)
		{ // Multiple values?
			return AssertValue(p_Value);
		}
		else
		{
			return SN::SN_Error(false, false, "SNI_StringRef::AddValue: Value conflict.");
		}
	}

	SN::SN_Error SNI_StringRef::AssertValue(const SN::SN_Expression &p_Value)
	{
		if (p_Value.IsError())
		{
			return p_Value.GetError();
		}
		string part;
		bool found = false;
		if (SN::Is<SNI_String *>(p_Value))
		{
			SN::SN_String text = p_Value;
			part = text.GetString();
			found = true;
		}
		else if (SN::Is<SNI_StringRef *>(p_Value))
		{
			SN::SN_StringRef text = p_Value;
			if (text.IsKnownValue())
			{
				part = text.GetString();
				found = true;
			}
		}
		else if (SN::Is<SNI_Char *>(p_Value))
		{
			SN::SN_Char text = p_Value;
			part = text.GetString();
			found = true;
		}
		if (found)
		{
			long part_len = (long) part.length();

			SN::SN_Value start = m_Start.DoEvaluate();
			if (start.IsNull())
			{
				return SN::SN_Error(false, false, "SNI_StringRef.AssertValue: Start is null.");
			}

			long start_pos = SN::SN_Long(start).GetNumber();
			if (start_pos + part_len > GetRightMostPos())
			{
				return SN::SN_Error(false, false, "SNI_StringRef.AssertValue: String too long.");
			}
			if (GetSourceString().substr(start_pos, part_len) == part)
			{
				return m_End.AssertValue(SN::SN_Long(start_pos + part_len));
			}
		}
		else if (SN::Is<SNI_ValueSet *>(p_Value))
		{
			SN::SN_ValueSet valueSet(p_Value);
			return valueSet.AssertValue(this);
		}
		else if (p_Value.IsVariable())
		{
			SN::SN_Variable variable(p_Value);
			return variable.AssertValue(this);
		}
		return SN::SN_Error(false, false, "SNI_StringRef.AssertValue: Cannot assert equal.");
	}

	SN::SN_Expression SNI_StringRef::DoEvaluate(long p_MetaLevel /* = 0 */) const
	{
		SN::SN_Value start = m_Start.DoEvaluate(p_MetaLevel);
		if (start.IsNull())
		{
			return this;
		}
		if (!SN::Is<SNI_Long *>(start))
		{
			return this;
		}

		long start_pos = SN::SN_Long(start).GetNumber();

		SN::SN_Value end = m_End.DoEvaluate(p_MetaLevel);
		if (end.IsNull())
		{
			return this;
		}
		if (!SN::Is<SNI_Long *>(end))
		{
			return this;
		}

		long end_pos = SN::SN_Long(end).GetNumber();
		return SN::SN_String(m_Source.GetString().substr(start_pos, end_pos - start_pos + 1));
	}

	SN::SN_Expression SNI_StringRef::DoPartialEvaluate(long p_MetaLevel /* = 0 */) const
	{
		return DoEvaluate(p_MetaLevel);
	}

	bool SNI_StringRef::Equivalent(SNI_Object * p_Other) const
	{
		if (dynamic_cast<SNI_StringRef *>(p_Other))
		{
			SN::SN_StringRef other = dynamic_cast<SNI_StringRef *>(p_Other);
			return m_Source.Equivalent(other.GetSource())
				&& m_Start.Equivalent(other.GetStart())
				&& m_End.Equivalent(other.GetEnd());
		}
		else if (dynamic_cast<SNI_String *>(p_Other))
		{
			if (!IsNull())
			{
				SN::SN_String other = dynamic_cast<SNI_String *>(p_Other);
				return GetString() == other.GetString();
			}
		}
		return false;
	}

	size_t SNI_StringRef::Hash() const
	{
		string data = GetString();
		return _Hash_array_representation(data.c_str(), data.size());
	}

	//-----------------------------------------------------------------------
	// Implementation
	//-----------------------------------------------------------------------

	SN::SN_Value SNI_StringRef::DoAdd(SNI_Value * p_Other) const
	{
		return DoConcat(p_Other);
	}

	SN::SN_Value SNI_StringRef::DoConcat(SNI_Value * p_Other) const
	{
		if (dynamic_cast<SNI_StringRef *>(p_Other))
		{
			SN::SN_StringRef other = p_Other;
			if (GetSource().GetSNI_String() == other.GetSource().GetSNI_String())
			{
				SN::SN_Expression start = other.GetStart().DoPartialEvaluate();
				SN::SN_Expression end = other.GetEnd().DoPartialEvaluate();
				if (end.Equivalent(start))
				{
					return SN::SN_StringRef(this, m_Start, other.GetEnd());
				}
			}
			SN::SN_Value this_value = DoEvaluate(0);
			if (SN::Is<SNI_String *>(this_value))
			{
				SN::SN_Value other_value = p_Other->DoEvaluate(0);
				if (SN::Is<SNI_String *>(other_value))
				{
					return this_value.GetSNI_Value()->DoConcat(other_value.GetSNI_Value());
				}
			}
		}
		if (dynamic_cast<SNI_String *>(p_Other))
		{
			SN::SN_Value value = DoEvaluate(0);
			if (SN::Is<SNI_String *>(value))
			{
				return SN::SN_String(value.GetString() + p_Other->GetString());
			}
		}
		if (dynamic_cast<SNI_String *>(p_Other))
		{
			SN::SN_Value value = DoEvaluate(0);
			if (SN::Is<SNI_String *>(value))
			{
				SNI_Char * l_char = dynamic_cast<SNI_Char *>(p_Other);
				return SN::SN_String(value.GetString() + l_char->GetChar());
			}
		}

		return skynet::Null;
	}

	SN::SN_Value SNI_StringRef::DoSubtractLeft(SNI_Value * p_Part) const
	{
		SN::SN_Value part = p_Part->DoEvaluate();
		if (part.IsNull())
		{
			return skynet::Null;
		}
		if (!SN::Is<SNI_String *>(part))
		{
			return SN::SN_Error(false, false, "StringRef.SubtractLeft: Expected a string to subtract: " + DisplayPmExpression(part));
		}
		string part_text = part.GetString();
		long part_len = (long) part_text.length();
		if (part_len == 0)
		{
			return SN::SN_Error(false, false, "StringRef.SubtractLeft: Expected a string to subtract: " + DisplayPmExpression(part));
		}
		SN::SN_Value start = m_Start.DoEvaluate();
		if (!start.IsNull())
		{
			if (!SN::Is<SNI_Long *>(start))
			{
				return SN::SN_Error(false, false, "StringRef.SubtractLeft: Start of string ref must be long in: " + DisplaySN0());
			}
			long start_pos = SN::SN_Long(start).GetNumber();

			SN::SN_Expression end = m_End.DoPartialEvaluate();
			if (SN::Is<SNI_Value *>(end))
			{
				if (!SN::Is<SNI_Long *>(end))
				{
					return SN::SN_Error(false, false, "StringRef.SubtractLeft: End of string ref must be long: " + DisplaySN0());
				}
				SN::SN_Long end = m_End.DoEvaluate();
				if (end.IsNullValue())
				{
					return SN::SN_Error(false, false, "StringRef.SubtractLeft: End is not known: " + DisplaySN0() + " < " + DisplayPmExpression(part));
				}
				long end_pos = end.GetNumber();
				if (start_pos + part_len > end_pos)
				{
					return SN::SN_Error(false, false, "StringRef.SubtractLeft: Not enough characters to subtract: " + DisplaySN0() + " < " + DisplayPmExpression(part));
				}
			}

			string match = GetSourceString().substr(start_pos, part_len);
			if (match != part_text)
			{
				return SN::SN_Error(false, false, "StringRef.SubtractLeft: Subtract string does not match: " + match + " != " + part_text);
			}
			return SN::SN_StringRef(this, SN::SN_Long(start_pos + part_len), end);
		}
		else
		{
			long start_pos = GetLeftMostPos();
			long end_pos = GetRightMostPos();
			if (start_pos + part_len > end_pos)
			{
				return SN::SN_Error(false, false, "StringRef.SubtractLeft: Not enough characters to subtract: " + DisplaySN0() + " < " + DisplayPmExpression(part));
			}
			SN::SN_ValueSet vs_start;
			SN::SN_ValueSet vs_end;
			SNI_WorldSet *ws_start = vs_start.GetWorldSet();
			SNI_WorldSet *ws_end = vs_end.GetWorldSet();
			size_t pos = start_pos;
			size_t find_pos = 0;
			while ((find_pos = GetSourceString().find(part_text, pos)) != string::npos)
			{
				vs_start.AddTaggedValue(SN::SN_Long((long) find_pos), ws_start->CreateWorld());
				vs_end.AddTaggedValue(SN::SN_Long((long) find_pos + part_len), ws_end->CreateWorld());
				pos = find_pos + 1;
			}
			ws_start->Complete();
			ws_end->Complete();
			if (vs_start.Length())
			{
				const_cast<SNI_StringRef *>(this)->m_Start.AssertValue(vs_start.SimplifyValue());
				return SN::SN_StringRef(this, vs_end.SimplifyValue(), m_End);
			}
		}
		return SN::SN_Error(false, false, "StringRef.SubtractLeft: Left string not matched in search: " + DisplaySN0() + " < " + DisplayPmExpression(part));
	}

	SN::SN_Value SNI_StringRef::DoSubtractRight(SNI_Value * p_Part) const
	{
		SN::SN_Value part = p_Part->DoEvaluate();
		if (part.IsNull())
		{
			return skynet::Null;
		}
		if (!SN::Is<SNI_String *>(part))
		{
			return SN::SN_Error(false, false, "SubtractLeft: Expected a string to subtract: " + DisplayPmExpression(part));
		}
		SN::SN_Value start = m_Start.DoPartialEvaluate();
		if (SN::Is<SNI_Value *>(start))
		{
			if (start.IsNull())
			{
				return skynet::Null;
			}
			if (!SN::Is<SNI_Long *>(start))
			{
				return SN::SN_Error(false, false, "SubtractLeft: Start of string ref must be long in: " + DisplaySN0());
			}
		}
		string part_text = part.GetString();
		long part_len = (long) part_text.length();
		if (part_len == 0)
		{
			return SN::SN_Error(false, false, "StringRef.SubtractLeft: Expected a string to subtract: " + DisplayPmExpression(part));
		}
		SN::SN_Value end = m_End.DoEvaluate();
		if (!end.IsNull())
		{
			if (!SN::Is<SNI_Long *>(end))
			{
				return SN::SN_Error(false, false, "SubtractLeft: End of string ref must be long in: " + DisplaySN0());
			}

			long end_pos = GetRightMostPos();

			string match = GetSourceString().substr(end_pos - part_len, part_len);
			if (match != part_text)
			{
				return SN::SN_Error(false, false, "SubtractLeft: Subtract string does not match: " + match + " != " + part_text);
			}
			return SN::SN_StringRef(this, start, SN::SN_Long(end_pos - part_len));
		}
		else
		{
			size_t start_pos = GetLeftMostPos();
			size_t end_pos = GetRightMostPos();
			if (start_pos + part_len > end_pos)
			{
				return SN::SN_Error(false, false, "StringRef.SubtractLeft: Not enough characters to subtract: " + DisplaySN0() + " < " + DisplayPmExpression(part));
			}
			SN::SN_ValueSet vs_start;
			SN::SN_ValueSet vs_end;
			SNI_WorldSet *worldSet = new SNI_WorldSet();
			vs_start.SetWorldSet(worldSet);
			vs_end.SetWorldSet(worldSet);
			size_t pos = start_pos;
			size_t find_pos = 0;
			while (pos < end_pos && (find_pos = GetSourceString().find(part_text, pos)) != string::npos)
			{
				if (find_pos < end_pos)
				{
					SNI_World *world = worldSet->CreateWorld();
					vs_start.AddTaggedValue(SN::SN_Long((long) find_pos), world);
					vs_end.AddTaggedValue(SN::SN_Long((long) find_pos + part_len), world);
				}
				pos = find_pos + 1;
			}
			worldSet->Complete();
			if (vs_start.Length())
			{
				const_cast<SNI_StringRef *>(this)->m_End.AssertValue(vs_end.SimplifyValue());
				return SN::SN_StringRef(this, m_Start, vs_start.SimplifyValue());
			}
		}
		return SN::SN_Error(false, false, "StringRef.SubtractRight: Right string not matched in search: " + DisplaySN0() + " < " + DisplayPmExpression(part));
	}

	SN::SN_Value SNI_StringRef::DoSubtractLeftChar() const
	{
		SN::SN_Value start = m_Start.DoEvaluate();
		if (start.IsNull())
		{
			return skynet::Null;
		}
		if (!SN::Is<SNI_Long *>(start))
		{
			return SN::SN_Error(false, false, "SubtractLeftChar: Start of string ref must be long in: " + DisplaySN0());
		}
		long start_pos = SN::SN_Long(start).GetNumber();
		SN::SN_Expression end = m_End.DoPartialEvaluate();
		if (SN::Is<SNI_Value *>(end))
		{
			if (!SN::Is<SNI_Long *>(end))
			{
				return SN::SN_Error(false, false, "DoSubtractLeftChar: End of string ref must be long: " + DisplaySN0());
			}
			long end_pos = SN::SN_Long(end).GetNumber();
			if (start_pos + 1 > end_pos)
			{
				return SN::SN_Error(false, false, "DoSubtractLeftChar: Not enough characters to subtract one: " + DisplaySN0());
			}
		}
		return SN::SN_StringRef(this, SN::SN_Long(start_pos + 1), end);
	}

	SN::SN_Value SNI_StringRef::DoSubtractRightChar() const
	{
		SN::SN_Expression end = m_End.DoPartialEvaluate();
		if (!SN::Is<SNI_Long *>(end))
		{
			return SN::SN_Error(false, false, "SubtractRightChar: End of string ref must be long: " + DisplaySN0());
		}
		long end_pos = SN::SN_Long(end).GetNumber();
		SN::SN_Value start = m_Start.DoEvaluate();
		if (SN::Is<SNI_Value *>(start))
		{
			if (start.IsNull())
			{
				return skynet::Null;
			}
			if (!SN::Is<SNI_Long *>(start))
			{
				return SN::SN_Error(false, false, "SubtractRightChar: Start of string ref must be long in: " + DisplaySN0());
			}
			long start_pos = SN::SN_Long(start).GetNumber();
			if (start_pos > end_pos - 1)
			{
				return SN::SN_Error(false, false, "SubtractRightChar: Not enough characters to subtract one: " + DisplaySN0());
			}
		}
		return SN::SN_StringRef(this, start, SN::SN_Long(end_pos - 1));
	}

	SN::SN_Value SNI_StringRef::DoSelectLeftChar() const
	{
		SN::SN_Value start = m_Start.DoEvaluate();
		if (start.IsNull())
		{
			return skynet::Null;
		}
		if (!SN::Is<SNI_Long *>(start))
		{
			return SN::SN_Error(false, false, "SelectLeftChar: Start of string ref must be long in: " + DisplaySN0());
		}

		long start_pos = SN::SN_Long(start).GetNumber();

		SN::SN_Expression end = m_End.DoPartialEvaluate();
		if (SN::Is<SNI_Value *>(end))
		{
			if (!SN::Is<SNI_Long *>(end))
			{
				return SN::SN_Error(false, false, "SelectLeftChar: End of string ref must be long: " + DisplaySN0());
			}
			long end_pos = SN::SN_Long(end).GetNumber();
			if (start_pos + 1 > end_pos)
			{
				return SN::SN_Error(false, false, "SelectLeftChar: Not enough characters to select char: " + DisplaySN0());
			}
		}
		return SN::SN_Char(GetSourceString()[start_pos]);
	}

	SN::SN_Value SNI_StringRef::DoSelectRightChar() const
	{
		SN::SN_Expression end = m_End.DoPartialEvaluate();
		if (!SN::Is<SNI_Long *>(end))
		{
			return SN::SN_Error(false, false, "SelectRightChar: End of string ref must be long: " + DisplaySN0());
		}
		long end_pos = SN::SN_Long(end).GetNumber();
		SN::SN_Value start = m_Start.DoEvaluate();
		if (SN::Is<SNI_Value *>(start))
		{
			if (start.IsNull())
			{
				return skynet::Null;
			}
			if (!SN::Is<SNI_Long *>(start))
			{
				return SN::SN_Error(false, false, "SelectRightChar: Start of string ref must be long in: " + DisplaySN0());
			}
			long start_pos = SN::SN_Long(start).GetNumber();
			if (start_pos + 1 > end_pos)
			{
				return SN::SN_Error(false, false, "SelectRightChar: Not enough characters to select char: " + DisplaySN0());
			}
		}
		return SN::SN_Char(GetSourceString()[end_pos - 1]);
	}

	SN::SN_Value SNI_StringRef::DoLookaheadLeft() const
	{
		SN::SN_Expression start = m_Start.DoPartialEvaluate();
		if (!SN::Is<SNI_Long *>(start))
		{
			return SN::SN_String("");
		}
		size_t start_pos = SN::SN_Long(start).GetNumber();
		if (start_pos < GetSourceString().length())
		{
			return SN::SN_Char(GetSourceString()[start_pos]);
		}
		return SN::SN_String("");
	}

	SN::SN_Value SNI_StringRef::DoLookaheadRight() const
	{
		SN::SN_Expression end = m_End.DoPartialEvaluate();
		if (!SN::Is<SNI_Long *>(end))
		{
			return SN::SN_String("");
		}
		size_t end_pos = SN::SN_Long(end).GetNumber();
		if (end_pos < GetSourceString().length())
		{
			return SN::SN_Char(GetSourceString()[end_pos]);
		}
		return SN::SN_String("");
	}

	SN::SN_Value SNI_StringRef::DoLookStringLeft(SNI_Value * p_Other) const
	{
		string other = p_Other->GetString();
		size_t other_length = other.length();
		SN::SN_Expression start = m_Start.DoPartialEvaluate();
		if (!SN::Is<SNI_Long *>(start))
		{
			return SN::SN_String("");
		}
		size_t start_pos = SN::SN_Long(start).GetNumber();
		if (start_pos + other_length <= GetSourceString().length())
		{
			return SN::SN_Bool(GetSourceString().substr(start_pos, other_length) == other);
		}
		return skynet::False;
	}

	SN::SN_Value SNI_StringRef::DoLookStringRight(SNI_Value * p_Other) const
	{
		string other = p_Other->GetString();
		size_t other_length = other.length();
		SN::SN_Expression end = m_End.DoPartialEvaluate();
		if (!SN::Is<SNI_Long *>(end))
		{
			return SN::SN_String("");
		}
		size_t end_pos = SN::SN_Long(end).GetNumber();
		if (end_pos + other_length <= GetSourceString().length())
		{
			return SN::SN_Bool(GetSourceString().substr(end_pos - other_length, other_length) == other);
		}
		return skynet::False;
	}

	SN::SN_Value SNI_StringRef::DoFile() const
	{
		return SN::SN_String("");
	}

	SN::SN_Value SNI_StringRef::DoEquals(SNI_Value *p_Other) const
	{
		SN::SN_Value start = m_Start.DoEvaluate();
		SN::SN_Value end = m_End.DoEvaluate();

		ASSERTM(!start.GetSafeValue().GetSNI_ValueSet(), "If start point is valueset, string ref should have been simplified.");
		ASSERTM(!end.GetSafeValue().GetSNI_ValueSet(), "If end point is valueset, string ref should have been simplified.");

		if (dynamic_cast<SNI_StringRef *>(p_Other))
		{
			if (!start.IsNullValue() && !end.IsNullValue())
			{
				p_Other->DoEquals(SN::SN_String(GetString()).GetSNI_String());
			}
			return skynet::Null;
		}
		if (dynamic_cast<SNI_String *>(p_Other))
		{
			if (!start.IsNullValue() && !end.IsNullValue())
			{
				return SN::SN_Bool(GetString() == p_Other->GetString());
			}
			if (!start.IsNullValue() && m_End.IsVariable() && end.IsNullValue())
			{
				if (SNI_Thread::TopManager()->GetLogicType() == skynet::FailAsNeg)
				{
					ASSERTM(false, "SNI_Equals::CardinalityOfUnify should stop this from being called when using the failure as negation implementation of Collapse.");
					return skynet::Null;
				}
				else
				{
					long start_pos = SN::SN_Long(start).GetNumber();
					string other = p_Other->GetString();
					string source = GetSourceString();
					string sourcePart = source.substr(start_pos, other.length());
					if (sourcePart != other)
					{
						return skynet::False;
					}
					if (SNI_Thread::TopManager()->GetLogicType() == skynet::Pure)
					{
						SN::SN_ValueSet result;
						SNI_WorldSet *ws_result = result.GetWorldSet();
						SNI_World *worldTrue = ws_result->CreateWorld();
						result.AddTaggedValue(skynet::True, worldTrue);
						SNI_World *worldFalse = ws_result->CreateWorld();
						result.AddTaggedValue(skynet::False, worldFalse);
						SN::SN_ValueSet endVS;
						SNI_Frame *topFrame = SNI_Frame::Top();
						SNI_Variable *endParam = topFrame->CreateParameterByName("_string_end", m_End);
						SN::SN_Variable endVar = topFrame->CreateTemporary();
						endVS.SetWorldSet(ws_result);
						endVS.AddTaggedValue(SN::SN_Long((long) (start_pos + other.length())), worldTrue);
						endVS.AddTaggedValue(endVar, worldFalse);
						endVS.GetSNI_ValueSet()->AssignToVariable(m_End.GetSNI_Variable());
						m_End.GetSNI_Variable()->SetValue(endVS);
						ws_result->Complete();
						SN::SN_Expression *l_ParameterList = new SN::SN_Expression[3];
						l_ParameterList[0] = skynet::False;
						l_ParameterList[1] = endVar;
						l_ParameterList[2] = SN::SN_Long((long)(start_pos + other.length()));
						SNI_Thread::GetThread()->GetProcessor()->Delay(skynet::Equals, 3, l_ParameterList, this);
						return result;
					}
					else
					{
						m_End.GetSNI_Variable()->SetValue(SN::SN_Long((long)(start_pos + other.length())));
						return skynet::True;
					}
				}
			}
			if (m_Start.IsVariable() && start.IsNullValue() && !end.IsNullValue())
			{
				if (SNI_Thread::TopManager()->GetLogicType() == skynet::FailAsNeg)
				{
					ASSERTM(false, "SNI_Equals::CardinalityOfUnify should stop this from being called when using the failure as negation implementation of Collapse.");
					return skynet::Null;
				}
				else
				{
					long end_pos = SN::SN_Long(end).GetNumber();
					string source = GetSourceString().substr(end_pos - p_Other->GetString().length());
					if (source != p_Other->GetString())
					{
						return skynet::False;
					}

					if (SNI_Thread::TopManager()->GetLogicType() == skynet::Pure)
					{
						SN::SN_ValueSet result;
						SNI_WorldSet *ws_result = result.GetWorldSet();
						SNI_World *worldTrue = ws_result->CreateWorld();
						result.AddTaggedValue(skynet::True, worldTrue);
						SNI_World *worldFalse = ws_result->CreateWorld();
						result.AddTaggedValue(skynet::False, worldFalse);
						SNI_Frame *topFrame = SNI_Frame::Top();
						SNI_Variable *startParam = topFrame->CreateParameterByName("_string_start", m_End);
						SN::SN_Variable startVar = topFrame->CreateTemporary();
						SN::SN_ValueSet startVS;
						startVS.SetWorldSet(ws_result);
						startVS.AddTaggedValue(SN::SN_Long((long) (end_pos - p_Other->GetString().length())), worldTrue);
						startVS.AddTaggedValue(startVar, worldFalse);
						startVS.GetSNI_ValueSet()->AssignToVariable(m_Start.GetSNI_Variable());
						m_Start.GetSNI_Variable()->SetValue(startVS);
						ws_result->Complete();
						SN::SN_Expression *l_ParamList = new SN::SN_Expression[3];
						l_ParamList[0] = skynet::False;
						l_ParamList[1] = startVar;
						l_ParamList[2] = SN::SN_Long((long)(end_pos - p_Other->GetString().length()));
						SNI_Thread::GetThread()->GetProcessor()->Delay(skynet::Equals, 3, l_ParamList, this);
						return result;
					}
					else
					{
						m_Start.GetSNI_Variable()->SetValue(SN::SN_Long((long)(end_pos - p_Other->GetString().length())));
						return skynet::True;
					}
				}
			}
			return skynet::Null;
		}
		return skynet::False;
	}

	SN::SN_Error SNI_StringRef::DoAssertEqualsValue(SNI_Value * p_Other, SNI_Value * p_Result) const
	{
		SN::SN_Value start = m_Start.DoEvaluate();
		SN::SN_Value end = m_End.DoEvaluate();
		SN::SN_Value result = p_Result;

		ASSERTM(!start.GetSafeValue().GetSNI_ValueSet(), "If start point is valueset, string ref should have been simplified.");
		ASSERTM(!end.GetSafeValue().GetSNI_ValueSet(), "If end point is valueset, string ref should have been simplified.");

		if (dynamic_cast<SNI_StringRef *>(p_Other))
		{
			if (!start.IsNullValue() && !end.IsNullValue())
			{
				return p_Other->DoAssertEqualsValue(SN::SN_String(GetString()).GetSNI_String(), p_Result);
			}
			//	Maybe this could be implemented in the future. It is unclear.
			return SN::SN_Error(false, false, "Not implemented. Trying to compare two string refs with unknown start or end.");
		}
		if (dynamic_cast<SNI_String *>(p_Other))
		{
			if (!start.IsNullValue() && !end.IsNullValue())
			{
				return SN::SN_Error((GetString() == p_Other->GetString()) == result.GetBool(), false);
			}
			if (!start.IsNullValue() && m_End.IsVariable() && end.IsNullValue())
			{
				long start_pos = SN::SN_Long(start).GetNumber();
				string other = p_Other->GetString();
				string source = GetSourceString();
				string sourcePart = source.substr(start_pos, other.length());
				if (sourcePart != other)
				{
					return SN::SN_Error(!result.GetBool(), false);
				}
				else
				{
					if (result.GetBool())
					{
						m_End.GetSNI_Variable()->SetValue(SN::SN_Long((long)(start_pos + other.length())));
						return skynet::OK;
					}
					else
					{
						SN::SN_Expression *l_ParameterList = new SN::SN_Expression[3];
						l_ParameterList[0] = skynet::False;
						l_ParameterList[1] = m_End;
						l_ParameterList[2] = SN::SN_Long((long)(start_pos + other.length()));
						SNI_Thread::GetThread()->GetProcessor()->Delay(skynet::Equals, 3, l_ParameterList, this);
						return skynet::OK;
					}
				}
			}
			if (m_Start.IsVariable() && start.IsNullValue() && !end.IsNullValue())
			{
				long end_pos = SN::SN_Long(end).GetNumber();
				string source = GetSourceString().substr(end_pos - p_Other->GetString().length());
				if (source != p_Other->GetString())
				{
					return SN::SN_Error(!result.GetBool(), false);
				}
				else
				{
					if (result.GetBool())
					{
						m_Start.GetSNI_Variable()->SetValue(SN::SN_Long((long)(end_pos - p_Other->GetString().length())));
						return skynet::OK;
					}
					else
					{
						SN::SN_Expression *l_ParameterList = new SN::SN_Expression[3];
						l_ParameterList[0] = skynet::False;
						l_ParameterList[1] = start;
						l_ParameterList[2] = SN::SN_Long((long)(end_pos - p_Other->GetString().length()));
						SNI_Thread::GetThread()->GetProcessor()->Delay(skynet::Equals, 3, l_ParameterList, this);
						return skynet::OK;
					}
				}
			}
			return skynet::Fail;
		}
		return skynet::OK;
	}

	void SNI_StringRef::SetStart(SN::SN_Expression p_Expression)
	{
		m_Start = p_Expression;
		REQUESTPROMOTION(m_Start.GetSNI_ExpressionRef());
	}

	void SNI_StringRef::SetEnd(SN::SN_Expression p_Expression)
	{
		m_End = p_Expression;
		REQUESTPROMOTION(m_End.GetSNI_ExpressionRef());
	}

	SN::SN_Value SNI_StringRef::DoEscape(enum skynet::EscapeType p_EscapeType) const
	{
		// Simple version, does not cope with value sets.
		switch (p_EscapeType)
		{
		case skynet::CPP:
			return SN::SN_String(EscapeStringToCPP(GetString()));
		case skynet::JSON:
			return SN::SN_String(EscapeStringToJSON(GetString()));
		}
		return SN::SN_Error(false, false, "Bad escape type for escape conversion");
	}

	SN::SN_Value SNI_StringRef::DoUnescape(enum skynet::EscapeType p_EscapeType) const
	{
		// Simple version, does not cope with value sets.
		switch (p_EscapeType)
		{
		case skynet::CPP:
			return SN::SN_String(UnescapeStringToCPPUsingQuoted(GetString()));
		case skynet::JSON:
			return SN::SN_String(UnescapeStringToJSON(GetString()));
		}
		return SN::SN_Error(false, false, "Bad escape type for escape conversion");
	}

	SN::SN_Value SNI_StringRef::DoStringToInt() const
	{
		// Simple version, does not cope with value sets.
		string s = GetString();
		size_t len = s.size();
		if (len < 10)
		{
			return SN::SN_Long(stol(s));
		}
		return SN::SN_LongLong(stoll(s));
	}

	SN::SN_Value SNI_StringRef::DoStringToDouble() const
	{
		// Simple version, does not cope with value sets.
		string s = GetString();
		size_t len = s.size();
		if (len < 18)
		{
			return SN::SN_Double(stod(s));
		}
		return SN::SN_LongDouble(stold(s));
	}
}
