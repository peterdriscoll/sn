#include "sni_valueset.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

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
		LOGGING(SN::LogContext context("SNI_ValueSet::SNI_ValueSet ( " + p_Value1.DisplaySN() + ", " + p_Value2.DisplaySN() + " )"));

		m_ValueList.push_back(SNI_TaggedValue(p_Value1, NULL));
		m_ValueList.push_back(SNI_TaggedValue(p_Value2, NULL));
	}

	SNI_ValueSet::SNI_ValueSet(const SN::SN_Value &p_Value1)
		: m_WorldSet(NULL)
		, m_Variable(NULL)
	{
		LOGGING(SN::LogContext context("SNI_ValueSet::SNI_ValueSet ( " + p_Value1.DisplaySN() + " )"));

		m_ValueList.push_back(SNI_TaggedValue(p_Value1, NULL));
	}

	SNI_ValueSet::~SNI_ValueSet()
	{
	}

	SNI_Expression *SNI_ValueSet::Copy() const
	{
		return new SNI_ValueSet(*this);
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
		if (!SN::SN_Transaction::InWebServer())
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
	}

	SNI_ValueSet::SNI_ValueSet(const SNI_ValueSet &)
		: m_WorldSet(NULL)
		, m_Variable(NULL)
	{
	}

	string SNI_ValueSet::GetTypeName() const
	{
		return "ValueSet";
	}

	string SNI_ValueSet::DisplayCpp() const
	{
		SNI_DisplayOptions displayOptions(doTextOnly);
		return "sn_ValueSet( [" + DisplaySnTaggedValueList(m_ValueList, displayOptions) + "] )";
	}

	string SNI_ValueSet::DisplaySN(long /*priority*/, SNI_DisplayOptions & p_DisplayOptions) const
	{
		string status;
		if (!IsComplete())
		{
			status = "incomplete:";
		}
		return status + "[" + DisplaySnTaggedExpressionList(m_ValueList, p_DisplayOptions) + "]";
	}

	long SNI_ValueSet::GetPriority() const
	{
		return 100;
	}

	bool SNI_ValueSet::IsValueHolder() const
	{
		return true;
	}

	bool SNI_ValueSet::IsValueSet() const
	{
		return true;
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

	bool SNI_ValueSet::IsString() const
	{
		if (!IsComplete())
		{
			return false;
		}
		SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
		bool found = false;
		for (const SNI_TaggedValue &tv : m_ValueList)
		{
			SNI_World *world = tv.GetWorld();
			if (!world || !world->IsEmpty())
			{
				if (!contextWorld || contextWorld->CompatibleWorld(world))
				{
					if (tv.GetValue().IsString())
					{
						found = true;
					}
					else
					{
						return false;
					}
				}
			}
		}
		return found;
	}

	bool SNI_ValueSet::IsKnownValue() const
	{
		if (!IsComplete())
		{
			return false;
		}
		SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
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
			return !m_ValueList.empty();
		}
		return false;
	}

	bool SNI_ValueSet::IsLeftKnownValue() const
	{
		if (!IsComplete())
		{
			return false;
		}
		SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
		bool found = false;
		for (const SNI_TaggedValue &tv : m_ValueList)
		{
			SNI_World *world = tv.GetWorld();
			if (!world || !world->IsEmpty())
			{
				if (!contextWorld || contextWorld->CompatibleWorld(world))
				{
					if (!tv.GetValue().IsVariable())
					{
						if (tv.GetValue().IsLeftKnownValue())
						{
							found = true;
						}
						else
						{
							return false;
						}
					}
				}
			}
		}
		return found;
	}

	bool SNI_ValueSet::IsRightKnownValue() const
	{
		if (!IsComplete())
		{
			return false;
		}
		SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
		bool found = false;
		for (const SNI_TaggedValue &tv : m_ValueList)
		{
			SNI_World *world = tv.GetWorld();
			if (!world || !world->IsEmpty())
			{
				if (!contextWorld || contextWorld->CompatibleWorld(world))
				{
					if (!tv.GetValue().IsVariable())
					{
						if (tv.GetValue().IsRightKnownValue())
						{
							found = true;
						}
						else
						{
							return false;
						}
					}
				}
			}
		}
		return found;
	}

	bool SNI_ValueSet::IsReferableValue() const
	{
		if (!IsComplete())
		{
			return false;
		}
		bool found = false;
		SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
		for (const SNI_TaggedValue &tv : m_ValueList)
		{
			SNI_World *world = tv.GetWorld();
			if (!world || !world->IsEmpty())
			{
				if (!contextWorld || contextWorld->CompatibleWorld(world))
				{
					if (!tv.GetValue().IsVariable())
					{
						if (tv.GetValue().IsReferableValue())
						{
							found = true;
						}
						else
						{
							return false;
						}
					}
				}
			}
		}
		return found;
	}

	bool SNI_ValueSet::CompareValue(const SN::SN_Value &p_Value) const
	{
		SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
		for (const SNI_TaggedValue &tv : m_ValueList)
		{
			SNI_World *world = tv.GetWorld();
			if (!world || !world->IsEmpty())
			{
				if (!contextWorld || contextWorld->CompatibleWorld(world))
				{
					if (!tv.GetValue().IsKnownValue() || tv.GetValue().Equivalent(p_Value))
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	bool SNI_ValueSet::AllValues() const
	{
		bool trueFound = false;
		bool falseFound = false;
		SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
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

	bool SNI_ValueSet::AllValuesBoolean() const
	{
		SNI_World* contextWorld = SNI_Thread::GetThread()->ContextWorld();
		for (const SNI_TaggedValue& tv : m_ValueList)
		{
			SNI_World* world = tv.GetWorld();
			if (!world || !world->IsEmpty())
			{
				if (!contextWorld || contextWorld->CompatibleWorld(world))
				{
					SN::SN_Expression b = tv.GetValue();
					if (!b.AllValuesBoolean())
					{
						return false;
					}
				}
			}
		}
		return true;
	}

	SN::SN_Expression SNI_ValueSet::GetVariableValue(bool p_IfComplete)
	{
		if (p_IfComplete && !IsComplete())
		{
			return this;
		}
		SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
		size_t count = 0;
		SN::SN_Expression value = this;
		for (const SNI_TaggedValue &tv : m_ValueList)
		{
			SNI_World *world = tv.GetWorld();
			if (!world || !world->IsEmpty())
			{
				if (!contextWorld || contextWorld->CompatibleWorld(world))
				{
					if (count++)
					{
						return this;
					}
					else
					{
						value = tv.GetValue();
					}
				}
			}
		}
		return value;
	}

	bool SNI_ValueSet::AllValuesEqual(const SN::SN_Expression & p_Value) const
	{
		bool found = false;
		SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
		for (const SNI_TaggedValue &tv : m_ValueList)
		{
			SNI_World *world = tv.GetWorld();
			if (!world || !world->IsEmpty())
			{
				if (!contextWorld || contextWorld->CompatibleWorld(world))
				{
					if (!tv.GetValue().Equivalent(p_Value))
					{
						return false;
					}
					else
					{
						found = true;
					}
				}
			}
		}
		return found;
	}

	SNI_Expression * SNI_ValueSet::Clone(long p_MetaLevel, SNI_Frame *p_Frame, bool &p_Changed)
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
			SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
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
					result.AddTaggedValue(tv.GetValue().GetSNI_Expression()->Clone(p_MetaLevel, p_Frame, p_Changed), cloneWorld);
				}
			}
			worldSetClone->Complete();
		}
		Validate();

		Breakpoint(SN::DebugStop, SN::CloneId, GetTypeName(), "Clone", this, SN::ClonePoint);
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
			SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
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
		SNI_Thread::GetThread()->Lock();
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
		SNI_Thread::GetThread()->Unlock();
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

	bool SNI_ValueSet::NeedsFlattening()
	{
		bool needsFlattening = false;
		for (size_t j = 0; j < m_ValueList.size(); j++)
		{
			SNI_World* world = m_ValueList[j].GetWorld();
			if (!world || !world->IsEmpty())
			{
				SN::SN_Expression loopValue = m_ValueList[j].GetValue();
				if (loopValue.IsKnownValue() && loopValue.Cardinality())
				{
					if (loopValue.GetSNI_Expression()->MarkComplete())
					{
						SN::SN_Expression value = loopValue.GetSafeValue();
						SNI_ValueSet* vs = value.GetSNI_ValueSet();
						if (vs)
						{
							needsFlattening = true;
						}
						else if (loopValue.IsVariable() && loopValue.IsKnownValue())
						{
							m_ValueList[j].SetValue(value);
						}
					}
				}
			}
		}
		return needsFlattening;
	}

	void SNI_ValueSet::FlattenValueSets()
	{
		SNI_TaggedValueList valueList = m_ValueList;
		m_ValueList.clear();
		SNI_WorldSet* vsWorldSet = new SNI_WorldSet();
		for (auto taggedValue : valueList)
		{
			SNI_World *world = taggedValue.GetWorld();
			if (!world || !world->IsEmpty())
			{
				SN::SN_Expression loopValue = taggedValue.GetValue();
				if (loopValue.IsKnownValue() && loopValue.Cardinality())
				{
					SN::SN_Expression value = loopValue.GetValue();
					SNI_ValueSet* vs = value.GetSNI_ValueSet();
					if (vs)
					{
						for (SNI_TaggedValue& tvLoop : vs->m_ValueList)
						{
							SN::SN_Expression vsValue = tvLoop.GetValue();
							SNI_World* vsWorld = tvLoop.GetWorld();
							bool exists = false;
							SNI_World* splitWorld = vsWorldSet->JoinWorldsArgs(AutoAddWorld, CreateIfActiveParents, exists, world, vsWorld);
							if (exists)
							{
								AddTaggedValue(vsValue, splitWorld);
							}
						}
					}
					else 
					{
						bool exists = false;
						SNI_World* splitWorld = vsWorldSet->JoinWorldsArgs(AutoAddWorld, CreateIfActiveParents, exists, world);
						if (exists)
						{
							AddTaggedValue(value, splitWorld);
						}
					}
				}
				else
				{
					bool exists = false;
					SNI_World* splitWorld = vsWorldSet->JoinWorldsArgs(AutoAddWorld, CreateIfActiveParents, exists, world);
					if (exists)
					{
						AddTaggedValue(loopValue, splitWorld);
					}
				}
			}
		}
		vsWorldSet->Complete();
		m_WorldSet = vsWorldSet;
	}

	SN::SN_Expression SNI_ValueSet::CommonValue()
	{
		if (m_ValueList.size() == 0)
		{
			return skynet::Null;
		}
		SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
		if (contextWorld)
		{
			return skynet::Null;
		}
		SN::SN_Expression result = skynet::Null;
		for (const SNI_TaggedValue &tv : m_ValueList)
		{
			SNI_World *world = tv.GetWorld();
			if (!world || !world->IsEmpty())
			{
				if (tv.GetValue().IsNullValue())
				{
					return skynet::Null;
				}
				if (result.IsNull())
				{
					result = tv.GetValue();
				}
				else
				{
					if (!result.Equivalent(tv.GetValue()))
					{
						return skynet::Null;
					}
				}
			}
		}
		return result;
	}

	bool SNI_ValueSet::ExtractBooleanValue(bool &p_Value)
	{
		if (m_ValueList.size() == 0)
		{
			return false;
		}
		SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
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
		if (NeedsFlattening())
		{
			FlattenValueSets();
		}
	}

	void SNI_ValueSet::Validate()
	{
		CheckWorldSetConsistency();
	}

	SN::SN_Expression SNI_ValueSet::SimplifyValue()
	{
		SN::SN_Expression result = CommonValue();
		if (!result.IsNull())
		{
			return result;
		}
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
		SNI_Thread::GetThread()->Lock();
		for (SNI_TaggedValueList::iterator it = m_ValueList.begin(); it != m_ValueList.end();)
		{
			SNI_World *world = it->GetWorld();
			if (!world || !world->IsEmpty())
			{
				SN::SN_Expression loopValue = it->GetValue().GetSafeValue();
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
		SNI_Thread::GetThread()->Unlock();
		Validate();
		if (!found)
		{
			if (loopError.IsNull())
			{
				return SN::SN_Error(false, false, GetTypeName() + " has no values.");
			}
			return loopError;
		}
		SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
		if (oneValue && !contextWorld)
		{
			return value;
		}
		return this;
	}

	SN::SN_Error SNI_ValueSet::AssertValue(const SN::SN_Expression &p_Value)
	{
		LOGGING(SN::LogContext context("SNI_ValueSet::AssertValue ( " + p_Value.DisplayValueSN() + " )"));

		SN::SN_Expression *paramList = new SN::SN_Expression[2];
		paramList[0] = p_Value;
		paramList[1] = this;
		SN::SN_Error e = skynet::Same.GetSNI_FunctionDef()->UnifyArray(paramList, this).GetError();
		if (e.IsSignificantError())
		{
			SNI_CallRecord *callRecord = new SNI_CallRecord("Assert value set to value.", this);
			LOGGING(callRecord->SetLogContext(context));
			e.GetSNI_Error()->AddNote(callRecord);
		}
		delete[] paramList;
		return e;
	}

	SNI_WorldSet * SNI_ValueSet::GetWorldSet()
	{
		if (!m_WorldSet && !SN::SN_Transaction::InWebServer())
		{
			m_WorldSet = new SNI_WorldSet();
		}
		return m_WorldSet;
	}

	void SNI_ValueSet::SetWorldSet(SNI_WorldSet * p_WorldSet)
	{
		m_WorldSet = p_WorldSet;
	}

	void SNI_ValueSet::AddVariables(long p_MetaLevel, SNI_VariablePointerMap& p_Map)
	{
		for (const SNI_TaggedValue& tv : m_ValueList)
		{
			SNI_Expression *v = tv.GetValue().GetSNI_Expression();
			if (v)
			{
				v->AddVariables(p_MetaLevel, p_Map);
			}
		}
	}

	SN::SN_Expression SNI_ValueSet::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel) const
	{
		SN::SN_ValueSet vs;
		SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
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
		LOGGING(SN::LogContext context("SNI_ValueSet::Unify ( " + DisplaySnExpressionList(p_ParameterList) + " )"));
		
		Validate();
		SN::SN_Error err = skynet::OK;
		bool success = false;
		SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
		SNI_WorldSet *worldSet = GetWorldSet();
		SN::SN_Expression &value = (*p_ParameterList)[0];
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
					SN::SN_ExpressionList *paramListClone = new SN::SN_ExpressionList();
					*paramListClone = *p_ParameterList;
					// Flatten the call stack, by returning the function to be called from Unify, instead of calling it there.
					SNI_Expression *function = tv.GetValue().GetSNI_Expression()->Clone(this, NULL);
					SNI_Error *e = dynamic_cast<SNI_Error *>(function);
					SNI_Thread::GetThread()->PushContextWorld(world);
					while (!e)
					{
						if (!paramListClone)
						{
							paramListClone = new SN::SN_ExpressionList();
							paramListClone->push_back(value);
						}
						SNI_FunctionDef *functionDef = dynamic_cast<SNI_FunctionDef *>(function);
						if (functionDef)
						{
							SN::SN_Expression *param_List = functionDef->LoadParametersUnify(paramListClone);
							delete paramListClone;
							paramListClone = NULL;
							function = functionDef->UnifyArray(param_List, this).GetSNI_Expression();
							delete[] param_List;
						}
						else
						{
							if (function->IsValue() && paramListClone->size() == 1)
							{
								SN::SN_Expression *paramList = new SN::SN_Expression[2];
								paramList[0] = value;
								paramList[1] = function;
								function = skynet::Same.GetSNI_FunctionDef()->UnifyArray(paramList, this).GetSNI_Expression();;
								delete[] paramList;
							}
							else
							{
								function = function->Unify(paramListClone).GetSNI_Expression();
							}
						}
						e = dynamic_cast<SNI_Error *>(function);
					}
					SNI_Thread::GetThread()->PopContextWorld();
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
		Breakpoint(SN::DebugStop, SN::LeftId, GetTypeName(), "Unify", this, SN::CallPoint);
		if (success)
		{
			return SN::SN_Error(success, false);
		}
		if (err.IsSignificantError())
		{
			SNI_CallRecord *callRecord = new SNI_CallRecord("No function from the valueset unified successfully.", this);
			LOGGING(callRecord->SetLogContext(context));
			err.GetSNI_Error()->AddNote(callRecord);
		}
		return err;
	}

	SN::SN_Error SNI_ValueSet::ForEach(std::function<SN::SN_Error(const SN::SN_Expression &p_Param, SNI_World *p_World)> p_Action)
	{
		SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
		SNI_WorldSet *worldSet = GetWorldSet();
		SN::SN_Error e;
		for (SNI_TaggedValue &tv : m_ValueList)
		{

			SNI_World *world = tv.GetWorld();
			if (!contextWorld || contextWorld->CompatibleWorld(world))
			{
				SN::SN_Expression l_Value = tv.GetValue();
				if (!world && worldSet)
				{
					world = worldSet->CreateWorldForValue(l_Value);
					tv.SetWorld(world);
				}
				e.AddError(p_Action(l_Value, world));
			}
		}
		if (worldSet)
		{
			worldSet->Complete();
			Validate();
		}
		return e;
	}

	SN::SN_Error SNI_ValueSet::ForEachCart(long p_Depth, SNI_Cart *p_Cart)
	{
		SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
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
		return skynet::OK;
	}

	void SNI_ValueSet::ForEachSplit(SNI_Splitter * p_Splitter)
	{
		SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
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
		SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
		size_t card = 0;
		if (contextWorld)
		{
			for (const SNI_TaggedValue &tv : m_ValueList)
			{
				SNI_World *world = tv.GetWorld();
				if (!contextWorld || contextWorld->CompatibleWorld(world))
				{	// Assume every tagged value is a known value.
					// Some may not be that can dealt with later when processing 
					// with the individual values.
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

	SN::SN_Expression SNI_ValueSet::DoEvaluate(long /* p_MetaLevel /* = 0 */) const
	{
		return this;
	}

	SN::SN_Expression SNI_ValueSet::DoPartialEvaluate(long /* p_MetaLevel = 0 */) const
	{
		return this;
	}

	SN::SN_ValueSet SNI_ValueSet::DoRemove(const SN::SN_Value &p_Other)
	{
		LOGGING(SN::LogContext context("SNI_ValueSet::DoRemove ( " + DisplaySnTaggedValueList(m_ValueList) + " )"));

		SN::SN_ValueSet valueSet;
		SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
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
		SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
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

	SNI_TaggedValue &SNI_ValueSet::operator[](size_t p_index)
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
			return p_Value.GetError();
		}
		bool exists = false;
		SNI_WorldSet *l_WorldSet = p_WorldSet;
		if (!l_WorldSet)
		{
			l_WorldSet = GetWorldSet();
		}
		m_WorldSet = l_WorldSet;
		SNI_TaggedValueList &valueList = m_ValueList;
		return p_Value.ForEach(
			[l_WorldSet, &exists, p_NumWorlds, p_WorldList, p_Value, &valueList] (const SN::SN_Expression &p_Param, SNI::SNI_World *p_World) -> SN::SN_Error
			{
				SNI_World *world = l_WorldSet->JoinWorldsArray(AutoAddWorld, AlwaysCreateWorld, exists, p_NumWorlds, p_WorldList, p_World);
				if (exists)
				{
					valueList.push_back(SNI_TaggedValue(p_Param, world));
				}
				else
				{
				}
				return skynet::OK;
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

	bool SNI_ValueSet::GetBool() const
	{
		return CompareValue(skynet::True);
	}

	SN::SN_Value SNI_ValueSet::DoOr(SNI_Expression * p_Other) const
	{
		return SNI_ValueSet::GeneralDoOr(this, p_Other);
	}

	/*static*/SN::SN_Value SNI_ValueSet::GeneralDoOr(const SNI_Value *p_Left, SNI_Expression *p_Right)
	{
		SN::SN_Value left_value(p_Left);
		SN::SN_Value right_value(p_Right);
		SN::SN_ValueSet result;
		SNI_WorldSet* worldSet = result.GetWorldSet();

		if (SN::Is<SNI_ValueSet*>(left_value))
		{
			SN::SN_ValueSet leftValueSet = left_value;
			for (size_t j = 0; j < leftValueSet.Length(); j++)
			{
				bool exists = false;
				SNI_World* world = worldSet->JoinWorldsArgs(AutoAddWorld, CreateIfActiveParents, exists, leftValueSet[j].GetWorld());
				if (exists)
				{
					result.AddTaggedValue(leftValueSet[j].GetValue(), world);
				}
			}
		}
		else
		{
			result.AddTaggedValue(left_value, worldSet->CreateWorld());
		}

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
			result.AddTaggedValue(right_value, worldSet->CreateWorld());
		}
		result.GetSNI_ValueSet()->Complete();
		return result;
	}

	SN::SN_Value SNI_ValueSet::DoIf(SNI_Expression * p_PositiveCase, SNI_Expression *p_NegativeCase) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + "SNI_ValueSet::DoIf ( " + SN::SN_Expression(p_PositiveCase).DisplaySN() + SN::SN_Expression(p_NegativeCase).DisplaySN() + " )"));

		SN::SN_ValueSet result;
		SNI_WorldSet * worldSet = result.GetSNI_ValueSet()->GetWorldSet();

		for (size_t i = 0; i<m_ValueList.size(); i++)
		{
			if (m_ValueList[i].GetValue().GetBool())
			{   // Positive case

				SN::SN_Expression l_PositiveExpression = p_PositiveCase->DoEvaluate();

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

				SN::SN_Expression l_NegativeExpression = p_NegativeCase->DoEvaluate();

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
		SNI_World *contextWorld = SNI_Thread::GetThread()->ContextWorld();
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
		}
		return SN::SN_Bool(value);
	}
}
