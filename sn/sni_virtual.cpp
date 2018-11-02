#include "sni_virtual.h"

#include "logcontext.h"
#include "sn_expression.h"
#include "sn_parameter.h"
#include "sn_value.h"
#include "sn_error.h"

#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	ConstructionTree::ConstructionTree(const string &p_ParameterName)
		: m_ParameterVariable(p_ParameterName)
	{
	}

	ConstructionTree::ConstructionTree(const string &p_ParameterName, SN::SN_Expression p_Parameter)
		: m_ParameterVariable(p_ParameterName)
		, m_Parameter(p_Parameter)
	{
	}

	ConstructionTree::ConstructionTree(const string &p_ParameterName, SN::SN_Expression p_Parameter, SN::SN_Expression p_ImplementationCall)
		: m_ParameterVariable(p_ParameterName)
		, m_Parameter(p_Parameter)
		, m_ImplementationCall(p_ImplementationCall)
	{
	}

	ConstructionTree::~ConstructionTree()
	{
	}

	void ConstructionTree::AddImplementation(SN::SN_Expression p_Implementation)
	{
		SN::SN_ExpressionList l_FormalParameterList;
		SN::SN_Expression l_CalledExpression = p_Implementation.GetSNI_Expression()->LoadFormalParameters(l_FormalParameterList);
		AddParameterList(0, l_FormalParameterList, p_Implementation);
	}

	SN::SN_Expression ConstructionTree::CreateExpression()
	{
		return BuildExpression(0);
	}

	SN::SN_Expression ConstructionTree::GetParameter()
	{
		return m_Parameter;
	}

	void ConstructionTree::AddParameterList(size_t p_Base, SN::SN_ExpressionList & p_FormalParameterList, SN::SN_Expression p_ImplementationCall)
	{
		SN::SN_Expression l_Expression = p_FormalParameterList[p_Base].GetVariableValue();
		list<ConstructionTree>::iterator it = m_List.begin();
		while (it != m_List.end() && !l_Expression.IsA(it->GetParameter()).GetBool())
		{
			++it;
		}
		size_t base = p_Base + 1;
		string paramName = "_param_" + to_string(base);
		SN::SN_Expression l_ImplentationCall = p_ImplementationCall(m_ParameterVariable);
		if (p_Base == p_FormalParameterList.size() - 1)
		{
			m_List.insert(it, ConstructionTree(paramName, l_Expression, l_ImplentationCall));
		}
		else
		{
			list<ConstructionTree>::iterator pos = m_List.insert(it, ConstructionTree(paramName, l_Expression));
			pos->AddParameterList(base, p_FormalParameterList, l_ImplentationCall);
		}
	}

	SN::SN_Expression ConstructionTree::BuildExpression(size_t p_Depth)
	{
		SN::SN_Error err(string("Virtual - No polymorphic implementation found for parameters."));
		SN::SN_Expression callExpression = m_ImplementationCall;
		if (callExpression.IsNull())
		{
			callExpression = err;
		}
		for (auto it = m_List.rbegin(); it != m_List.rend(); it++)
		{
			callExpression = it->BuildCondition(p_Depth, callExpression, m_ParameterVariable);
		}
		if (m_List.size())
		{
			return SN::SN_Lambda(m_ParameterVariable, callExpression);
		}
		return callExpression;
	}

	SN::SN_Expression ConstructionTree::BuildCondition(size_t p_Depth, SN::SN_Expression p_ElseCondition, SN::SN_Variable p_ParameterVariable)
	{
		return p_ParameterVariable.IsA(m_Parameter).If(BuildExpression(p_Depth - 1), p_ElseCondition);
	}

	/*static*/ SNI_Class *SNI_Virtual::m_Class = NULL;
	/*static*/ SNI_Class *SNI_Virtual::Class()
	{
		if (!m_Class)
		{
			m_Class = new SNI_Class("Virtual");
		}
		return m_Class;
	}

	SNI_Virtual::SNI_Virtual()
	: m_Fixed(false)
	, m_DefineId(0)
	{
	}

	SNI_Virtual::~SNI_Virtual()
	{
	}

	void SNI_Virtual::PromoteMembers()
	{
		for (auto &entry : m_CallList)
		{
			REQUESTPROMOTION(entry);
		}
	}

	string SNI_Virtual::GetTypeName() const
	{
		return "Virtual";
	}

	string SNI_Virtual::DisplayCpp() const
	{
		if (m_Fixed)
		{
			return m_CallExpression.DisplayCpp();
		}
		string result = GetTypeName() + "(";
		string delimeter;
		for (const SN::SN_Expression &call : m_CallList)
		{
			result += delimeter + call.DisplayCpp();
			delimeter = ",";
		}
		result += ")";
		return result;
	}

	string SNI_Virtual::DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const
	{
		if (m_Fixed)
		{
			return m_CallExpression.GetSNI_Expression()->DisplaySN(priority, p_DisplayOptions);
		}
		string result = GetTypeName() + "(";
		string delimeter;
		for (const SN::SN_Expression &call : m_CallList)
		{
			result += delimeter + call.GetSNI_Expression()->DisplaySN(priority, p_DisplayOptions);
			delimeter = ",";
		}
		result += ")";
		return result;
	}

	long SNI_Virtual::GetPriority() const
	{
		return 3;
	}

	bool SNI_Virtual::Equivalent(SNI_Object * p_Other) const
	{
		if (m_Fixed)
		{
			if (dynamic_cast<SNI_Virtual *>(p_Other))
			{
				SNI_Virtual * l_vector = dynamic_cast<SNI_Virtual *>(p_Other);
				size_t size = m_CallList.size();
				if (size != l_vector->m_CallList.size())
				{
					return false;
				}
				
				for (size_t j =0; j < size; j++)
				{
					if (!m_CallList[j].GetSNI_Expression()->Equivalent(l_vector->m_CallList[j].GetSNI_Expression()))
					{
						return false;
					}
				}
			}
		}
		return false;
	}

	size_t SNI_Virtual::Hash() const
	{
		return _Hash_representation(m_CallList);
	}

	bool SNI_Virtual::IsFixed() const
	{
		return m_Fixed;
	}

	// Inheritance
	SN::SN_Value SNI_Virtual::DoIsA(const SNI_Value * p_Parent) const
	{
		return Class()->DoIsA(p_Parent);
	}

	void SNI_Virtual::Fix()
	{
		if (!m_Fixed)
		{
			BuildImplementation();
		}
		m_Fixed = true;
	}

	ConstructionTree * SNI_Virtual::BuildTree()
	{
		ConstructionTree *base = new ConstructionTree("_param_0");
		for (SN::SN_Expression &e : m_CallList)
		{
			base->AddImplementation(e);
		}
		return base;
	}

	SN::SN_Expression SNI_Virtual::CreateImplementation()
	{
		if (m_CallList.size() == 0)
		{
			return SN::SN_Error(GetTypeName() + " - No function definitions.");
		}
		ConstructionTree *tree = BuildTree();
		return tree->CreateExpression();
	}

	void SNI_Virtual::BuildImplementation()
	{
		m_CallExpression = CreateImplementation();
	}

	SNI_Expression * SNI_Virtual::Clone(SNI_Frame *p_Frame, bool &p_Changed)
	{
		if (m_Fixed)
		{
			return m_CallExpression.GetSNI_Expression()->Clone(p_Frame, p_Changed);
		}

		bool changed = false;

		SNI_Virtual *l_clone = new SNI_Virtual();
		l_clone->m_CallList.resize(m_CallList.size());
		for (size_t j = 0; j < m_CallList.size(); j++)
		{
			SN::SN_Expression item = m_CallList[j];
			if (!item.IsNull())
			{
				l_clone->m_CallList[j] = item.GetSNI_Expression()->Clone(p_Frame, changed);
			}
		}

		if (changed)
		{
			p_Changed = true;
			return dynamic_cast<SNI_Expression *>(l_clone);
		}
		return this;
	}

	SN::SN_Expression SNI_Virtual::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Virtual::Call ( " + DisplayPmExpressionList(p_ParameterList) + " )");
		if (!m_Fixed)
		{
			return SN::SN_Error(GetTypeName() + " Fix the virtual calls. There maybe be more defines, so the call is undefined.");
		}
		return m_CallExpression.GetSNI_Expression()->Call(p_ParameterList, p_MetaLevel);
	}

	SN::SN_Expression SNI_Virtual::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Virtual::PartialCall ( " + DisplayPmExpressionList(p_ParameterList) + " )");

		if (!m_Fixed)
		{
			return SN::SN_Error(GetTypeName() + " Fix the virtual calls. There maybe be more defines, so the define is undefined.");
		}
		return m_CallExpression.GetSNI_Expression()->PartialCall(p_ParameterList, p_MetaLevel);
	}

	SN::SN_Expression SNI_Virtual::Unify(SN::SN_ExpressionList * p_ParameterList)
	{
		if (m_Fixed)
		{
			return m_CallExpression.GetSNI_Expression()->Unify(p_ParameterList);
		}
		else
		{
			m_CallList.push_back(AddLambdas(p_ParameterList).GetSNI_Expression());
			REQUESTPROMOTION(m_CallList.back());
			return skynet::OK;
		}
	}

	SN::SN_Error SNI_Virtual::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		if (m_Fixed)
		{
			return m_CallExpression.GetSNI_Expression()->PartialUnify(p_ParameterList, p_Result, p_Define);
		}
		else
		{
			long l_DefineId = SNI_Thread::GetThread()->GetDefineId();
			if (m_DefineId != l_DefineId)
			{
				m_DefineId = l_DefineId;
				m_CallList.push_back(AddLambdasPartial(p_ParameterList, p_Result).GetSNI_Expression());
				REQUESTPROMOTION(m_CallList.back());
			}
			else if ((m_CallList.back().IsNull()) || p_Define)
			{
				m_CallList.back() = AddLambdasPartial(p_ParameterList, p_Result).GetSNI_Expression();
				REQUESTPROMOTION(m_CallList.back());
			}
			return skynet::OK;
		}
	}
}
