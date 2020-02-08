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
	SNI_Function::SNI_Function()
	{
	}

	SNI_Function::SNI_Function(SNI_Expression *p_Function)
		: m_Function(p_Function)
		, m_Parameter(NULL)
		, m_Condition(NULL)
	{
		REQUESTPROMOTION(m_Function);
		REQUESTPROMOTION(m_Parameter);
	}


	SNI_Function::SNI_Function(SNI_Expression *p_Function, SNI_Expression *p_Parameter)
		: m_Function(p_Function)
		, m_Parameter(p_Parameter)
		, m_Condition(NULL)
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
		SN::SN_ExpressionList * l_ParameterList = new SN::SN_ExpressionList();
		SNI_Expression *nextFunction = LoadParameters(l_ParameterList);
		const SNI_Expression *function = NULL;
		while (nextFunction)
		{
			function = nextFunction;
			nextFunction = function->LoadParameters(l_ParameterList);
		}
		return function->DisplayCall(GetPriority(), p_DisplayOptions, l_ParameterList, this);
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

	SNI_Expression * SNI_Function::Clone(SNI_Frame *p_Frame, bool &p_Changed)
	{
		bool changed = false;
		SNI_Expression * l_Function = m_Function->Clone(p_Frame, changed);
		SNI_Expression * l_Parameter = m_Parameter->Clone(p_Frame, changed);
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
		SNI_Thread::GetThread()->SetDebugId(GetDebugId());
		SN::SN_ExpressionList * l_ParameterList = new SN::SN_ExpressionList();
		l_ParameterList->push_back(m_Parameter);
		return m_Function->Call(l_ParameterList, p_MetaLevel);
	}

	SN::SN_Expression SNI_Function::DoPartialEvaluate(long p_MetaLevel /* = 0 */) const
	{
		SN::SN_ExpressionList * l_ParameterList = new SN::SN_ExpressionList();
		l_ParameterList->push_back(m_Parameter);
		SNI_World *world(new SNI_World(NULL)); // Only create a world so that errors are not fatal.
		SNI_World::PushContextWorld(world);
		SN::SN_Expression result = m_Function->PartialCall(l_ParameterList, p_MetaLevel);
		SNI_World::PopContextWorld();
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
			return p_Value;
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
			SNI_FunctionDef *functionDef = dynamic_cast<SNI_FunctionDef *>(function);
			if (functionDef)
			{
				SNI_Thread::GetThread()->SetDebugId(GetDebugId());
				SN::SN_Expression *param_List = functionDef->LoadParametersUnify(l_ParameterList);
				function = functionDef->UnifyArray(param_List).GetSNI_Expression();
			}
			else
			{
				SNI_Thread::GetThread()->SetDebugId(GetDebugId());
				function = function->Unify(l_ParameterList).GetSNI_Expression();
			}
			e = dynamic_cast<SNI_Error *>(function);
		}
		SN::SN_Error err(e);
		if (err.IsError())
		{
			SNI_CallRecord *callRecord = new SNI_CallRecord("Function call.", this);
			LOGGING(callRecord->SetLogContext(context));
			err.GetSNI_Error()->AddNote(callRecord);
			SNI_Thread::GetThread()->DebugCommand(SN::ErrorPoint, "Error", SN::ErrorId);
		}
		return LOG_RETURN(context, SN::SN_Expression(e));
	}

	SN::SN_Error SNI_Function::AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World ** p_WorldList, SNI_WorldSet * p_WorldSet)
	{
		ASSERTM(p_WorldList == 0, "Called only with a cardinality of 1, so no worlds.");
		return AssertValue(p_Value);
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
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Function::Call ( " + DisplayPmExpressionList(p_ParameterList) + " )"));

		p_ParameterList->push_back(m_Parameter);
	
		return LOG_RETURN(context, m_Function->Call(p_ParameterList, p_MetaLevel));
	}

	SN::SN_Expression SNI_Function::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Function::PartialCall ( " + DisplayPmExpressionList(p_ParameterList) + " )"));

		p_ParameterList->push_back(m_Parameter);

		return LOG_RETURN(context, m_Function->PartialCall(p_ParameterList, p_MetaLevel));
	}

	SN::SN_Expression SNI_Function::Unify(SN::SN_ExpressionList * p_ParameterList)
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Function::Unify ( " + DisplayPmExpressionList(p_ParameterList) + " )"));

		p_ParameterList->push_back(m_Parameter);

		return LOG_RETURN(context, m_Function);
	}

	SN::SN_Error SNI_Function::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Function::PartialUnify ( " + DisplayPmParameterList(p_ParameterList) + " = " + p_Result.DisplaySN() + " )"));

		p_ParameterList->push_back(SN::SN_Parameter(m_Parameter, m_Condition));

		return LOG_RETURN(context, m_Function->PartialUnify(p_ParameterList, p_Result));
	}

}
