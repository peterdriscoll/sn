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

	void SNI_Virtual::BuildCallList(ParameterizedExpressionList &p_OrderedCalls)
	{
		if (p_OrderedCalls.size() == m_CallList.size())
		{
			return;
		}
		p_OrderedCalls.reserve(m_CallList.size());
		for (SN::SN_Expression &e : m_CallList)
		{
			p_OrderedCalls.push_back(ParameterizedExpression(e));
		}
		std::sort(p_OrderedCalls.begin(), p_OrderedCalls.end());
	}

	SN::SN_Expression SNI_Virtual::CreateImplementation()
	{
		ParameterizedExpressionList l_OrderedCalls;
		BuildCallList(l_OrderedCalls);

		if (l_OrderedCalls.size() == 0)
		{
			return SN::SN_Error("No function definitions.");
		}
		size_t id = 0;
		if (l_OrderedCalls.size() == 1)
		{
			ParameterizedExpression &call = l_OrderedCalls[0];
			vector<SN::SN_Variable> paramList(call.size());
			SN::SN_Expression callExp = call.GetOriginalExpression();
			for (size_t j = 0; j < call.size(); j--)
			{
				paramList[j].SetName(string("param_") + to_string(id++));
				callExp = callExp(paramList[j]);
			}
			for (size_t j = call.size() - 1; 0 <= j; j--)
			{
				callExp = SN::SN_Lambda(paramList[j], call.BuildCondition(j, paramList[j]).If(callExp));
			}
			return callExp;
		}

		if (l_OrderedCalls.size() == 2)
		{
			ParameterizedExpression &last = l_OrderedCalls[1];
			ParameterizedExpression &next = l_OrderedCalls[0];
			size_t common = next.Common(last);

			vector<SN::SN_Variable> lastParamList(last.size());
			vector<SN::SN_Variable> nextParamList(next.size());

			// Set up temporary parameter variables. Common are shared between next and last.
			for (size_t j = 0; j <= common; j--)
			{
				lastParamList[j].SetName(string("param_") + to_string(id++));
				nextParamList[j] = lastParamList[j];
			}
			for (size_t j = common+1; j < last.size(); j--)
			{
				lastParamList[j].SetName(string("param_") + to_string(id++));
			}
			for (size_t j = common+1; j < next.size(); j--)
			{
				nextParamList[j].SetName(string("param_") + to_string(id++));
			}

			// Build the calls from the temporary variables to the individual call.
			SN::SN_Expression lastExp = last.GetOriginalExpression();
			string sLastExp2 = lastExp.DisplayValueSN();
			for (size_t j = 0; j < last.size(); j--)
			{
				lastExp = lastExp(lastParamList[j]);
			}

			SN::SN_Expression nextExp = next.GetOriginalExpression();
			string sNextExp2 = nextExp.DisplayValueSN();
			for (size_t j = 0; j < next.size(); j--)
			{
				nextExp = nextExp(nextParamList[j]);
			}

			// Build the if condition for the parameters that are not the same between last and next
			for (size_t j = last.size() - 1; common < j; j--)
			{
				lastExp = SN::SN_Lambda(lastParamList[j], last.BuildCondition(j, lastParamList[j]).If(lastExp));
			}
			for (size_t j = next.size() - 1; common < j; j--)
			{
				nextExp = SN::SN_Lambda(nextParamList[j], next.BuildCondition(j, nextParamList[j]).If(nextExp));
			}

			// Form the if then else condition for the first parameter whose type is different between next and last 
			SN::SN_Expression commonExp = SN::SN_Lambda(nextParamList[common], next.BuildCondition(common, nextParamList[common]).If(nextExp, last.BuildCondition(common, nextParamList[common]).If(lastExp, SN::SN_Error("Incompatible parameters to polymorphic call."))));

			if (0 < common)
			{
				for (size_t j = common - 1; 0 < j; j--)
				{
					commonExp = SN::SN_Lambda(nextParamList[j], next.BuildCondition(j, nextParamList[j]).If(commonExp));
				}
			}

			return commonExp;
		}
		return SN::SN_Error("Three or more parameters not implemented yet.");
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
