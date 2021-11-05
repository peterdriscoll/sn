#include "sni_incompletefunction.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sn_expression.h"
#include "sn_parameter.h"
#include "sn_value.h"
#include "sn_error.h"

#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ SNI_Class* SNI_IncompleteFunction::m_Class = NULL;
	/*static*/ SNI_Class* SNI_IncompleteFunction::Class()
	{
		if (!m_Class)
		{
			m_Class = new SNI_Class("IncompleteFunction");
		}
		return m_Class;
	}

	SNI_Class* SNI_IncompleteFunction::VClass()
	{
		return Class();
	}

	SNI_IncompleteFunction::SNI_IncompleteFunction()
		: m_Function(NULL)
	{
	}

	SNI_IncompleteFunction::SNI_IncompleteFunction(SNI_Expression* p_Function)
		: m_Function(p_Function)
	{
		REQUESTPROMOTION(m_Function);
	}

	SNI_IncompleteFunction::SNI_IncompleteFunction(const SN::SN_Expression &p_Expression)
		: m_Function(p_Expression.GetSNI_Expression())
	{
		REQUESTPROMOTION(m_Function);
	}

	SNI_IncompleteFunction::SNI_IncompleteFunction(const SNI_IncompleteFunction &p_Function)
	{
		m_Function = p_Function.m_Function;
		REQUESTPROMOTION(m_Function);
	}

	SNI_IncompleteFunction::~SNI_IncompleteFunction()
	{

	}

	void SNI_IncompleteFunction::PromoteMembers()
	{
		REQUESTPROMOTION(m_Function);
	}

	string SNI_IncompleteFunction::GetTypeName() const
	{
		return "IncompleteFunction";
	}

	string SNI_IncompleteFunction::GetReferredName() const
	{
		if (m_Function)
		{
			return GetTypeName() + "_" +m_Function->GetReferredName();
		}
		return GetTypeName();
	}

	string SNI_IncompleteFunction::DisplayCpp() const
	{
		return m_Function->DisplayCpp();
	}

	string SNI_IncompleteFunction::DisplaySN(long priority, SNI_DisplayOptions& p_DisplayOptions) const
	{
		return GetTypeName()+"("+m_Function->DisplaySN(priority, p_DisplayOptions)+")";
	}

	SNI_Expression *SNI_IncompleteFunction::LoadParameters(SN::SN_ExpressionList * p_ParameterList) const
	{
		return m_Function->LoadParameters(p_ParameterList);
	}

	void SNI_IncompleteFunction::AddVariables(long p_MetaLevel, SNI_VariablePointerMap& p_Map)
	{
		m_Function->AddVariables(p_MetaLevel, p_Map);
	}

	long SNI_IncompleteFunction::GetPriority() const
	{
		return m_Function->GetPriority();
	}

	SNI_Expression * SNI_IncompleteFunction::GetFunction()
	{
		return dynamic_cast<SNI_Function *>(m_Function)->GetFunction();
	}

	SNI_Expression * SNI_IncompleteFunction::Clone(long p_MetaLevel, SNI_Frame *p_Frame, bool &p_Changed)
	{
		bool changed = false;
		SNI_Expression * l_Function = m_Function->Clone(p_MetaLevel, p_Frame, changed);
		if (changed)
		{
			p_Changed = true;
			return dynamic_cast<SNI_Expression *>(new SNI_IncompleteFunction(l_Function));
		}
		return this;
	}

	bool SNI_IncompleteFunction::Equivalent(SNI_Object * p_Other) const
	{
		return m_Function->Equivalent(p_Other);
	}

	size_t SNI_IncompleteFunction::Hash() const
	{
		return m_Function->Hash();
	}

	SN::SN_Expression SNI_IncompleteFunction::DoEvaluate(long p_MetaLevel /* = 0 */) const
	{
		return m_Function->DoEvaluate(p_MetaLevel);
	}

	SN::SN_Expression SNI_IncompleteFunction::DoPartialEvaluate(long p_MetaLevel /* = 0 */) const
	{
		return m_Function->DoPartialEvaluate(p_MetaLevel);
	}

	SN::SN_Error SNI_IncompleteFunction::DoAssert()
	{
		return DoAssert();
	}

	SN::SN_Error SNI_IncompleteFunction::AssertValue(const SN::SN_Expression &p_Value)
	{
		return m_Function->AssertValue(p_Value);
	}

	SN::SN_Error SNI_IncompleteFunction::AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World ** p_WorldList, SNI_WorldSet * p_WorldSet)
	{
		return m_Function->AddValue(p_Value, p_NumWorlds, p_WorldList, p_WorldSet);
	}

	SN::SN_Error SNI_IncompleteFunction::DoPartialAssert()
	{
		return DoPartialAssert();
	}

	SN::SN_Error SNI_IncompleteFunction::PartialAssertValue(const SN::SN_Expression &p_Expression, bool p_Define)
	{
		return m_Function->PartialAssertValue(p_Expression, p_Define);
	}

	SN::SN_Expression SNI_IncompleteFunction::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		return m_Function->Call(p_ParameterList, p_MetaLevel);
	}

	SN::SN_Expression SNI_IncompleteFunction::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		return m_Function->PartialCall(p_ParameterList, p_MetaLevel);
	}

	SN::SN_Expression SNI_IncompleteFunction::Unify(SN::SN_ExpressionList * p_ParameterList)
	{
		return m_Function->Unify(p_ParameterList);
	}

	SN::SN_Error SNI_IncompleteFunction::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		return m_Function->PartialUnify(p_ParameterList, p_Result, p_Define);
	}
}
