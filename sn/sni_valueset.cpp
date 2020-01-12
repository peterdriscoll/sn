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
		, m_Variable(NULL)
	{
		SN::LogContext context("SNI_ValueSet::SNI_ValueSet ( " + p_Value1.DisplaySN() + ", " + p_Value2.DisplaySN() + " )");

		m_ValueList.push_back(SNI_TaggedValue(p_Value1, NULL));
		m_ValueList.push_back(SNI_TaggedValue(p_Value2, NULL));
	}

	SNI_ValueSet::SNI_ValueSet(const SN::SN_Value &p_Value1)
		: m_WorldSet(NULL)
		, m_Variable(NULL)
	{
		SN::LogContext context("SNI_ValueSet::SNI_ValueSet ( " + p_Value1.DisplaySN() + " )");

		m_ValueList.push_back(SNI_TaggedValue(p_Value1, NULL));
	}

	SNI_ValueSet::~SNI_ValueSet()
	{
	}

	void SNI_ValueSet::PromoteMembers()
	{
		for (SNI_TaggedValue &tv : m_ValueList)
		{
			tv.PromoteMembersExternal(this);
		}
	}

	void SNI_ValueSet::CheckWorldSetConsistency()
	{
		for (const SNI_TaggedValue &tv : m_ValueList)
		{
			SNI_World *world = tv.GetWorld();
			if (world)
			{
				if (world->GetWorldSet() != m_WorldSet)
				{
					ASSERTM(world->GetWorldSet() == m_WorldSet, "World set inconsistency");
				}
			}
			else
			{
				if (m_WorldSet)
				{
					ASSERTM(!m_WorldSet, "Null world for null world set inconsistency");
				}
			}
		}
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

	string SNI_ValueSet::DisplaySN(long /*priority*/, SNI_DisplayOptions & p_DisplayOptions) const
	{
		string status;
		if (!IsComplete())
		{
			status = "incomplete:";
		}
		return status + "[" + DisplayPmTaggedExpressionList(m_ValueList, p_DisplayOptions) + "]";
	}

	long SNI_ValueSet::GetPriority() const
	{
		return 100;
	}

	bool SNI_ValueSet::IsComplete() const
	{
		if (m_WorldSet)
		{
			if (!m_WorldSet->IsComplete())
			{
				return false;
			}
		}
		return true;
	}

	void SNI_ValueSet::Complete()
	{
		if (m_WorldSet)
		{
			m_WorldSet->Complete();
		}
	}

	bool SNI_ValueSet::IsKnownValue() const
	{
		SNI_World *contextWorld = SNI_World::ContextWorld();
		if (contextWorld)
		{
			for (const SNI_TaggedValue &tv : m_ValueList)
			{
				SNI_World *world = tv.GetWorld();
				if (!world || !world->IsEmpty())
				{
					if (contextWorld->CompatibleWorld(world))
					{
						return true;
					}
				}
			}
		}
		else
		{
			return IsComplete() && !m_ValueList.empty();
		}
		return false;
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
		SNI_World *contextWorld = SNI_World::ContextWorld();
		for (const SNI_TaggedValue &tv : m_ValueList)
		{
			SNI_World *world = tv.GetWorld();
			if (!world || !world->IsEmpty())
			{
				if (!contextWorld || contextWorld->CompatibleWorld(world))
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
			}
		}
		return trueFound && falseFound;
	}

	SNI_Expression * SNI_ValueSet::Clone(SNI_Frame *p_Frame, bool &p_Changed)
	{
		Validate();
		SN::SN_ValueSet result;
		if (0 < m_ValueList.size())
		{
			SNI_WorldSet *worldSet = m_WorldSet;
			SNI_WorldSet *worldSetClone = NULL;
			if (worldSet)
			{
				worldSetClone = worldSet->Clone();
			}
			result.SetWorldSet(worldSetClone);
			SNI_World *contextWorld = SNI_World::ContextWorld();
			for (SNI_TaggedValue &tv : m_ValueList)
			{
				SNI_World *world = tv.GetWorld();
				if (!contextWorld || contextWorld->CompatibleWorld(world))
				{
					SNI_World *cloneWorld = NULL;
					if (worldSet)
					{
						cloneWorld = worldSetClone->CreateCloneWorld(world);
					}
					result.AddTaggedValue(tv.GetValue().GetSNI_Expression()->Clone(p_Frame, p_Changed), cloneWorld);
				}
			}
		}
		Validate();
		LOG(WriteLine(SN::DebugLevel, result.DisplaySN()));
		SNI_Thread::GetThread()->SetDebugId(GetDebugId());
		SNI_Thread::GetThread()->DebugCommand(SN::ClonePoint, GetTypeName() + ".Clone", SN::CloneId);
		return dynamic_cast<SNI_Expression *>(result.GetSNI_ValueSet());
	}

	void SNI_ValueSet::AssignToVariable(SNI_Variable *p_Variable)
	{
		if (!m_Variable)
		{
			m_Variable = p_Variable;
		}
		// Construct worlds from each value for the variable.
		if (0 < m_ValueList.size() && !m_WorldSet)
		{
			SNI_WorldSet * worldSet = GetWorldSet();
			SNI_World *contextWorld = SNI_World::ContextWorld();
			for (SNI_TaggedValue &tv : m_ValueList)
			{
				SNI_World *world = tv.GetWorld();
				if (!world || !world->IsEmpty())
				{
					if (world)
					{
						world->Activate();
					}
					else
					{
						tv.SetWorld(worldSet->CreateWorldForValue(tv.GetValue()));
					}
				}
			}
			worldSet->Complete();
			Validate();
		}
		m_WorldSet->AttachExpression(m_Variable);
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
		if (oneParent && parentWorldSet)
		{ 
			for (SNI_TaggedValue &tv : m_ValueList)
			{
				tv.SetWorld(tv.GetWorld()->OneParent(parentWorldSet));
			}
			SetWorldSet(parentWorldSet);
		}
		CheckWorldSetConsistency();
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
						SNI_WorldSet *vsWorldSet = new SNI_WorldSet(SN::SN_ValueSet(this));
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
						vsWorldSet->Complete();
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
		SNI_World *contextWorld = SNI_World::ContextWorld();
		for (const SNI_TaggedValue &tv : m_ValueList)
		{
			SNI_World *world = tv.GetWorld();
			if (!world || !world->IsEmpty())
			{
				if (!contextWorld || contextWorld->CompatibleWorld(world))
				{
					if (!SN::Is<SNI_Bool *>(tv.GetValue()))
					{
						return false;
					}
					else if (world && world->IsAnyActive())
					{
						return false;
					}
				}
			}
		}
		for (const SNI_TaggedValue &tv : m_ValueList)
		{
			SNI_World *world = tv.GetWorld();
			if (!world || !world->IsEmpty())
			{
				if (!contextWorld || contextWorld->CompatibleWorld(world))
				{
					if (tv.GetValue().GetBool())
					{
						p_Value = true;
						return true;
					}
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

	void SNI_ValueSet::Validate()
	{
		CheckWorldSetConsistency();
	}

	SN::SN_Expression SNI_ValueSet::SimplifyValue()
	{
		bool extractValue = false;
		Validate();
		if (ExtractBooleanValue(extractValue))
		{
		 	return SN::SN_Bool(extractValue);
		}
		Simplify();
		SN::SN_Expression value(SNI_Helpers::Null);
		bool oneValue = true;
		bool found = false;

		SN::SN_Expression loopError;
		SNI_World *contextWorld = SNI_World::ContextWorld();
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
			else
			{
				it++;
			}
		}
		Validate();
		if (!found)
		{
			if (loopError.IsNull())
			{
				return SN::SN_Error(GetTypeName() + " has no values.");
			}
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
		SN::SN_Expression *paramList = new SN::SN_Expression[2];
		paramList[0] = p_Value;
		paramList[1] = this;
		SN::SN_Error e = skynet::Same.GetSNI_FunctionDef()->UnifyArray(paramList);
		if (e.IsError())
		{
			e.AddNote(context, this, "Assert by cartesian product failed");
		}
		return e;
	}

	SNI_WorldSet * SNI_ValueSet::GetWorldSet()
	{
		if (!m_WorldSet && !SN::SN_Transaction::InWebServer())
		{
			m_WorldSet = new SNI_WorldSet(SN::SN_ValueSet(this));
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
		SNI_World *contextWorld = SNI_World::ContextWorld();
		for (const SNI_TaggedValue &tv : m_ValueList)
		{
			SNI_World *world = tv.GetWorld();
			if (!world || !world->IsEmpty())
			{
				if (!contextWorld || contextWorld->CompatibleWorld(world))
				{
					SN::SN_ExpressionList paramListClone = *p_ParameterList;
					SN::SN_Expression v = tv.GetValue().GetSNI_Expression()->Call(&paramListClone, p_MetaLevel);
					vs.AddTaggedValue(v, tv.GetWorld());
				}
			}
		}
		return vs.SimplifyValue();
	}

	SN::SN_Expression SNI_ValueSet::Unify(SN::SN_ExpressionList * p_ParameterList)
	{
		SN::LogContext context("SNI_ValueSet::Unify ( " + DisplayPmExpressionList(p_ParameterList) + " )");
		Validate();
		SN::SN_Error err(true);
		bool success = false;
		SNI_World *contextWorld = SNI_World::ContextWorld();
		SNI_WorldSet *worldSet = GetWorldSet();
		for (SNI_TaggedValue &tv : m_ValueList)
		{
			SNI_World *world = tv.GetWorld();
			if (world)
			{
				world->Activate();
			}
			else
			{
				world = worldSet->CreateWorldForValue(tv.GetValue());
			}
			if (!world->IsEmpty())
			{
				if (!contextWorld || contextWorld->CompatibleWorld(world))
				{
					SN::SN_ExpressionList paramListClone = *p_ParameterList;
					// Flatten the call stack, by returning the function to be called from Unify, instead of calling it there.
					SNI_Expression *function = tv.GetValue().GetSNI_Expression();
					SNI_Error *e = dynamic_cast<SNI_Error *>(function);
					SNI_World::PushContextWorld(world);
					while (!e)
					{
						SNI_FunctionDef *functionDef = dynamic_cast<SNI_FunctionDef *>(function);
						if (functionDef)
						{
							SN::SN_Expression *param_List = functionDef->LoadParametersUnify(&paramListClone);
							function = functionDef->UnifyArray(param_List).GetSNI_Expression();
						}
						else
						{
							function = function->Unify(&paramListClone).GetSNI_Expression();
						}
						e = dynamic_cast<SNI_Error *>(function);
					}
					SNI_World::PopContextWorld();
					if (e->GetBool())
					{
						success = true;
					}
					else
					{
						err = e;
					}
				}
			}
		}
		worldSet->Complete();
		SNI_Thread::GetThread()->DebugCommand(SN::CallPoint, GetTypeName() + ".Unify after all values", SN::LeftId);
		if (success)
		{
			return SN::SN_Error(success);
		}
		err.AddNote(context, this, "No function from the valueset unified successfully.");
		return err;
	}

	SN::SN_Error SNI_ValueSet::ForEach(std::function<SN::SN_Error(const SN::SN_Expression &p_Param, SNI_World *p_World)> p_Action)
	{
		Validate();
		SNI_World *contextWorld = SNI_World::ContextWorld();
		SNI_WorldSet *worldSet = GetWorldSet();
		for (SNI_TaggedValue &tv : m_ValueList)
		{
			SNI_World *world = tv.GetWorld();
			if (!contextWorld || contextWorld->CompatibleWorld(world))
			{
				SN::SN_Value l_Value = tv.GetValue().GetVariableValue();
				if (!world && worldSet)
				{
					world = worldSet->CreateWorldForValue(l_Value);
					tv.SetWorld(world);
				}
				p_Action(l_Value, world);
			}
		}
		if (worldSet)
		{
			worldSet->Complete();
			Validate();
		}
		return skynet::OK;
	}

	SN::SN_Error SNI_ValueSet::ForEachCart(long p_Depth, SNI_Cart *p_Cart)
	{
		SNI_World *contextWorld = SNI_World::ContextWorld();
		for (SNI_TaggedValue &tv : m_ValueList)
		{
			SNI_World *world = tv.GetWorld();
			if (!contextWorld || contextWorld->CompatibleWorld(world))
			{
				SNI_Error::PushHandler();
				SN::SN_Error e = p_Cart->ProcessValue(p_Depth, tv.GetValue(), world);
				SNI_Error::PopHandler();
			}
		}
		return true;
	}

	void SNI_ValueSet::ForEachSplit(SNI_Splitter * p_Splitter)
	{
		SNI_World *contextWorld = SNI_World::ContextWorld();
		p_Splitter->SetWorldSet(GetWorldSet());
		for (SNI_TaggedValue &tv : m_ValueList)
		{
			SNI_World *world = tv.GetWorld();
			if (!contextWorld || contextWorld->CompatibleWorld(world))
			{
				p_Splitter->ProcessValueSplit(tv.GetValue(), world);
			}
		}
	}

	size_t SNI_ValueSet::Cardinality(size_t p_MaxCardinality) const
	{
		SNI_World *contextWorld = SNI_World::ContextWorld();
		size_t card = 1;
		if (contextWorld)
		{
			for (const SNI_TaggedValue &tv : m_ValueList)
			{
				SNI_World *world = tv.GetWorld();
				if (!contextWorld || contextWorld->CompatibleWorld(world))
				{
					card++;
				}
			}
		}
		else
		{
			card = m_ValueList.size();
		}
		if (card)
		{
			return card;
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
		SNI_World *contextWorld = SNI_World::ContextWorld();
		for (SNI_TaggedValue &tv : m_ValueList)
		{
			SNI_World *world = tv.GetWorld();
			if (!world || !world->IsEmpty())
			{
				if (!contextWorld || contextWorld->CompatibleWorld(world))
				{
					if (!p_Other.Equivalent(tv.GetValue()))
					{
						valueSet.AddTaggedValue(tv.GetValue(), world);
					}
				}
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
		SNI_World *contextWorld = SNI_World::ContextWorld();
		for (const SNI_TaggedValue &tv : m_ValueList)
		{
			SNI_World *world = tv.GetWorld();
			if (!world || !world->IsEmpty())
			{
				if (!contextWorld || contextWorld->CompatibleWorld(world))
				{
					if (tv.GetValue().GetSNI_Expression()->Equivalent(p_Other))
					{
						return true;
					}
				}
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
		if (p_World)
		{
			p_World->AttachValue(p_Value);
		}
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
					SN::LogContext context("SNI_ValueSet::AddValue ( ok " + p_Param.DisplayValueSN() + " " + worldString + " )");
					valueList.push_back(SNI_TaggedValue(p_Param, world));
				}
				else
				{
					string worldString = DisplayWorlds(p_NumWorlds, p_WorldList);
					SN::LogContext context("SNI_ValueSet::AddValue ( conflict " + p_Param.DisplayValueSN() + " " + worldString + " )");
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
		SNI_World *contextWorld = SNI_World::ContextWorld();
		for (const SNI_TaggedValue &tv : m_ValueList)
		{
			SNI_World *world = tv.GetWorld();
			if (!world || !world->IsEmpty())
			{
				if (!contextWorld || contextWorld->CompatibleWorld(world))
				{
					if (tv.GetValue().IsNull() || tv.GetValue().Equivalent(p_Value))
					{
						return true;
					}
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
			SNI_WorldSet *worldSet = new SNI_WorldSet(SN::SN_Value(this)||SN::SN_Expression(p_Other));
			for (const SNI_TaggedValue &tv : m_ValueList)
			{
				bool exists = false;
				SNI_World * newWorld = worldSet->JoinWorldsArgs(AutoAddWorld, CreateIfActiveParents, exists, tv.GetWorld());
				if (exists)
				{
					result.AddTaggedValue(tv.GetValue(), newWorld);
				}
			}
			worldSet->Complete();
		}
		SN::SN_Expression other_expression = p_Other;
		if (SN::Is<SNI_ValueSet *>(other_expression))
		{
			SNI_WorldSet *worldSet = new SNI_WorldSet(SN::SN_Value(this)||other_expression);
			SN::SN_ValueSet other = other_expression;
			for (size_t j = 0; j<other.Length(); j++)
			{
				bool exists = false;
				SNI_World * newWorld = worldSet->JoinWorldsArgs(AutoAddWorld, CreateIfActiveParents, exists, other[j].GetWorld());
				if (exists)
				{
					result.AddTaggedValue(other[j].GetValue(), newWorld);
				}
			}
			worldSet->Complete();
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
		SN::SN_Expression source = left_value || right_value;
		SNI_WorldSet * worldSet = new SNI_WorldSet(source);
		SN::SN_ValueSet result;
		result.AddTaggedValue(left_value, NULL);

		if (SN::Is<SNI_ValueSet *>(right_value))
		{
			SN::SN_ValueSet rightValueSet = right_value;
			for (size_t j = 0; j<rightValueSet.Length(); j++)
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
		SNI_WorldSet * worldSet = result.GetSNI_ValueSet()->GetWorldSet();

		for (size_t i = 0; i<m_ValueList.size(); i++)
		{
			if (m_ValueList[i].GetValue().GetBool())
			{   // Positive case

				SN::SN_Expression l_PositiveExpression = p_PositiveCase->Evaluate();

				if (SN::Is<SNI_ValueSet *>(l_PositiveExpression))
				{
					SN::SN_ValueSet l_PositiveValueSet = l_PositiveExpression;

					for (size_t j = 0; j<l_PositiveValueSet.Length(); j++)
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

					for (size_t j = 0; j<l_NegativeValueSet.Length(); j++)
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

		worldSet->Complete();
		return LOG_RETURN(context, result);
	}

	SN::SN_Value SNI_ValueSet::DoCollapse()
	{
		bool typeIsBool = true;
		bool value = false;
		SNI_World *contextWorld = SNI_World::ContextWorld();
		for (const SNI_TaggedValue &tv : m_ValueList)
		{
			SNI_World *world = tv.GetWorld();
			if (!world || !world->IsEmpty())
			{
				if (!contextWorld || contextWorld->CompatibleWorld(world))
				{
					SN::SN_Value val(tv.GetValue());
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
		}
		if (typeIsBool && value)
		{
			for (const SNI_TaggedValue &tv : m_ValueList)
			{
				SNI_World *world = tv.GetWorld();
				if (!world || !world->IsEmpty())
				{
					if (!contextWorld || contextWorld->CompatibleWorld(world))
					{
						if (SN::Is<SNI_Null *>(tv.GetValue()) || !tv.GetValue().GetBool())
						{
							SN::SN_Error err = world->Fail(IncompatibleValue);
							if (err.IsError())
							{
								return err;
							}
						}
					}
				}
			}
			RemoveFailedWorlds();
			return SN::SN_Bool(value);
		}
		return this;
	}
}
