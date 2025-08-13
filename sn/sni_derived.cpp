#include "sni_derived.h"

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
	/*static*/ SNI_Class *SNI_Derived::m_Class = NULL;
	/*static*/ SNI_Class *SNI_Derived::Class()
	{
		if (!m_Class)
		{
			m_Class = new SNI_Class("Derived");
		}
		return m_Class;
	}

	SNI_Class *SNI_Derived::VClass()
	{
		return Class();
	}

	SNI_Derived::SNI_Derived()
	: m_Fixed(false)
	, m_DefineId(0)
	{
	}

	SNI_Derived::~SNI_Derived()
	{
	}

	SNI_Expression * SNI_Derived::Copy() const
	{
		return new SNI_Derived(*this);
	}

	void SNI_Derived::PromoteMembers()
	{
		for (auto &entry : m_Vector)
		{
			REQUESTPROMOTION(entry);
		}
	}

	std::string SNI_Derived::GetTypeName() const
	{
		return "Derived";
	}

	std::string SNI_Derived::DisplayCpp() const
	{
		return GetTypeName() + "()";
	}

	std::string SNI_Derived::DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const
	{
		if (p_DisplayOptions.GetLevel() == 0)
		{
			p_DisplayOptions.IncrementLevel();
			long param = SN::DerivedOneId;
			std::string result = SetBreakPoint(GetTypeName() + "(", p_DisplayOptions, this, param);
			std::string delimeter;
			for (const SN::SN_Expression &call : m_Vector)
			{
				++param;
				result += delimeter + call.GetSNI_Expression()->DisplaySN(priority, p_DisplayOptions);
				delimeter = SetBreakPoint(",", p_DisplayOptions, this, param);
			}
			result += ")";
			p_DisplayOptions.DecrementLevel();
			return result;
		}
		return GetTypeName() + "()";
	}

	long SNI_Derived::GetPriority() const
	{
		return 3;
	}

	bool SNI_Derived::Equivalent(SNI_Object * p_Other) const
	{
		if (m_Fixed)
		{
			if (dynamic_cast<SNI_Derived *>(p_Other))
			{
				SNI_Derived * l_vector = dynamic_cast<SNI_Derived *>(p_Other);
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

	size_t SNI_Derived::Hash() const
	{
		return 0; // _Hash_representation(m_Vector);
	}

	bool SNI_Derived::IsValue() const
	{
		return false;
	}

	bool SNI_Derived::IsFixed() const
	{
		return m_Fixed;
	}

	bool SNI_Derived::IsInline() const
	{
		return false;
	}

	void SNI_Derived::Fix()
	{
		m_Fixed = true;
	}

	SNI_Expression * SNI_Derived::Clone(long p_MetaLevel, SNI_Frame *p_Frame, bool &p_Changed)
	{
		return this;
	}

	SN::SN_Expression SNI_Derived::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Derived::Call ( " + DisplaySnExpressionList(p_ParameterList) + " )"));
		if (!m_Fixed)
		{
			return LOG_RETURN(context, SN::SN_Error(false, false, GetTypeName() + " Fix the derived calls. There maybe be more defines, so the call is undefined."));
		}
		unsigned long id = 0;

		SN::SN_Expression finalResult;
		bool resultFound = false;
		size_t numParams = 0; // p_ParameterList->size();
		for (auto &item : m_Vector)
		{
			if (item && !item->IsNull())
			{
				SNI_Expression * l_clone = item->Clone(this, NULL);
				SN::SN_ExpressionList* paramListClone = new SN::SN_ExpressionList(*p_ParameterList);
				SNI_Frame* topFrame = SNI_Frame::Top();
				Breakpoint(SN::DebugStop, (SN::BreakId)(SN::DerivedOneId + id++), GetTypeName(), "Call hierarchy " + to_string(id), this, SN::CallPoint);

				SN::SN_Expression result = FlattenStackCall(p_MetaLevel, l_clone, paramListClone);

				//if (paramListClone->size() < numParams)
				//{
				//	numParams = paramListClone->size();
				//}

				SNI_Variable* resultVar = topFrame->GetResult();
				resultVar->SetValue(result);
				Breakpoint(SN::DebugStop, (SN::BreakId)(SN::DerivedOneId + id), GetTypeName(), "Call hierarchy " + to_string(id), this, SN::CallPoint);
				SNI_Frame::Pop();

				if (result.IsError())
				{
					return result;
				}
				if (!result.IsNullValue())
				{
					if (result.IsKnownValue())
					{
						if (resultFound)
						{
							if (!finalResult.Equivalent(result.GetSNI_Expression()))
							{
								return LOG_RETURN(context, SN::SN_Error(false, false, GetTypeName() + " Polymorphic calls gave different results. Did you mean to use Virtual instead of Derived?"));
							}
						}
						else
						{
							resultFound = true;
							finalResult = result;
						}
					}
					else
					{
						p_ParameterList->resize(numParams);
						return result;
					}
				}
			}
			else
			{
				return LOG_RETURN(context, SN::SN_Error(false, false, GetTypeName() + " function to call is unknown."));
			}
		}
		p_ParameterList->resize(numParams);
		return LOG_RETURN(context, finalResult);
	}

	SN::SN_Expression SNI_Derived::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Derived::PartialCall ( " + DisplaySnExpressionList(p_ParameterList) + " )"));

		if (!m_Fixed)
		{
			SN::SN_Expression result(this);
			while (!p_ParameterList->empty())
			{
				result = SN::SN_Function(result, p_ParameterList->back().DoPartialEvaluate(p_MetaLevel));
				p_ParameterList->pop_back();
			}
			return LOG_RETURN(context, result);
		}
		SN::SN_Expression finalResult;
		unsigned long id = 0;
		bool resultFound = false;
		for (auto &item : m_Vector)
		{
			if (item)
			{
				if (!SN::SN_Expression(item).GetSNI_Lambda())
				{
					SN::SN_ExpressionList paramListClone = *p_ParameterList;
					SN::SN_Expression result = item->PartialCall(&paramListClone, p_MetaLevel);
					if (result.IsError())
					{
						return LOG_RETURN(context, result);
					}
					if (result.IsKnownValue())
					{
						if (resultFound)
						{
							if (!finalResult.Equivalent(result.GetSNI_Expression()))
							{
								return LOG_RETURN(context, SN::SN_Error(false, false, GetTypeName() + " Polymorphic calls gave different results. Did you mean to use Virtual instead of Derived?"));
							}
						}
						else
						{
							resultFound = true;
							finalResult = result;
						}
					}
				}
			}
			else
			{
				return LOG_RETURN(context, SN::SN_Error(false, false, GetTypeName() + " partial function to call is unknown."));
			}
		}
		return finalResult;
	}

	SN::SN_Expression SNI_Derived::Unify(SN::SN_ExpressionList * p_ParameterList)
	{
		if (m_Fixed)
		{
			unsigned long id = 0;
			for (auto item : m_Vector)
			{
				if (item && !item->IsNull())
				{
					Breakpoint(SN::DebugStop, (SN::BreakId)(SN::DerivedOneId + id), GetTypeName(), "Unify hierarchy " + to_string(id), this, SN::CallPoint);
					++id;
					SNI_Expression * l_clone = item->Clone(this, NULL);
					SN::SN_ExpressionList paramListClone = *p_ParameterList;
					SN::SN_Expression e = l_clone->Unify(&paramListClone);
					if (e.IsError())
					{
						return e;
					}
				}
				else
				{
					return SN::SN_Error(false, false, GetTypeName() + " function to unify is unknown.");
				}
			}
			Breakpoint(SN::DebugStop, SN::ReturnId, GetTypeName(), "End hierarchy", NULL, SN::CallPoint);
			return skynet::OK;
		}
		else
		{
			m_Vector.push_back(AddLambdas(p_ParameterList).GetSNI_Expression());
			REQUESTPROMOTION(m_Vector.back());
			return skynet::OK;
		}
	}

	SN::SN_Error SNI_Derived::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		if (m_Fixed)
		{
			unsigned long id = 0;

			for (auto item : m_Vector)
			{
				if (item && !item->IsNull())
				{
					SN::SN_ParameterList paramListClone = *p_ParameterList;
					SN::SN_Expression e = item->PartialUnify(&paramListClone, p_Result, p_Define);
					//SNI_Variable *result = SNI_Frame::Top()->GetResult();
					//result->SetValue((*p_ParameterList)[0].GetVariableValue());
					SNI_Frame::Pop();
					if (e.IsError())
					{
						return e.GetError();
					}
				}
				else
				{
					return SN::SN_Error(false, false, GetTypeName() + " function to unify is unknown.");
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
