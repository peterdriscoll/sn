#include "sni_valueset.h"

#include "logcontext.h"
#include "sn.h"
#include "sn_cartesian.h"

#include "sni_delayedprocessor.h"
#include "sni_world.h"
#include "sni_worldset.h"
#include "sni_helpers.h"
#include "sni_unary.h"
#include "sni_binary.h"
#include "sni_string.h"
#include "sni_stringref.h"
#include "sni_bool.h"
#include "sni_error.h"
#include "sni_null.h"
#include "utility.h"

#include <vector>
using namespace std;

#include "sn_pch.h"

namespace SNI
{
	/*static*/ long SNI_ValueSet::m_MaxTempNum = 0;

	SNI_ValueSet::SNI_ValueSet()
		: m_WorldSet(NULL)
		, m_Variable(NULL)
	{
	}

	SNI_ValueSet::SNI_ValueSet(const SN::SN_Value &p_Value1, const SN::SN_Value &p_Value2)
		: m_WorldSet(NULL)
	{
		SN::LogContext context("SNI_ValueSet::SNI_ValueSet ( " + p_Value1.DisplaySN() + ", " + p_Value2.DisplaySN() + " )");

		m_ValueList.push_back(SNI_TaggedValue(p_Value1, NULL));
		m_ValueList.push_back(SNI_TaggedValue(p_Value2, NULL));
	}

	SNI_ValueSet::SNI_ValueSet(const SN::SN_Value &p_Value1)
		: m_WorldSet(NULL)
	{
		SN::LogContext context("SNI_ValueSet::SNI_ValueSet ( " + p_Value1.DisplaySN() + " )");

		m_ValueList.push_back(SNI_TaggedValue(p_Value1, NULL));
	}

	SNI_ValueSet::~SNI_ValueSet()
	{
	}

	void SNI_ValueSet::PromoteMembers()
	{
	}

	SNI_ValueSet::SNI_ValueSet(const SNI_ValueSet &)
		: m_WorldSet(NULL)
	{
	}

	string SNI_ValueSet::GetTypeName() const
	{
		return "ValueSet";
	}

	string SNI_ValueSet::DisplayCpp() const
	{
		return "sn_ValueSet( [" + DisplayPmTaggedValueList(m_ValueList) + "] )";
	}

	string SNI_ValueSet::DisplaySN(long /*priority*/, SNI_VariablePointerList & p_DisplayVariableList) const
	{
		return "[" + DisplayPmTaggedExpressionList(m_ValueList, p_DisplayVariableList) + "]";
	}

	long SNI_ValueSet::GetPriority() const
	{
		return 100;
	}

	bool SNI_ValueSet::IsString() const
	{
		if (0 < m_ValueList.size())
		{
			return SN::Is<SNI_String *>(m_ValueList[1].GetValue()) || SN::Is<SNI_StringRef *>(m_ValueList[1].GetValue());
		}
		return false;
	}

	bool SNI_ValueSet::AllValues() const
	{
		bool trueFound = false;
		bool falseFound = false;
		for (const SNI_TaggedValue &tv : m_ValueList)
		{
			SN::SN_Bool b = tv.GetValue();
			if (b.IsKnownValue())
			{
				if (b.GetBool())
				{
					trueFound = true;
				}
				else
				{
					falseFound = true;
				}
			}
		}
		return trueFound && falseFound;
	}

	SNI_Expression * SNI_ValueSet::Clone(SNI_ReplacementList * p_ReplacementList, bool &p_Changed)
	{
		SN::SN_ValueSet result;
		if (0 < m_ValueList.size())
		{
			SNI_WorldSet * worldSet = new SNI_WorldSet();
			for (size_t i = 0; i<m_ValueList.size(); i++)
			{
				result.AddTaggedValue(m_ValueList[i].GetValue().GetSNI_Expression()->Clone(p_ReplacementList, p_Changed), NULL);
			}
		}
		return dynamic_cast<SNI_Expression *>(result.GetSNI_ValueSet());
	}

	void SNI_ValueSet::AssignToVariable(SNI_Variable *p_Variable)
	{
		if (!m_Variable)
		{
			m_Variable = p_Variable;
		}
		// Construct worlds from each value for the variable.
		if (0 < m_ValueList.size())
		{
			SNI_WorldSet * worldSet = new SNI_WorldSet();
			for (size_t i = 0; i<m_ValueList.size(); i++)
			{
				SNI_World * world = m_ValueList[i].GetWorld();
				if (world)
				{
					world->Activate();
				}
				else
				{
					m_ValueList[i].SetWorld(worldSet->CreateWorld());
				}
			}
		}
	}

	string SNI_ValueSet::GenerateTempVariableName()
	{
		if (m_Variable)
		{
			return m_Variable->GetName()+to_string(++m_MaxTempNum);
		}
		return "temp_" + to_string(++m_MaxTempNum);
	}

	void SNI_ValueSet::RemoveFailedWorlds()
	{
		SNI_WorldSet *worldSet = NULL;
		SNI_WorldSet *parentWorldSet = NULL;
		bool oneParent = true;
		for (SNI_TaggedValueList::iterator it = m_ValueList.begin(); it != m_ValueList.end();)
		{
			SNI_World * world = it->GetWorld();
			if (world && world->IsEmpty())
			{
				it = m_ValueList.erase(it);
			}
			else
			{
				if (world)
				{
					if (!worldSet)
					{
						worldSet = world->GetWorldSet();
						parentWorldSet = worldSet->OneParent();
					}
					oneParent &= (world->OneParent(parentWorldSet) != NULL);
				}
				++it;
			}
		}
		if (worldSet)
		{
			worldSet->CheckDependentWorlds();
		}
		if (oneParent && parentWorldSet)
		{ 
			for (SNI_TaggedValue &tv : m_ValueList)
			{
				tv.SetWorld(tv.GetWorld()->OneParent(parentWorldSet));
			}
			SetWorldSet(parentWorldSet);
		}
	}

	void SNI_ValueSet::FlattenValueSets()
	{
		for (size_t j = 0; j < m_ValueList.size(); j++)
		{
			SNI_World *world = m_ValueList[j].GetWorld();
			if (!world || !world->IsEmpty())
			{
				SN::SN_Expression loopValue = m_ValueList[j].GetValue();
				if (loopValue.GetSNI_Expression()->MarkComplete())
				{
					SNI_World *loopWorld = m_ValueList[j].GetWorld();
					SNI_ValueSet *vs = SN::Is<SNI_ValueSet *>(loopValue.GetVariableValue());
					if (vs)
					{
						m_ValueList[j].MarkForDeletion();
						SNI_WorldSet *vsWorldSet = new SNI_WorldSet;
						for (SNI_TaggedValueList::iterator itVS = vs->m_ValueList.begin(); itVS != vs->m_ValueList.end(); itVS++)
						{
							SN::SN_Expression vsValue = itVS->GetValue();
							SNI_World *vsWorld = itVS->GetWorld();
							bool exists = false;
							SNI_World *splitWorld = vsWorldSet->JoinWorldsArgs(AutoAddWorld, CreateIfActiveParents, exists, loopWorld, vsWorld);
							if (exists)
							{
								AddTaggedValue(vsValue, splitWorld);
							}
						}
					}
				}
			}
		}
	}

	bool SNI_ValueSet::ExtractBooleanValue(bool &p_Value)
	{
		if (m_ValueList.size() == 0)
		{
			return false;
		}
		for (size_t i = 0; i < m_ValueList.size(); i++)
		{
			SNI_World *world = m_ValueList[i].GetWorld();
			if (!world || !world->IsEmpty())
			{
				if (!SN::Is<SNI_Bool *>(m_ValueList[i].GetValue()))
				{
					return false;
				}
				else if (world && world->IsAnyActive())
				{
					return false;
				}
			}
		}
		for (size_t i = 0; i < m_ValueList.size(); i++)
		{
			SNI_World *world = m_ValueList[i].GetWorld();
			if (!world || !world->IsEmpty())
			{
				if (m_ValueList[i].GetValue().GetBool())
				{
					p_Value = true;
					return true;
				}
			}
		}
		p_Value = false;
		return true;
	}

	void SNI_ValueSet::Simplify()
	{
		RemoveFailedWorlds();
		FlattenValueSets();
	}

	SN::SN_Expression SNI_ValueSet::SimplifyValue()
	{
		bool extractValue = false;
		if (ExtractBooleanValue(extractValue))
		{
		 	return SN::SN_Bool(extractValue);
		}
		Simplify();
		SN::SN_Expression value(SNI_Helpers::Null);
		bool oneValue = true;
		bool found = false;
		SN::SN_Expression loopError = SN::SN_Error(GetTypeName() + " has no values.");
		for (SNI_TaggedValueList::iterator it = m_ValueList.begin(); it != m_ValueList.end();)
		{
			SNI_World *world = it->GetWorld();
			if (!world || !world->IsEmpty())
			{
				SN::SN_Expression loopValue = it->GetValue().GetVariableValue();
				if (it->IsMarkedForDeletion())
				{
					it = m_ValueList.erase(it);
				}
				else if (loopValue.IsError())
				{
					loopError = loopValue;
					it = m_ValueList.erase(it);
				}
				else
				{
					if (found)
					{
						if (!value.Equivalent(loopValue))
						{
							oneValue = false;
							break;
						}
					}
					else
					{
						value = loopValue;
						found = true;
					}
					it++;
				}
			}
		}
		if (!found)
		{
			return loopError;
		}
		if (oneValue)
		{
			return value;
		}
		return this;
	}

	SN::SN_Error SNI_ValueSet::AssertValue(const SN::SN_Expression &p_Value)
	{
		SN::LogContext context("SNI_ValueSet::AssertValue ( " + p_Value.DisplayValueSN() + " )");
		SN::SN_ParameterList *paramList = new SN::SN_ParameterList(1);
		(*paramList)[0] = SN::SN_Parameter(this);
		SN::SN_Error e = skynet::Same.Unify(paramList, p_Value);
		if (e.IsError())
		{
			e.AddNote(context, this, "Assert by cartesian product failed");
		}
		return e;
	}

	SNI_WorldSet * SNI_ValueSet::GetWorldSet()
	{
		if (!m_WorldSet)
		{
			m_WorldSet = new SNI_WorldSet;
		}
		return m_WorldSet;
	}

	void SNI_ValueSet::SetWorldSet(SNI_WorldSet * p_WorldSet)
	{
		m_WorldSet = p_WorldSet;
	}

	SN::SN_Expression SNI_ValueSet::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel) const
	{
		SN::SN_ValueSet vs;
		for (const SNI_TaggedValue &tv : m_ValueList)
		{
			SN::SN_ExpressionList paramListClone = *p_ParameterList;
			SN::SN_Expression v = tv.GetValue().GetSNI_Expression()->Call(&paramListClone, p_MetaLevel);
			vs.AddTaggedValue(v, tv.GetWorld());
		}
		return vs.SimplifyValue();
	}

	SN::SN_Error SNI_ValueSet::Unify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result)
	{
		SN::LogContext context("SNI_ValueSet::Unify ( " + DisplayPmParameterList(p_ParameterList) + " = " + p_Result.DisplaySN() + " )");
		SN::SN_Error err(true);
		bool success = false;
		for (const SNI_TaggedValue &tv : m_ValueList)
		{
			SN::SN_ParameterList paramListClone = *p_ParameterList;
			SN::SN_Error e = tv.GetValue().GetSNI_Expression()->Unify(&paramListClone, p_Result);
			if (e.GetBool())
			{
				success = true;
			}
			else
			{
				err = e;
			}
		}
		if (success)
		{
			return success;
		}
		err.AddNote(context, this, "No function from the valueset unified successfully.");
		return err;
	}

	SN::SN_Error SNI_ValueSet::ForEach(std::function<SN::SN_Error(const SN::SN_Expression &p_Param, SNI_World *p_World)> p_Action)
	{
		for (SNI_TaggedValue &tv : m_ValueList)
		{
			SN::SN_Value l_Value = tv.GetValue().GetVariableValue();
			SNI_World *world = tv.GetWorld();
			if (!world)
			{
				world = GetWorldSet()->CreateWorld();
				tv.SetWorld(world);
			}
			p_Action(l_Value, world);
		}
		return true;
	}

	void SNI_ValueSet::ForEachCall(SNI_Cartesian *p_Cart, long p_Depth)
	{
		for (SNI_TaggedValue &tv : m_ValueList)
		{
			p_Cart->ProcessValueCall(tv.GetValue(), tv.GetWorld(), p_Depth);
		}
	}

	SN::SN_Error SNI_ValueSet::ForEachCartUnify(long p_Depth, SNI_CartUnify *p_Cart)
	{
		for (SNI_TaggedValue &tv : m_ValueList)
		{
			SN::SN_Error e = p_Cart->ProcessValueUnify(p_Depth, tv.GetValue(), tv.GetWorld());
		}
		return true;
	}

	void SNI_ValueSet::ForEachSplit(SNI_Splitter * p_Splitter)
	{
		for (SNI_TaggedValue &tv : m_ValueList)
		{
			p_Splitter->ProcessValueSplit(tv.GetValue(), tv.GetWorld());
		}
	}

	size_t SNI_ValueSet::Cardinality(size_t p_MaxCardinality) const
	{
		if (m_ValueList.size())
		{
			return m_ValueList.size();
		}
		return p_MaxCardinality;
	}

	SN::SN_Expression SNI_ValueSet::Evaluate(long /* p_MetaLevel /* = 0 */) const
	{
		return this;
	}

	SN::SN_Expression SNI_ValueSet::PartialEvaluate(long /* p_MetaLevel = 0 */) const
	{
		return this;
	}

	SN::SN_ValueSet SNI_ValueSet::DoRemove(const SN::SN_Value &p_Other)
	{
		SN::LogContext context("SNI_ValueSet::DoRemove ( " + DisplayPmTaggedValueList(m_ValueList) + " )");

		SN::SN_ValueSet valueSet;
		for (size_t i = 0; i<m_ValueList.size(); i++)
		{
			if (!p_Other.Equivalent(m_ValueList[i].GetValue()))
			{
				valueSet.AddTaggedValue(m_ValueList[i].GetValue(), m_ValueList[i].GetWorld());
			}
		}
		return LOG_RETURN(context, valueSet);
	}


	bool SNI_ValueSet::DoIsEmpty() const
	{

		return m_ValueList.size() == 0;
	}

	SN::SN_Value SNI_ValueSet::DoBuildSet() const
	{
		return SN::SN_Set(SN::SN_ValueSet(this));
	}

	bool SNI_ValueSet::Equivalent(SNI_Object * p_Other) const
	{
		for (const SNI_TaggedValue &tv : m_ValueList)
		{
			if (tv.GetValue().GetSNI_Expression()->Equivalent(p_Other))
			{
				return true;
			}
		}
		return false; // There is no simple test for equivalence.
	}

	size_t SNI_ValueSet::Length() const
	{
		return m_ValueList.size();
	}

	SNI_TaggedValue &SNI_ValueSet::operator[](long p_index)
	{
		return m_ValueList[p_index];
	}

	void SNI_ValueSet::AddTaggedValue(const SN::SN_Expression &p_Value, SNI_World *p_World)
	{
		m_ValueList.push_back(SNI_TaggedValue(p_Value, p_World));
	}

	SN::SN_Error SNI_ValueSet::AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World **p_WorldList, SNI_WorldSet *p_WorldSet)
	{
		if (p_Value.IsError())
		{
			return p_Value;
		}
		bool exists = false;
		SNI_WorldSet *l_WorldSet = p_WorldSet;
		if (!l_WorldSet)
		{
			l_WorldSet = GetWorldSet();
		}
		SNI_TaggedValueList &valueList = m_ValueList;
		return p_Value.ForEach(
			[l_WorldSet, &exists, p_NumWorlds, p_WorldList, p_Value, &valueList] (const SN::SN_Expression &p_Param, SNI::SNI_World *p_World) -> SN::SN_Error
			{
				SNI_World *world = l_WorldSet->JoinWorldsArray(AutoAddWorld, AlwaysCreateWorld, exists, p_NumWorlds, p_WorldList, p_World);
				if (exists)
				{
					string worldString = DisplayWorlds(p_NumWorlds, p_WorldList);
					SN::LogContext context("SNI_Variable::AddValue ( ok " + p_Param.DisplayValueSN() + " " + worldString + " )");
					valueList.push_back(SNI_TaggedValue(p_Param, world));
				}
				else
				{
					string worldString = DisplayWorlds(p_NumWorlds, p_WorldList);
					SN::LogContext context("SNI_Variable::AddValue ( conflict " + p_Param.DisplayValueSN() + " " + worldString + " )");
				}
				return true;
			}
		);
	}

	void SNI_ValueSet::Append(const SN::SN_Value &p_Value)
	{
		m_ValueList.push_back(SNI_TaggedValue(p_Value));
	}

	SNI_TaggedValue SNI_ValueSet::At(const long &p_Value)
	{
		return m_ValueList.at(p_Value);
	}

	bool SNI_ValueSet::IsReferableValue(const SN::SN_Value &p_Value) const
	{
		for (size_t i = 0; i < m_ValueList.size(); i++)
		{
			if (!m_ValueList[i].GetWorld() || !m_ValueList[i].GetWorld()->IsEmpty())
			{
				if (m_ValueList[i].GetValue().IsNull() || m_ValueList[i].GetValue().Equivalent(p_Value)) //19/09/2017
				{
					return true;
				}
			}
		}
		return false;
	}

	bool SNI_ValueSet::GetBool() const
	{
		return IsReferableValue(skynet::True);
	}

	SN::SN_Value SNI_ValueSet::DoOr(SNI_Expression * p_Other) const
	{
		SN::SN_ValueSet result(this);
		if (m_Variable)
		{
			result = new SNI_ValueSet;
			SNI_WorldSet *worldSet = new SNI_WorldSet;
			for (size_t i = 0; i<m_ValueList.size(); i++)
			{
				bool exists = false;
				SNI_World * world = worldSet->JoinWorldsArgs(AutoAddWorld, CreateIfActiveParents, exists, m_ValueList[i].GetWorld());
				if (exists)
				{
					result.AddTaggedValue(m_ValueList[i].GetValue(), world);
				}
			}
		}
		SN::SN_Expression other_expression = p_Other;
		if (SN::Is<SNI_ValueSet *>(other_expression))
		{
			SNI_WorldSet *worldSet = new SNI_WorldSet;
			SN::SN_ValueSet other = other_expression;
			for (long j = 0; j<other.Length(); j++)
			{
				bool exists = false;
				SNI_World * world = worldSet->JoinWorldsArgs(AutoAddWorld, CreateIfActiveParents, exists, other[j].GetWorld());
				if (exists)
				{
					result.AddTaggedValue(other[j].GetValue(), world);
				}
			}
		}
		else
		{
			result.AddTaggedValue(other_expression, NULL);
		}

		return result;
	}

	/*static*/SN::SN_Value SNI_ValueSet::GeneralDoOr(const SNI_Value *p_Left, SNI_Expression *p_Right)
	{
		SN::SN_Value left_value(p_Left);
		SN::SN_Value right_value(p_Right);
		SNI_WorldSet * worldSet = new SNI_WorldSet();
		SN::SN_ValueSet result;
		result.AddTaggedValue(left_value, NULL);

		if (SN::Is<SNI_ValueSet *>(right_value))
		{
			SN::SN_ValueSet rightValueSet = right_value;
			for (long j = 0; j<rightValueSet.Length(); j++)
			{
				bool exists = false;
				SNI_World * world = worldSet->JoinWorldsArgs(AutoAddWorld, CreateIfActiveParents, exists, rightValueSet[j].GetWorld());
				if (exists)
				{
					result.AddTaggedValue(rightValueSet[j].GetValue(), world);
				}
			}
		}
		else
		{
			result.AddTaggedValue(right_value, NULL);
		}

		return result;
	}

	SN::SN_Value SNI_ValueSet::DoIf(SNI_Expression * p_PositiveCase, SNI_Expression *p_NegativeCase) const
	{
		SN::LogContext context(DisplaySN0() + "SNI_ValueSet::DoIf ( " + SN::SN_Expression(p_PositiveCase).DisplaySN() + SN::SN_Expression(p_NegativeCase).DisplaySN() + " )");

		SN::SN_ValueSet result;
		SNI_WorldSet * worldSet = new SNI_WorldSet();

		for (size_t i = 0; i<m_ValueList.size(); i++)
		{
			if (m_ValueList[i].GetValue().GetBool())
			{   // Positive case

				SN::SN_Expression l_PositiveExpression = p_PositiveCase->Evaluate();

				if (SN::Is<SNI_ValueSet *>(l_PositiveExpression))
				{
					SN::SN_ValueSet l_PositiveValueSet = l_PositiveExpression;

					for (long j = 0; j<l_PositiveValueSet.Length(); j++)
					{
						bool exists = false;
						SNI_World * world = worldSet->JoinWorldsArgs(AutoAddWorld, CreateIfActiveParents, exists, m_ValueList[i].GetWorld(), l_PositiveValueSet[j].GetWorld());
						if (exists)
						{
							result.AddTaggedValue(l_PositiveValueSet[j].GetValue(), world);
						}
					}
				}
				else
				{
					bool exists = false;
					SNI_World * world = worldSet->JoinWorldsArgs(AutoAddWorld, CreateIfActiveParents, exists, m_ValueList[i].GetWorld());
					if (exists)
					{
						result.AddTaggedValue(l_PositiveExpression, world);
					}
				}
			}
			else
			{   // Negative case.

				SN::SN_Expression l_NegativeExpression = p_NegativeCase->Evaluate();

				if (SN::Is<SNI_ValueSet *>(l_NegativeExpression))
				{
					SN::SN_ValueSet l_NegativeValueSet = l_NegativeExpression;

					for (long j = 0; j<l_NegativeValueSet.Length(); j++)
					{
						bool exists = false;
						SNI_World * world = worldSet->JoinWorldsArgs(AutoAddWorld, CreateIfActiveParents, exists, m_ValueList[i].GetWorld(), l_NegativeValueSet[j].GetWorld());
						if (exists)
						{
							result.AddTaggedValue(l_NegativeValueSet[j].GetValue(), world);
						}
					}
				}
				else
				{
					bool exists = false;
					SNI_World * world = worldSet->JoinWorldsArgs(AutoAddWorld, CreateIfActiveParents, exists, m_ValueList[i].GetWorld());
					if (exists)
					{
						result.AddTaggedValue(l_NegativeExpression, world);
					}
				}
			}
		}

		return LOG_RETURN(context, result);
	}

	SN::SN_Value SNI_ValueSet::DoCollapse()
	{
		bool typeIsBool = true;
		bool value = false;
		for (SNI_TaggedValueList::iterator it = m_ValueList.begin(); it != m_ValueList.end(); it++)
		{
			if (!it->GetWorld()->IsEmpty())
			{
				SN::SN_Value val(it->GetValue());
				if (!SN::Is<SNI_Bool *>(val) && !SN::Is<SNI_Null *>(val))
				{
					typeIsBool = false;
				}
				if (val.GetBool())
				{
					value = true;
				}
			}
		}
		if (typeIsBool)
		{
			for (SNI_TaggedValueList::iterator it = m_ValueList.begin(); it != m_ValueList.end(); it++)
			{
				if (!it->GetWorld()->IsEmpty())
				{
					if (SN::Is<SNI_Null *>(it->GetValue()) || !it->GetValue().GetBool())
					{
						it->GetWorld()->Fail();
					}
				}
			}
			RemoveFailedWorlds();
			return SN::SN_Bool(value);
		}
		return this;
	}
}
