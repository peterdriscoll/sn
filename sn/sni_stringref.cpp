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

	SNI_StringRef::SNI_StringRef()
		: m_WorldSet(NULL)
	{
	}

	SNI_StringRef::SNI_StringRef(const SN::SN_Value & p_Source, const SN::SN_Expression & p_Start, const SN::SN_Expression & p_End)
		: m_WorldSet(NULL)
	{
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
			ReplaceAll(source, "\"", "\\\"");
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
			return "\"" + source + "\", " + GetStart().DisplayCpp() + ", " + end.DisplayCpp();
		}
		string quotedString = GetString();
		ReplaceAll(quotedString, "\"", "\\\"");
		return "\"" + quotedString + "\"";
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
			const string &source_text = GetSourceString();
			SN::SN_String source = m_Source;
			SNI_WorldSet *worldSet = new SNI_WorldSet(source);
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
					return true;
				});
			});
			worldSet->Complete();
		}
	}

	SN::SN_Error SNI_StringRef::ForEach(std::function<SN::SN_Error(const SN::SN_Expression &p_Param, SNI_World *p_World)> p_Action)
	{
		SN::SN_Expression start = GetStart().DoEvaluate();
		SN::SN_Expression end = GetEnd().DoEvaluate();
		if (Cardinality() == 1)
		{
			return p_Action(this, NULL);
		}
		else
		{
			const string &source_text = GetSourceString();
			SN::SN_String source = m_Source;
			SNI_WorldSet *worldSet = new SNI_WorldSet(source);
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
					return true;
				});
				return true;
			});
			worldSet->Complete();
		}
	}

	string SNI_StringRef::DisplaySN(long /*priority*/, SNI_DisplayOptions & p_DisplayOptions) const
	{
		if (IsKnownValue())
		{
			return "\"" + GetSourceString() + "\"";
		}
		return "\"" + GetSourceString() + "\"[" + GetStart().GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions) + ".." + GetEnd().GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions) + "]";
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
		SN::SN_Value start_value = m_Start.DoEvaluate();
		SN::SN_Value end_value = m_End.DoEvaluate();
		return start_value.IsNull() || end_value.IsNull();
	}

	bool SNI_StringRef::IsNullValue() const
	{
		return IsNull();
	}

	bool SNI_StringRef::IsKnownValue() const
	{
		SN::SN_Value start_value = m_Start.DoEvaluate();
		SN::SN_Value end_value = m_End.DoEvaluate();
		return start_value.IsKnownValue() && end_value.IsKnownValue();
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
			m_WorldSet = new SNI_WorldSet(SN::SN_StringRef(this));
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
		return m_Source.GetSNI_String()->GetSourceString();
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
			string worldString = DisplayWorlds(p_NumWorlds, p_World);
			return SN::SN_Error("SNI_StringRef::AddValue: Value conflict.");
		}
	}

	SN::SN_Error SNI_StringRef::AssertValue(const SN::SN_Expression &p_Value)
	{
		if (p_Value.IsError())
		{
			return p_Value;
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
				return false;
			}

			long start_pos = SN::SN_Long(start).GetNumber();
			if (start_pos + part_len > GetRightMostPos())
			{
				return SN::SN_Error("SNI_StringRef.AssertValue: String too long.");
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
		return false;
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
			return SN::SN_Error("StringRef.SubtractLeft: Expected a string to subtract: " + DisplayPmExpression(part));
		}
		string part_text = part.GetString();
		long part_len = (long) part_text.length();
		if (part_len == 0)
		{
			return SN::SN_Error("StringRef.SubtractLeft: Expected a string to subtract: " + DisplayPmExpression(part));
		}
		SN::SN_Value start = m_Start.DoEvaluate();
		if (!start.IsNull())
		{
			if (!SN::Is<SNI_Long *>(start))
			{
				return SN::SN_Error("StringRef.SubtractLeft: Start of string ref must be long in: " + DisplaySN0());
			}
			long start_pos = SN::SN_Long(start).GetNumber();

			SN::SN_Expression end = m_End.DoPartialEvaluate();
			if (SN::Is<SNI_Value *>(end))
			{
				if (!SN::Is<SNI_Long *>(end))
				{
					return SN::SN_Error("StringRef.SubtractLeft: End of string ref must be long: " + DisplaySN0());
				}
				SN::SN_Long end = m_End.DoEvaluate();
				if (end.IsNullValue())
				{
					return SN::SN_Error("StringRef.SubtractLeft: End is not known: " + DisplaySN0() + " < " + DisplayPmExpression(part));
				}
				long end_pos = end.GetNumber();
				if (start_pos + part_len > end_pos)
				{
					return SN::SN_Error("StringRef.SubtractLeft: Not enough characters to subtract: " + DisplaySN0() + " < " + DisplayPmExpression(part));
				}
			}

			string match = GetSourceString().substr(start_pos, part_len);
			if (match != part_text)
			{
				return SN::SN_Error("StringRef.SubtractLeft: Subtract string does not match: " + match + " != " + part_text);
			}
			return SN::SN_StringRef(this, SN::SN_Long(start_pos + part_len), end);
		}
		else
		{
			long start_pos = GetLeftMostPos();
			long end_pos = GetRightMostPos();
			if (start_pos + part_len > end_pos)
			{
				return SN::SN_Error("StringRef.SubtractLeft: Not enough characters to subtract: " + DisplaySN0() + " < " + DisplayPmExpression(part));
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
		return SN::SN_Error("StringRef.SubtractLeft: Left string not matched in search: " + DisplaySN0() + " < " + DisplayPmExpression(part));
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
			return SN::SN_Error("SubtractLeft: Expected a string to subtract: " + DisplayPmExpression(part));
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
				return SN::SN_Error("SubtractLeft: Start of string ref must be long in: " + DisplaySN0());
			}
		}
		string part_text = part.GetString();
		long part_len = (long) part_text.length();
		if (part_len == 0)
		{
			return SN::SN_Error("StringRef.SubtractLeft: Expected a string to subtract: " + DisplayPmExpression(part));
		}
		SN::SN_Value end = m_End.DoEvaluate();
		if (!end.IsNull())
		{
			if (!SN::Is<SNI_Long *>(end))
			{
				return SN::SN_Error("SubtractLeft: End of string ref must be long in: " + DisplaySN0());
			}

			long end_pos = GetRightMostPos();

			string match = GetSourceString().substr(end_pos - part_len, part_len);
			if (match != part_text)
			{
				return SN::SN_Error("SubtractLeft: Subtract string does not match: " + match + " != " + part_text);
			}
			return SN::SN_StringRef(this, start, SN::SN_Long(end_pos - part_len));
		}
		else
		{
			size_t start_pos = GetLeftMostPos();
			size_t end_pos = GetRightMostPos();
			if (start_pos + part_len > end_pos)
			{
				return SN::SN_Error("StringRef.SubtractLeft: Not enough characters to subtract: " + DisplaySN0() + " < " + DisplayPmExpression(part));
			}
			SN::SN_ValueSet vs_start;
			SN::SN_ValueSet vs_end;
			SNI_WorldSet *worldSet = new SNI_WorldSet(vs_start);
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
		return SN::SN_Error("StringRef.SubtractRight: Right string not matched in search: " + DisplaySN0() + " < " + DisplayPmExpression(part));
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
			return SN::SN_Error("SubtractLeftChar: Start of string ref must be long in: " + DisplaySN0());
		}
		long start_pos = SN::SN_Long(start).GetNumber();
		SN::SN_Expression end = m_End.DoPartialEvaluate();
		if (SN::Is<SNI_Value *>(end))
		{
			if (!SN::Is<SNI_Long *>(end))
			{
				return SN::SN_Error("DoSubtractLeftChar: End of string ref must be long: " + DisplaySN0());
			}
			long end_pos = SN::SN_Long(end).GetNumber();
			if (start_pos + 1 > end_pos)
			{
				return SN::SN_Error("DoSubtractLeftChar: Not enough characters to subtract one: " + DisplaySN0());
			}
		}
		return SN::SN_StringRef(this, SN::SN_Long(start_pos + 1), end);
	}

	SN::SN_Value SNI_StringRef::DoSubtractRightChar() const
	{
		SN::SN_Expression end = m_End.DoPartialEvaluate();
		if (!SN::Is<SNI_Long *>(end))
		{
			return SN::SN_Error("SubtractRightChar: End of string ref must be long: " + DisplaySN0());
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
				return SN::SN_Error("SubtractRightChar: Start of string ref must be long in: " + DisplaySN0());
			}
			long start_pos = SN::SN_Long(start).GetNumber();
			if (start_pos > end_pos - 1)
			{
				return SN::SN_Error("SubtractRightChar: Not enough characters to subtract one: " + DisplaySN0());
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
			return SN::SN_Error("SelectLeftChar: Start of string ref must be long in: " + DisplaySN0());
		}

		long start_pos = SN::SN_Long(start).GetNumber();

		SN::SN_Expression end = m_End.DoPartialEvaluate();
		if (SN::Is<SNI_Value *>(end))
		{
			if (!SN::Is<SNI_Long *>(end))
			{
				return SN::SN_Error("SelectLeftChar: End of string ref must be long: " + DisplaySN0());
			}
			long end_pos = SN::SN_Long(end).GetNumber();
			if (start_pos + 1 > end_pos)
			{
				return SN::SN_Error("SelectLeftChar: Not enough characters to select char: " + DisplaySN0());
			}
		}
		return SN::SN_Char(GetSourceString()[start_pos]);
	}

	SN::SN_Value SNI_StringRef::DoSelectRightChar() const
	{
		SN::SN_Expression end = m_End.DoPartialEvaluate();
		if (!SN::Is<SNI_Long *>(end))
		{
			return SN::SN_Error("SelectRightChar: End of string ref must be long: " + DisplaySN0());
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
				return SN::SN_Error("SelectRightChar: Start of string ref must be long in: " + DisplaySN0());
			}
			long start_pos = SN::SN_Long(start).GetNumber();
			if (start_pos + 1 > end_pos)
			{
				return SN::SN_Error("SelectRightChar: Not enough characters to select char: " + DisplaySN0());
			}
		}
		return SN::SN_Char(GetSourceString()[end_pos - 1]);
	}

	SN::SN_Value SNI_StringRef::DoLookaheadLeft() const
	{
		SN::SN_Expression start = m_Start.DoPartialEvaluate();
		if (!SN::Is<SNI_Long *>(start))
		{
			return SN::SN_Char(0);
		}
		size_t start_pos = SN::SN_Long(start).GetNumber();
		if (start_pos < GetSourceString().length())
		{
			return SN::SN_Char(GetSourceString()[start_pos]);
		}
		return SN::SN_Char(0);
	}

	SN::SN_Value SNI_StringRef::DoLookaheadRight() const
	{
		SN::SN_Expression end = m_End.DoPartialEvaluate();
		if (!SN::Is<SNI_Long *>(end))
		{
			return SN::SN_Char(0);
		}
		size_t end_pos = SN::SN_Long(end).GetNumber();
		if (end_pos < GetSourceString().length())
		{
			return SN::SN_Char(GetSourceString()[end_pos]);
		}
		return SN::SN_Char(0);
	}

	SN::SN_Value SNI_StringRef::DoFile() const
	{
		return SN::SN_String("");
	}

	SN::SN_Value SNI_StringRef::DoEquals(SNI_Value *p_Other) const
	{
		SN::SN_Value start = m_Start.DoEvaluate();
		SN::SN_Value end = m_End.DoEvaluate();
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
			if (!start.IsNullValue() && end.IsNullValue())
			{
				long start_pos = SN::SN_Long(start).GetNumber();
				string source = m_Source.GetString().substr(start_pos, p_Other->GetString().length());
				if (source != p_Other->GetString())
				{
					return skynet::False;
				}
				SN::SN_ValueSet result;
				SNI_WorldSet *ws_result = result.GetWorldSet();
				SNI_World *worldTrue = ws_result->CreateWorld();
				result.AddTaggedValue(skynet::True, worldTrue);
				SNI_World *worldFalse = ws_result->CreateWorld();
				result.AddTaggedValue(skynet::False, worldFalse);
				SN::SN_ValueSet endVS;
				SN::SN_Variable endVar;
				SN::SN_Expression *l_ParameterList = new SN::SN_Expression[3];
				l_ParameterList[0] = skynet::False;
				l_ParameterList[1] = endVar;
				l_ParameterList[2] = SN::SN_Long((long) (start_pos + p_Other->GetString().length()));
				SNI_DelayedProcessor::GetProcessor()->Delay(skynet::Equals, l_ParameterList);
				endVS.AddTaggedValue(SN::SN_Long((long) (start_pos + p_Other->GetString().length())), worldTrue);
				endVS.AddTaggedValue(endVar, worldFalse);
				const_cast<SNI_StringRef *>(this)->SetEnd(endVS);
				ws_result->Complete();
				return result;
			}
			if (start.IsNullValue() && !end.IsNullValue())
			{
				long end_pos = SN::SN_Long(end).GetNumber();
				string source = m_Source.GetString().substr(end_pos - p_Other->GetString().length());
				if (source != p_Other->GetString())
				{
					return skynet::False;
				}
				SN::SN_ValueSet result;
				SNI_WorldSet *ws_result = result.GetWorldSet();
				SNI_World *worldTrue = ws_result->CreateWorld();
				result.AddTaggedValue(skynet::True, worldTrue);
				SNI_World *worldFalse = ws_result->CreateWorld();
				result.AddTaggedValue(skynet::False, worldFalse);
				SN::SN_Variable startVar;
				SN::SN_Expression *l_ParameterList = new SN::SN_Expression[3];
				l_ParameterList[0] = skynet::False;
				l_ParameterList[1] = startVar;
				l_ParameterList[2] = SN::SN_Long((long) (end_pos - p_Other->GetString().length()));
				SNI_DelayedProcessor::GetProcessor()->Delay(skynet::Equals, l_ParameterList);
				SN::SN_ValueSet startVS;
				startVS.AddTaggedValue(SN::SN_Long((long) (end_pos - p_Other->GetString().length())), worldTrue);
				startVS.AddTaggedValue(startVar, worldFalse);
				const_cast<SNI_StringRef *>(this)->SetStart(startVS);
				ws_result->Complete();
				return result;
			}
			return skynet::Null;
		}
		return skynet::False;
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

	// Inheritance
	SN::SN_Error  SNI_StringRef::AssertIsAValue(const SNI_Value * p_Parent, SN::SN_Expression p_Result)
	{
		return p_Result.AssertValue(Class()->DoIsA(p_Parent));
	}

	SN::SN_Value SNI_StringRef::DoIsA(const SNI_Value * p_Parent) const
	{
		return Class()->DoIsA(p_Parent);
	}

	SN::SN_Value SNI_StringRef::DoStringToInt() const
	{
		string s = GetString();
		size_t len = s.size();
		if (len < 10)
		{
			return SN::SN_Long(stol(s));
		}
		return SN::SN_LongLong(stoll(s));
	}
}
