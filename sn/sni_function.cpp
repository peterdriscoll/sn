#include "sni_function.h"

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
	/*static*/ SNI_Class* SNI_Function::m_Class = NULL;
	/*static*/ SNI_Class* SNI_Function::Class()
	{
		if (!m_Class)
		{
			m_Class = new SNI_Class("Function");
		}
		return m_Class;
	}

	SNI_Class* SNI_Function::VClass()
	{
		return Class();
	}

	SNI_Function::SNI_Function()
		: m_Function(NULL)
		, m_Parameter(NULL)
		, m_Condition(NULL)
	{
	}

	SNI_Function::SNI_Function(SNI_Expression *p_Function)
		: m_Function(p_Function)
		, m_Parameter(skynet::Null.GetSNI_Expression())
		, m_Condition(skynet::Null.GetSNI_Expression())
	{
		REQUESTPROMOTION(m_Function);
		REQUESTPROMOTION(m_Parameter);
	}


	SNI_Function::SNI_Function(SNI_Expression *p_Function, SNI_Expression *p_Parameter)
		: m_Function(p_Function)
		, m_Parameter(p_Parameter)
		, m_Condition(skynet::Null.GetSNI_Expression())
	{
		REQUESTPROMOTION(m_Function);
		REQUESTPROMOTION(m_Parameter);
	}

	SNI_Function::SNI_Function(SNI_Expression *p_Function, SNI_Expression *p_Parameter, SNI_Expression *p_Condition, unsigned long p_Id)
		: m_Function(p_Function)
		, m_Parameter(p_Parameter)
		, m_Condition(p_Condition)
		, SNI_VarDef(p_Id)
	{
		REQUESTPROMOTION(m_Function);
		REQUESTPROMOTION(m_Parameter);
		REQUESTPROMOTION(m_Condition);
	}

	SNI_Function::SNI_Function(const SNI_Function &p_Function)
		: SNI_VarDef(p_Function)
	{
		m_Function = p_Function.m_Function;
		m_Parameter = p_Function.m_Parameter;
		m_Condition = p_Function.m_Condition;
		REQUESTPROMOTION(m_Function);
		REQUESTPROMOTION(m_Parameter);
		REQUESTPROMOTION(m_Condition);
	}

	SNI_Function::~SNI_Function()
	{

	}


	SN::SN_Function SNI_Function::Condition(SNI_Expression *p_Condition)
	{
		m_Condition = p_Condition;
		return this;
	}

	void SNI_Function::PromoteMembers()
	{
		REQUESTPROMOTION(m_Function);
		REQUESTPROMOTION(m_Parameter);
		REQUESTPROMOTION(m_Condition);
	}

	string SNI_Function::GetTypeName() const
	{
		return "Function";
	}

	string SNI_Function::GetReferredName() const
	{
		if (m_Function)
		{
			return GetTypeName() + "_" +m_Function->GetReferredName();
		}
		return GetTypeName();
	}

	string SNI_Function::DisplayCpp() const
	{
		return m_Function->DisplayCpp() + "(" + m_Parameter->DisplayCpp() + ")";
	}

	SNI_Expression *SNI_Function::LoadParameters(SN::SN_ExpressionList * p_ParameterList) const
	{
		p_ParameterList->push_back(m_Parameter);
		return m_Function;
	}

	string SNI_Function::DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const
	{
		SN::SN_ExpressionList l_ParameterList;
		SNI_Expression *nextFunction = LoadParameters(&l_ParameterList);
		SNI_Expression *function = NULL;
		while (nextFunction)
		{
			function = nextFunction;
			nextFunction = function->LoadParameters(&l_ParameterList);
		}
		if (function)
		{
			size_t numParams = l_ParameterList.size();
			SN::SN_Expression *param_List = function->LoadParametersCall(&l_ParameterList);
			string result = function->DisplayCall(GetPriority(), p_DisplayOptions, numParams, param_List, this);
			delete[] param_List;
			if (!l_ParameterList.empty())
			{
				for (SN::SN_ExpressionList::reverse_iterator paramIt = l_ParameterList.rbegin(); paramIt != l_ParameterList.rend(); paramIt++)
				{
					result += " " + paramIt->DisplaySN();
				}
			}
			return result;
		}
		return "";
	}

	void SNI_Function::AddVariables(long p_MetaLevel, SNI_VariablePointerMap& p_Map)
	{
		SNI_Expression* function = m_Function;
		if (function)
		{
			function->AddVariables(p_MetaLevel, p_Map);
		}
		SNI_Expression* parameter = m_Parameter;
		if (parameter)
		{
			parameter->AddVariables(p_MetaLevel, p_Map);
		}
		SNI_Expression* condition = m_Condition;
		if (condition)
		{
			condition->AddVariables(p_MetaLevel, p_Map);
		}
	}

	long SNI_Function::GetPriority() const
	{
		return 0;
	}

	SNI_Expression * SNI_Function::GetFunction()
	{
		return m_Function;
	}

	SNI_Expression * SNI_Function::GetParameter()
	{
		return m_Parameter;
	}

	SNI_Expression * SNI_Function::Clone(long p_MetaLevel, SNI_Frame *p_Frame, bool &p_Changed)
	{
		bool changed = false;
		SNI_Expression * l_Function = m_Function->Clone(p_MetaLevel, p_Frame, changed);
		SNI_Expression * l_Parameter = m_Parameter->Clone(p_MetaLevel, p_Frame, changed);
		if (changed)
		{
			p_Changed = true;
			return dynamic_cast<SNI_Expression *>(new SNI_Function(l_Function, l_Parameter, m_Condition, GetId()));
		}
		return this;
	}

	bool SNI_Function::Equivalent(SNI_Object * p_Other) const
	{
		if (dynamic_cast<SNI_Function *>(p_Other))
		{
			SNI_Function * l_function = dynamic_cast<SNI_Function *>(p_Other);
			return m_Function->Equivalent(dynamic_cast<SNI_Object *>(l_function->m_Function))
				&& m_Parameter->Equivalent(dynamic_cast<SNI_Object *>(l_function->m_Parameter));
		}
		return false;
	}

	size_t SNI_Function::Hash() const
	{
		// return _Hash_array_representation(GetFunction()->GetTypeName().c_str(), GetFunction()->GetTypeName().size());
		return 0;
	}

	SN::SN_Expression SNI_Function::DoEvaluate(long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Function::DoEvaluate (" + to_string(p_MetaLevel) + ")"));

		if (0 < p_MetaLevel)
		{
			SN::SN_Expression function = m_Function;
			SN::SN_Expression parameter = m_Parameter;
			return SN::SN_Function(function.DoEvaluate(p_MetaLevel), parameter.DoEvaluate(p_MetaLevel));
		}

		SN::SN_ExpressionList* l_ParameterList = new SN::SN_ExpressionList();
		l_ParameterList->push_back(m_Parameter);

		return FlattenStackCall(p_MetaLevel, m_Function, l_ParameterList);
	}

	SN::SN_Expression SNI_Function::DoPartialEvaluate(long p_MetaLevel /* = 0 */) const
	{
		if (0 < p_MetaLevel)
		{
			SN::SN_Expression function = m_Function;
			SN::SN_Expression parameter = m_Parameter;
			return SN::SN_Function(function.DoPartialEvaluate(p_MetaLevel), parameter.DoPartialEvaluate(p_MetaLevel));
		}

		SN::SN_ExpressionList * l_ParameterList = new SN::SN_ExpressionList();
		l_ParameterList->push_back(m_Parameter);
		SNI_World *world(new SNI_World(NULL)); // Only create a world so that errors are not fatal.
		SNI_Thread::GetThread()->PushContextWorld(world);
		SN::SN_Expression result = m_Function->PartialCall(l_ParameterList, p_MetaLevel);
		SNI_Thread::GetThread()->PopContextWorld();
		if (result.IsNull())
		{
			return this;
		}
		return result;
	}

	SN::SN_Error SNI_Function::DoAssert()
	{
		return AssertValue(skynet::True);
	}

	SN::SN_Error SNI_Function::AssertValue(const SN::SN_Expression &p_Value)
	{
		LOG(WriteExpression(SN::DebugLevel, p_Value, this));
		if (p_Value.IsError())
		{
			return p_Value.GetError();
		}

		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Function::AssertValue ( " + p_Value.DisplaySN() + " )"));

		SN::SN_ExpressionList * l_ParameterList = new SN::SN_ExpressionList();
		l_ParameterList->push_back(p_Value);
		l_ParameterList->push_back(m_Parameter);
		// Flatten the call stack, by returning the function to be called from Unify, instead of calling it there.
		SNI_Expression *function = m_Function;
		SNI_Error *e = dynamic_cast<SNI_Error *>(function);
		while (!e)
		{
			if (!l_ParameterList)
			{
				l_ParameterList = new SN::SN_ExpressionList();
				l_ParameterList->push_back(p_Value);
			}
			SNI_FunctionDef *functionDef = dynamic_cast<SNI_FunctionDef *>(function);
			if (functionDef)
			{
				if (functionDef->GetNumParameters() == l_ParameterList->size())
				{
					SN::SN_Expression *param_List = functionDef->LoadParametersUnify(l_ParameterList);
					if (l_ParameterList->empty())
					{
						delete l_ParameterList;
						l_ParameterList = NULL;
					}
					else
					{
						long dog = 10;
					}
					function = functionDef->UnifyArray(param_List, this).GetSNI_Expression();
					delete[] param_List;
				}
				else
				{
					function = functionDef->Call(l_ParameterList).GetSNI_Expression();
				}
			}
			else
			{
				if (function->IsValue() && l_ParameterList->size() == 1)
				{
					SN::SN_Expression *paramList = new SN::SN_Expression[2];
					paramList[0] = p_Value;
					paramList[1] = function;
					function = skynet::Same.GetSNI_FunctionDef()->UnifyArray(paramList, this).GetSNI_Expression();;
					delete[] paramList;
				}
				else
				{
					function = function->Unify(l_ParameterList).GetSNI_Expression();
				}
			}
			e = dynamic_cast<SNI_Error *>(function);
		}
		if (l_ParameterList)
		{
			delete l_ParameterList;;
		}
		SN::SN_Error err(e);
		ASSERTM(e == err.GetSNI_Error(), "Must be equal.");
		if (err.IsSignificantError())
		{
			SNI_CallRecord *callRecord = new SNI_CallRecord("Function call.", this);
			LOGGING(callRecord->SetLogContext(context));
			err.GetSNI_Error()->AddNote(callRecord);
		}
		return LOG_RETURN(context, err);
	}

	SN::SN_Error SNI_Function::AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World ** p_WorldList, SNI_WorldSet * p_WorldSet)
	{
		ASSERTM(p_WorldList == 0, "Called only with a cardinality of 1, so no worlds.");
		if (p_Value.IsKnownValue() || p_Value.IsReferableValue())
		{
			return AssertValue(p_Value);
		}
		return skynet::OK;
	}

	SN::SN_Error SNI_Function::DoPartialAssert()
	{
		return PartialAssertValue(skynet::True);
	}

	SN::SN_Error SNI_Function::PartialAssertValue(const SN::SN_Expression &p_Expression, bool /* p_Define = false */)
	{
		LOGGING(SN::LogContext context(DisplaySN0() + "SNI_Function::PartialAssertValue ( " + p_Expression.DisplaySN() + " )"));

		SN::SN_ParameterList * l_ParameterList = new SN::SN_ParameterList();
		l_ParameterList->push_back(SN::SN_Parameter(m_Parameter, m_Condition));

		return LOG_RETURN(context, m_Function->PartialUnify(l_ParameterList, p_Expression));
	}

	SN::SN_Expression SNI_Function::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Function::Call ( " + DisplaySnExpressionList(p_ParameterList) + " )"));

		p_ParameterList->push_back(m_Parameter);
	
		return LOG_RETURN(context, m_Function);
	}

	SN::SN_Expression SNI_Function::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Function::PartialCall ( " + DisplaySnExpressionList(p_ParameterList) + " )"));

		p_ParameterList->push_back(m_Parameter);

		return LOG_RETURN(context, m_Function->PartialCall(p_ParameterList, p_MetaLevel));
	}

	SN::SN_Expression SNI_Function::Unify(SN::SN_ExpressionList * p_ParameterList)
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Function::Unify ( " + DisplaySnExpressionList(p_ParameterList) + " )"));

		p_ParameterList->push_back(m_Parameter);

		return LOG_RETURN(context, m_Function);
	}

	SN::SN_Error SNI_Function::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Function::PartialUnify ( " + DisplaySnParameterList(p_ParameterList) + " = " + p_Result.DisplaySN() + " )"));

		p_ParameterList->push_back(SN::SN_Parameter(m_Parameter, m_Condition));

		return LOG_RETURN(context, m_Function->PartialUnify(p_ParameterList, p_Result));
	}

}
