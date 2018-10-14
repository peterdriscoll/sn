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
			m_Class = new SNI_Class();
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
		for (auto &entry : m_Vector)
		{
			REQUESTPROMOTION(entry);
		}
	}

	string SNI_Virtual::GetTypeName() const
	{
		return "Derived";
	}

	string SNI_Virtual::DisplayCpp() const
	{
		return GetTypeName() + "()";
	}

	string SNI_Virtual::DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const
	{
		return GetTypeName() + "()";
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
				size_t size = m_Vector.size();
				if (size != l_vector->m_Vector.size())
				{
					return false;
				}
				
				for (size_t j =0; j < size; j++)
				{
					if (!m_Vector[j]->Equivalent(l_vector->m_Vector[j]))
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
		return _Hash_representation(m_Vector);
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
		m_Fixed = true;
	}

	SNI_Expression * SNI_Virtual::Clone(SNI_Frame *p_Frame, bool &p_Changed)
	{
		bool changed = false;

		SNI_Virtual *l_clone = new SNI_Virtual();
		if (m_Fixed)
		{
			l_clone->Fix();
		}
		l_clone->m_Vector.resize(m_Vector.size());
		for (size_t j = 0; j < m_Vector.size(); j++)
		{
			SNI_Expression *item = m_Vector[j];
			if (item)
			{
				l_clone->m_Vector[j] = item->Clone(p_Frame, changed);
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
			return SN::SN_Error(GetTypeName() + " Fix the derived calls. There maybe be more defines, so the call is undefined.");
		}
		SN::SN_Expression finalResult;
		for (auto &item : m_Vector)
		{
			if (item && !item->IsNull())
			{
				SNI_Thread::GetThread()->DebugCommand(SN::CallPoint, "Derived.Call");
				SN::SN_ExpressionList paramListClone = *p_ParameterList;
				SN::SN_Expression result = item->Call(&paramListClone, p_MetaLevel);
				if (!result.IsNull())
				{
					return result;
				}
			}
			else
			{
				return SN::SN_Error(GetTypeName() + " function to call is unknown.");
			}
		}
		return SN::SN_Error(GetTypeName() + " Call did not give a value.");
	}

	SN::SN_Expression SNI_Virtual::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Virtual::PartialCall ( " + DisplayPmExpressionList(p_ParameterList) + " )");

		if (!m_Fixed)
		{
			return SN::SN_Error(GetTypeName() + " Fix the derived calls. There maybe be more defines, so the define is undefined.");
		}
		for (auto &item : m_Vector)
		{
			if (item)
			{
				if (!SN::SN_Expression(item).GetSNI_Lambda())
				{
					SN::SN_ExpressionList paramListClone = *p_ParameterList;
					SN::SN_Expression result = item->PartialCall(&paramListClone, p_MetaLevel);
					if (!result.IsNull())
					{
						return result;
					}
				}
			}
			else
			{
				return SN::SN_Error(GetTypeName() + " partial function to call is unknown.");
			}
		}
		return SN::SN_Error(GetTypeName() + " PartialCall did not give a value.");
	}

	SN::SN_Expression SNI_Virtual::Unify(SN::SN_ExpressionList * p_ParameterList)
	{
		if (m_Fixed)
		{
			for (auto item : m_Vector)
			{
				if (item && !item->IsNull())
				{
					SNI_Thread::GetThread()->DebugCommand(SN::CallPoint, GetTypeName() + ".Unify after clone");
					SN::SN_ExpressionList paramListClone = *p_ParameterList;
					SN::SN_Expression e = item->Unify(&paramListClone);
					SNI_Thread::GetThread()->DebugCommand(SN::CallPoint, GetTypeName() + ".Unify after unify");
					if (!e.IsNull())
					{
						return e;
					}
					SNI_Variable *result = SNI_Frame::Top()->GetResult();
					result->SetValue((*p_ParameterList)[0].GetVariableValue());
				}
				else
				{
					return SN::SN_Error(GetTypeName() + " function to unify is unknown.");
				}
			}
			return skynet::OK;
		}
		else
		{
			m_Vector.push_back(AddLambdas(p_ParameterList).GetSNI_Expression());
			REQUESTPROMOTION(m_Vector.back());
			return skynet::OK;
		}
	}

	SN::SN_Error SNI_Virtual::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		if (m_Fixed)
		{
			for (auto item : m_Vector)
			{
				if (item && !item->IsNull())
				{
					SNI_Thread::GetThread()->DebugCommand(SN::CallPoint, GetTypeName() + ".Unify after clone");
					SN::SN_ParameterList paramListClone = *p_ParameterList;
					SN::SN_Expression e = item->PartialUnify(&paramListClone, p_Result, p_Define);
					SNI_Thread::GetThread()->DebugCommand(SN::CallPoint, GetTypeName() + ".Unify after unify");
					//SNI_Variable *result = SNI_Frame::Top()->GetResult();
					//result->SetValue((*p_ParameterList)[0].GetVariableValue());
					SNI_Frame::Pop();
					if (!e.IsNull())
					{
						return e;
					}
				}
				else
				{
					return SN::SN_Error(GetTypeName() + " function to unify is unknown.");
				}
			}
			return skynet::OK;
		}
		else
		{
			long l_DefineId = SNI_Thread::GetThread()->GetDefineId();
			if (m_DefineId != l_DefineId)
			{
				m_DefineId = l_DefineId;
				m_Vector.push_back(AddLambdasPartial(p_ParameterList, p_Result).GetSNI_Expression());
				REQUESTPROMOTION(m_Vector.back());
			}
			else if (m_Vector.back() == NULL || p_Define)
			{
				m_Vector.back() = AddLambdasPartial(p_ParameterList, p_Result).GetSNI_Expression();
				REQUESTPROMOTION(m_Vector.back());
			}
			return skynet::OK;
		}
	}
}
