#include "sni_expression.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sn.h"
#include "sn_cartesian.h"
#include "sn_error.h"
#include "sn_manager.h"

#include "sni_string.h"
#include "sni_stringref.h"
#include "sni_char.h"
#include "sni_taggedvalue.h"
#include "sni_world.h"
#include "sni_worldset.h"
#include "sni_valueset.h"
#include "sni_error.h"
#include "sni_null.h"
#include "sni_manager.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	map<string, unsigned long> SNI_Expression::m_IdMap;

	SNI_Expression::SNI_Expression()
		: m_Id(0)
	{
	}

	SNI_Expression::SNI_Expression(unsigned long p_Id)
		: m_Id(p_Id)
	{
	}

	SNI_Expression::SNI_Expression(const SNI_Expression &p_Expression)
		: SNI_Base(p_Expression)
	{
	}

	SNI_Expression::~SNI_Expression()
	{

	}

	//-----------------------------------------------------------------------
	// Logging
	//-----------------------------------------------------------------------

	string SNI_Expression::GetTypeName() const
	{
		return "Expression";
	}

	string SNI_Expression::GetValueTypeName() const
	{
		return GetTypeName();
	}

	string SNI_Expression::GetReferredName() const
	{
		return GetTypeName();
	}

	string SNI_Expression::DisplayCpp() const
	{
		return GetTypeName();
	}

	string SNI_Expression::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return GetTypeName();
	}

	void SNI_Expression::WriteJSON(ostream& p_Stream, const string& p_Prefix, size_t p_DebugFieldWidth, SNI::SNI_DisplayOptions& p_DisplayOptions) const
	{
		p_Stream << p_Prefix << "\"typetext\" : \"" << GetTypeName() << "\"";
		p_Stream << ",\n" << p_Prefix << "\"value\" : [\n";
		string prefix = p_Prefix;
		if (IsStringValue())
		{
			// Problem calling ForEach from HTTP server.
			// It creates SNI objects. It is supposed to be watching only, not changing things.
			SNI::SNI_DisplayOptions plainText(doTextOnly);
			string valueText;
			string valueTextHTML;
			valueText = DisplayValueSN(0, plainText);
			valueTextHTML = DisplayValueSN(0, p_DisplayOptions);
			p_Stream << prefix << "\t" << DetailsFS(valueText, valueTextHTML, p_DebugFieldWidth);
		}
		else
		{
			string delimeter;
			SNI_Expression* value = const_cast<SNI_Expression*>(this);
			value->ForEach(
				[&p_Stream, &delimeter, p_DebugFieldWidth, &p_DisplayOptions, &prefix](const SN::SN_Expression& p_Expression, SNI_World* p_World)->SN::SN_Error
				{
					SNI::SNI_DisplayOptions plainText(doTextOnly);
					string valueText;
					string valueTextHTML;
					if (p_Expression.GetSNI_Expression())
					{
						valueText = p_Expression.DisplaySN(plainText) + string(p_World ? "::" + p_World->DisplaySN(plainText) : "");
						valueTextHTML = p_Expression.DisplaySN(p_DisplayOptions) + string(p_World ? "::" + p_World->DisplaySN(p_DisplayOptions) : "");
					}
					p_Stream << delimeter << prefix << "\t" << DetailsFS(valueText, valueTextHTML, p_DebugFieldWidth);
					delimeter = ",\n";
					return skynet::OK;
				});
		}
		p_Stream << "\n" << p_Prefix << "]\n";
	}

	string SNI_Expression::DisplayValueSN(long priority, SNI_DisplayOptions & p_DisplayOptions) const
	{
		return DisplaySN(priority, p_DisplayOptions);
	}

	unsigned long SNI_Expression::GetId() const
	{
		const_cast<SNI_Expression *>(this)->CreateId();
		return m_Id;
	}
	
	string SNI_Expression::GetDebugId() const
	{
		return GetReferredName() + "_" + to_string(GetId());
	}

	void SNI_Expression::CreateId()
	{
		if (!m_Id)
		{
			string typeNameString = GetReferredName();
			if (m_IdMap.find(typeNameString) == m_IdMap.end())
			{
				m_Id = 1;
				m_IdMap[typeNameString] = m_Id;
			}
			else
			{
				m_Id = m_IdMap[typeNameString]++;
			}
		}
	}

	string SNI_Expression::GetBreakPoint(long p_Index) const
	{
		return MakeBreakPoint(GetDebugId(), p_Index);
	}

	string SNI_Expression::GetBreakPointJS(long p_Index) const
	{
		return MakeBreakPointJS(GetDebugId(), p_Index);
	}

	string SNI_Expression::SetBreakPoint(const string &p_Caption, SNI_DisplayOptions & p_DisplayOptions, const SNI_Expression *p_DebugSource, long p_Index) const
	{
		switch (p_DisplayOptions.GetDebugHTML())
		{
		case doTextOnly:
			if (p_Caption == "~" || p_Caption == ";" || p_Caption == "end")
			{
				return "";
			}
			return p_Caption;
		case doDebugPointsHTML:
			return p_Caption;
		case doDebugPointsJS:
			{
				string breakPoint = p_DebugSource->GetBreakPointJS(p_Index);
				return "<button title='" + breakPoint + "' ng-click='setbreakpoint(" + breakPoint + ")' ng-class='breakpointclass(" + breakPoint + ")'>" + p_Caption + "</button>";
			}
		}
		return "";
	}

	void SNI_Expression::Breakpoint(SN::DebuggingStop p_DebuggingStop, SN::BreakId p_BreakId, const string &p_TypeName, const string & p_Description, const SNI_Expression * p_Source, SN::InterruptPoint p_InterruptPoint) const
	{
		SNI_Thread::GetThread()->Breakpoint(p_DebuggingStop, p_BreakId, p_TypeName, p_Description, p_Source, p_InterruptPoint);
	}

	long SNI_Expression::GetPriority() const
	{
		return 1000;
	}

	string SNI_Expression::GetOperator() const
	{
		return "";
	}

	SNI_Expression *SNI_Expression::LoadParameters(SN::SN_ExpressionList * p_ParameterList) const
	{
		return NULL;
	}

	SNI_Expression *SNI_Expression::LoadFormalParameters(SN::SN_ExpressionList &p_FormalParameterList)
	{
		return this;
	}

	SN::SN_Expression * SNI_Expression::LoadParametersCall(SN::SN_ExpressionList * p_ParameterList) const
	{
		size_t numParams = p_ParameterList->size();
		SN::SN_Expression *paramList = new SN::SN_Expression[numParams];
		for (size_t j = 0; j < numParams; j++)
		{
			paramList[j] = (*p_ParameterList)[numParams - j - 1];
		}
		return paramList;
	}

	SN::SN_Expression * SNI_Expression::LoadParametersUnify(SN::SN_ExpressionList * p_ParameterList) const
	{
		size_t numParams = p_ParameterList->size();
		SN::SN_Expression *paramList = new SN::SN_Expression[numParams];
		paramList[PU2_Result] = (*p_ParameterList)[0];
		for (size_t j = 1; j < numParams; j++)
		{
			paramList[j] = (*p_ParameterList)[numParams - j];
		}
		return paramList;
	}
	
	string SNI_Expression::DisplayCall(long priority, SNI_DisplayOptions &p_DisplayOptions, size_t p_NumParams, SN::SN_Expression *p_ParamList, const SNI_Expression *p_DebugSource) const
	{
		string text;
		for (size_t j = 0; j < p_NumParams; j++)
		{
			text += " " + p_ParamList[j].GetSNI_Expression()->DisplaySN(GetPriority(), p_DisplayOptions);
		}
		return Bracket(priority, DisplaySN(GetPriority(), p_DisplayOptions), p_DisplayOptions, p_DebugSource) + text;
	}

	string SNI_Expression::DisplayUnify(size_t p_NumParams, SN::SN_Expression *p_ParamList, const SNI_Expression *p_DebugSource) const
	{
		SNI_DisplayOptions displayOptions(doTextOnly);
		return "dummy"; // p_ParamList[0].DisplaySN() + " = " + DisplayCall(0, displayOptions, p_NumParams - 1, p_ParamList + 1, NULL);
	}

	string SNI_Expression::DisplayUnifyExp(SN::SN_ExpressionList * p_ParameterList) const
	{
		size_t numParams = p_ParameterList->size();
		SNI_DisplayOptions displayOptions(doTextOnly);
		string result = (*p_ParameterList)[0].DisplaySN() + " = " + DisplayValueSN(0, displayOptions);
		for (size_t j = 1; j < numParams; j++)
		{
			result += " " + (*p_ParameterList)[numParams - j].DisplaySN();
		}
		return result;
	}

	SNI_Expression * SNI_Expression::Clone(long p_MetaLevel, SNI_Frame *p_Frame, bool &/*p_Changed*/)
	{
		return this;
	}

	SNI_Expression * SNI_Expression::Clone(const SNI_Expression *p_Function, SNI_Expression *p_Result)
	{
		bool changed = false;
		SNI_Frame *frame = SNI_Frame::Push(p_Function, p_Result);
		SNI_Expression *function = Clone(0, frame, changed);
		frame->SetCloneFunction(function);
		return function;
	}

	SNI_Expression * SNI_Expression::Copy() const
	{
		return const_cast<SNI_Expression *>(this);
	}

	/* static */ SN::SN_Expression SNI_Expression::AddLambdasPartial(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result)
	{
		if (p_ParameterList->size())
		{
			SN::SN_Expression param = p_ParameterList->back().GetValue();
			p_ParameterList->pop_back();
			return SN::SN_Lambda(param, AddLambdasPartial(p_ParameterList, p_Result));
		}
		return p_Result;
	}

	/* static */ SN::SN_Expression SNI_Expression::AddLambdas(SN::SN_ExpressionList * p_ParameterList)
	{
		if (1 < p_ParameterList->size())
		{
			SN::SN_Expression param = p_ParameterList->back();
			p_ParameterList->pop_back();
			return SN::SN_Lambda(param, AddLambdas(p_ParameterList));
		}
		return (*p_ParameterList)[0];
	}

	string SNI_Expression::Bracket(long p_Priority, const string &p_Expression, SNI_DisplayOptions & p_DisplayOptions, const SNI_Expression *p_DebugSource) const
	{
		string bracketLeft;
		string bracketRight;
		if (p_DisplayOptions.GetDebugHTML())
		{
			bracketLeft = SetBreakPoint("(", p_DisplayOptions, p_DebugSource, SN::LeftId);
			bracketRight = SetBreakPoint(")", p_DisplayOptions, p_DebugSource, SN::RightId);
		}
		else if (p_Priority > GetPriority())
		{
			bracketLeft = "(";
			bracketRight = ")";
		}
		return bracketLeft + p_Expression + bracketRight;
	}

	void SNI_Expression::AddVariables(long p_MetaLevel, SNI_VariablePointerMap& p_Map)
	{
	}

	string SNI_Expression::DisplaySN0() const
	{
		SNI_DisplayOptions l_DisplayOptions(doTextOnly);
		return DisplaySN(0, l_DisplayOptions);
	}

	//-----------------------------------------------------------------------
	// Base
	//-----------------------------------------------------------------------
	bool SNI_Expression::GetBool() const
	{
		return false;
	}

	string SNI_Expression::GetString() const
	{
		return "";
	}

	size_t SNI_Expression::Count() const
	{
		return 0;
	}

	size_t SNI_Expression::Length() const
	{
		return 0;
	}

	void SNI_Expression::Simplify()
	{
	}

	void SNI_Expression::Validate()
	{
	}

	SN::SN_Expression SNI_Expression::SimplifyValue()
	{
		return this;
	}

	void SNI_Expression::AttachDelayedCall(SNI_DelayedCall *p_Call)
	{
	}

	bool SNI_Expression::IsRequested() const
	{
		return false;
	}

	void SNI_Expression::Request()
	{
	}

	SNI_WorldSet * SNI_Expression::GetWorldSet()
	{
		return NULL;
	}

	bool SNI_Expression::IsNull() const
	{
		return false;
	}

	bool SNI_Expression::IsValue() const
	{
		return false;
	}

	bool SNI_Expression::IsKnownValue() const
	{
		return false;
	}

	bool SNI_Expression::IsLeftKnownValue() const
	{
		return IsKnownValue();
	}

	bool SNI_Expression::IsRightKnownValue() const
	{
		return IsKnownValue();
	}

	bool SNI_Expression::IsKnownTypeValue() const
	{
		return IsKnownValue();
	}

	bool SNI_Expression::IsLambdaValue() const
	{
		return false;
	}

	bool SNI_Expression::IsInline() const
	{
		return true;
	}

	bool SNI_Expression::IsComplete() const
	{
		return true;
	}
	
	void SNI_Expression::Complete()
	{
	}

	bool SNI_Expression::IsVariable() const
	{
		return false;
	}

	bool SNI_Expression::IsValueHolder() const
	{
		return false;
	}

	bool SNI_Expression::IsValueSet() const
	{
		return false;
	}

	bool SNI_Expression::IsString() const
	{
		return false;
	}

	bool SNI_Expression::IsStringValue() const
	{
		return IsString();
	}

	bool SNI_Expression::IsError() const
	{
		return false;
	}

	bool SNI_Expression::IsSignificantError() const
	{
		return false;
	}

	bool SNI_Expression::IsErrorType() const
	{
		return false;
	}

	bool SNI_Expression::IsFixed() const
	{
		return false;
	}

	bool SNI_Expression::AllValues() const
	{
		return false;
	}

	bool SNI_Expression::AllValuesEqual(const SN::SN_Expression & p_Value) const
	{
		return false;
	}

	bool SNI_Expression::AllValuesBoolean() const
	{
		return false;
	}

	SN::SN_Error SNI_Expression::AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World ** p_World, SNI_WorldSet * p_WorldSet)
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Expression::AddValue ( )"));

		return LOG_RETURN(context, SN::SN_Error(false, false, GetTypeName() + " AddValue not implemented ."));
	}

	bool SNI_Expression::MarkComplete()
	{
		return true;
	}

	bool SNI_Expression::IsNullValue() const
	{
		return false;
	}

	bool SNI_Expression::IsReferableValue() const
	{
		return false;
	}

	SN::SN_Expression SNI_Expression::GetVariableValue(bool p_IfComplete)
	{
		return this;
	}

	SNI_Expression* SNI_Expression::GetValue(bool p_Request) const
	{
		return const_cast<SNI_Expression*>(this);
	}
	
	const SNI_Expression* SNI_Expression::GetSafeValue() const
	{
		return this;
	}

	size_t SNI_Expression::Cardinality(size_t p_MaxCardinality) const
	{
		return p_MaxCardinality;
	}

	size_t SNI_Expression::LeftCardinality(size_t p_MaxCardinality) const
	{
		return Cardinality(p_MaxCardinality);
	}

	size_t SNI_Expression::RightCardinality(size_t p_MaxCardinality) const
	{
		return Cardinality(p_MaxCardinality);
	}

	SN::SN_Error SNI_Expression::ForEach(std::function<SN::SN_Error(const SN::SN_Expression &p_Param, SNI_World *p_World)> p_Action)
	{
		return p_Action(SN::SN_Expression(this), NULL);
	}

	SN::SN_Error SNI_Expression::ForEachCart(long p_Depth, SNI_Cart * p_Cart)
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Expression::ForEachCart ( cartesian )"));

		return LOG_RETURN(context, SN::SN_Error(false, false, GetTypeName() + " ForEachCart not implemented ."));
	}

	void SNI_Expression::ForEachSplit(SNI_Splitter * p_Splitter)
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Expression::ForEachSplit ( splitter )"));

		FORCE_ASSERTM(GetTypeName() + " ForEachSplit not implemented .");
	}

	SN::SN_Cartesian SNI_Expression::CartProd(long p_Index, SNI_FunctionDef *p_FunctionDef)
	{
		return SN::SN_Cartesian(SN::SN_Expression(this), p_Index, p_FunctionDef);
	}

	bool SNI_Expression::Equivalent(SNI_Object * /*p_Other*/) const
	{
		return false;
	}

	size_t SNI_Expression::Hash() const
	{
		ASSERTM(false, GetTypeName() + " hash not implemented.");
		return 0;
	}

	SN::SN_Expression SNI_Expression::DoEvaluate(long /* p_MetaLevel = 0 */) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_Expression::DoPartialEvaluate(long /* p_MetaLevel = 0 */) const
	{
		return this;
	}

	SN::SN_Error SNI_Expression::DoAssert()
	{
		return skynet::Fail;
	}

	SN::SN_Error SNI_Expression::AssertValue(const SN::SN_Expression &p_Value)
	{
		SN::SN_Value thisValue(this);
		if (SN::Is<SNI_ValueSet *>(p_Value))
		{
			SN::SN_ValueSet valueSet(p_Value, 0);

			LOGGING(SN::LogContext context(DisplaySN0() + "SNI_Expression::AssertValue ( " + valueSet.DisplaySN() + " )"));

			bool result = false;
			for (size_t i = 0; i < valueSet.Length(); i++)
			{
				if (valueSet[i].GetValue().IsNull())
				{
					result = true;
				}
				else if (valueSet[i].GetValue().IsNullValue())
				{
					SN::SN_Error e1 = valueSet[i].GetValue().AssertValue(thisValue);
					if (e1.IsError())
					{
						return e1;
					}
					result = true;
				}
				else if (valueSet[i].GetValue().Equivalent(thisValue))
				{
					result = true;
				}
				else
				{
					SNI_World * world = valueSet[i].GetWorld();
					if (world)
					{
						SN::SN_Error err = world->Fail(IncompatibleValue);
						if (err.IsError())
						{
							return err;
						}
					}
				}
			}
			valueSet.Simplify();
			return SN::SN_Error(result, false);
		}
		return SN::SN_Error(p_Value.Equivalent(thisValue), false);
	}

	SN::SN_Error SNI_Expression::AssertIsA(const SN::SN_Expression &p_Value)
	{
		return skynet::Fail;
	}

	SN::SN_Error SNI_Expression::SelfAssert()
	{
		return skynet::OK;
	}

	SN::SN_Error SNI_Expression::DoPartialAssert()
	{
		return skynet::Fail;
	}

	SN::SN_Expression SNI_Expression::DoBuildMeta(long p_MetaLevel)
	{
		SN::SN_Expression expression(this);
		return SN::SN_Meta(p_MetaLevel, expression);
	}

	SN::SN_Error SNI_Expression::PartialAssertValue(const SN::SN_Expression &p_Expression, bool /* p_Define */)
	{
		return SN::SN_Error(Equivalent(p_Expression.GetSNI_Expression()), false);
	}

	SN::SN_Expression SNI_Expression::Call(SN::SN_ExpressionList * p_ParameterList, long  /* p_MetaLevel = 0 */) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Expression::Call ( " + DisplaySnExpressionList(p_ParameterList) + " )"));

		return LOG_RETURN(context, SN::SN_Error(false, false, GetTypeName() + " Call not implemented."));
	}

	SN::SN_Expression SNI_Expression::PartialCall(SN::SN_ExpressionList * p_ParameterList, long  /* p_MetaLevel = 0 */) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Concat::PartialCall ( " + DisplaySnExpressionList(p_ParameterList) + " )"));

		return LOG_RETURN(context, SN::SN_Error(false, false, GetTypeName() + " Partial Call not implemented."));
	}

	SN::SN_Expression SNI_Expression::Unify(SN::SN_ExpressionList * /*p_ParameterList*/)
	{
		return SN::SN_Error(false, false, GetTypeName() + " Unify not implemented.");
	}

	SN::SN_Error SNI_Expression::PartialUnify(SN::SN_ParameterList * /*p_ParameterList*/, SN::SN_Expression /*p_Expression*/, bool /*p_Define*/)
	{
		return skynet::Fail;
	}

	void SNI_Expression::Throw()
	{
		DoWithHandler(SNI_Manager::ThrowErrorHandler);
	}

	void SNI_Expression::Do()
	{
		DoWithHandler(SNI_Thread::TopManager()->ErrorHandler());
	}

	void SNI_Expression::DoWithHandler(OnErrorHandler * p_ErrorHandler)
	{
		LOG(WriteExp(this));
		SNI_Thread *thread = SNI_Thread::GetThread();
		bool finished = false;
		SNI_Frame* frame = thread->Top();
		vector<bool> savePoint;
		if (frame)
		{
			frame->RecordSavePoint(savePoint);
		}
		thread->SaveStepCount();
		do
		{
			try
			{
				SN::SN_Variable resultVariable;
				SN::SN_Error e = AssertValue(resultVariable);
				HandleAction(e, p_ErrorHandler);
				HandleAction(resultVariable.GetVariableValue(), p_ErrorHandler);
				finished = true;
			}
			catch (SN::SN_Error &e)
			{
				if (e.RequestRerun())
				{
					thread->ResetStepCount();
					if (frame)
					{
						frame->RestoreSavePoint(savePoint);
					}
				}
				else
				{
					throw e;
				}
			}
		} while (!finished);
	}

	void SNI_Expression::HandleAction(SN::SN_Expression p_Result, OnErrorHandler *p_ErrorHandler)
	{
		SN::SN_Error e = p_Result.GetError();
		if (!e.IsError())
		{
			SNI_DelayedProcessor* processor = SNI_Thread::GetThread()->GetProcessor();
			if (processor)
			{
				e = processor->DoAssert();
			}
		}
		if (e.IsError())
		{
			e.Log();
			p_ErrorHandler(e.IsError(), e.GetDescription());
		}
	}

	SN::SN_Error SNI_Expression::GetError()
	{
		SNI_Error* err = dynamic_cast<SNI_Error*>(this);
		if (err)
		{
			return err;
		}
		SNI_Bool* result = dynamic_cast<SNI_Bool*>(this);
		if (result)
		{
			return SN::SN_Error(result->GetBool(), false);
		}
		return SN::SN_Error(false, false, "Bool or error expected.");
	}

	SN::SN_Expression SNI_Expression::Meta(long p_MetaLevel)
	{
		return SN::SN_Meta(p_MetaLevel, GetVariableValue(true));
	}

	void SNI_Expression::Fix(SN::SN_Expression p_Value)
	{
	}

	void SNI_Expression::Fix()
	{
	}

	//-----------------------------------------------------------------------
	// Implementation
	//-----------------------------------------------------------------------

	// Numbers
	SN::SN_Value SNI_Expression::DoAdd(SNI_Value * /*p_Other*/) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " + operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoSubtract(SNI_Value * /*p_Other*/) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " - operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoDivide(SNI_Value * /*p_Other*/) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " / operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoMultiply(SNI_Value * /*p_Other*/) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " * operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoNegative() const
	{
		return SN::SN_Error(false, false, GetTypeName() + " Unary - operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoSquare() const
	{
		return SN::SN_Error(false, false, GetTypeName() + "  Square function not implemented.");
	}

	SN::SN_Value SNI_Expression::DoSquareRoot() const
	{
		return SN::SN_Error(false, false, GetTypeName() + "  SquareRoot function not implemented.");
	}

	//Logic
	SN::SN_Value SNI_Expression::DoNot() const
	{
		return SN::SN_Error(false, false, GetTypeName() + " not function not implemented.");
	}

	SN::SN_Value SNI_Expression::DoRevAnd(SNI_Expression * p_PositiveCase) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " 'reverse and' function not implemented.");
	}

	SN::SN_Value SNI_Expression::DoRevOr(SNI_Expression * p_PositiveCase) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " 'reverse or' function not implemented.");
	}

	SN::SN_Value SNI_Expression::DoCollapse()
	{
		return SN::SN_Error(false, false, GetTypeName() + " collapse function not implemented.");
	}

	SN::SN_Value SNI_Expression::DoUnaryAnd() const
	{
		return SN::SN_Error(false, false, GetTypeName() + " 'Unary and' function not implemented.");
	}

	SN::SN_Value SNI_Expression::DoUnaryOr() const
	{
		return SN::SN_Error(false, false, GetTypeName() + " 'Unary or' function not implemented.");
	}

	SN::SN_Value SNI_Expression::DoAnd(SNI_Expression * /*p_Other*/) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " && operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoOr(SNI_Expression * /*p_Other*/) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " || operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoIf(SNI_Expression * /*p_PositiveCase*/, SNI_Expression * /*p_NegativeCase*/) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " If operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoImplies(SNI_Expression * /*p_PositiveCase*/) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " Implies operator not implemented.");
	}

	//Comparison
	SN::SN_Value SNI_Expression::DoEquals(SNI_Value *p_Other) const
	{
		if (SN::Is<SNI_StringRef *>(p_Other))
		{
			return p_Other->DoEquals(dynamic_cast<SNI_Value *>(const_cast<SNI_Expression *>(this)));
		}
		if (SN::Is<SNI_ValueSet *>(p_Other))
		{
			return p_Other->DoEquals(dynamic_cast<SNI_Value *>(const_cast<SNI_Expression *>(this)));
		}
		return SN::SN_Bool(Equivalent(dynamic_cast<SNI_Object *>(p_Other)));
	}

	SN::SN_Error SNI_Expression::DoAssertEqualsValue(SNI_Value *p_Other, SNI_Value *p_Result)
	{
		if (SN::Is<SNI_StringRef *>(p_Other))
		{
			return p_Other->DoEquals(dynamic_cast<SNI_Value *>(const_cast<SNI_Expression *>(this))).GetError();
		}
		if (SN::Is<SNI_ValueSet *>(p_Other))
		{
			return p_Other->DoEquals(dynamic_cast<SNI_Value *>(const_cast<SNI_Expression *>(this))).GetError();
		}
		return SN::SN_Error(p_Result->GetBool() == Equivalent(dynamic_cast<SNI_Object *>(p_Other)), false);
	}

	SN::SN_Value SNI_Expression::DoLessThan(SNI_Value * /*p_Other*/) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " < operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoGreaterThan(SNI_Value * /*p_Other*/) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " > operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoLessEquals(SNI_Value * /*p_Other*/) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " <= operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoGreaterEquals(SNI_Value * /*p_Other*/) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " >= operator not implemented.");
	}

	//Strings
	SN::SN_Value SNI_Expression::DoConcat(SNI_Value * /*p_Other*/) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " + operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoSubtractLeft(SNI_Value * /*p_Other*/) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " SubtractLeft method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoSubtractRight(SNI_Value * /*p_Other*/) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " SubtractRight method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoSubtractLeftChar() const
	{
		return SN::SN_Error(false, false, GetTypeName() + " SubtractLeftChar method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoSubtractRightChar() const
	{
		return SN::SN_Error(false, false, GetTypeName() + " SubtractRightChar method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoSelectRightChar() const
	{
		return SN::SN_Error(false, false, GetTypeName() + " SelectRightChar method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoSelectLeftChar() const
	{
		return SN::SN_Error(false, false, GetTypeName() + " SelectLeftChar method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoLookaheadLeft() const
	{
		return SN::SN_Error(false, false, GetTypeName() + " LookaheadLeft method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoLookaheadRight() const
	{
		return SN::SN_Error(false, false, GetTypeName() + " LookaheadRight method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoLookStringLeft(SNI_Value * p_Other) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " LookStringLeft method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoLookStringRight(SNI_Value * p_Other) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " LookStringRight method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoFile() const
	{
		return SN::SN_Error(false, false, GetTypeName() + " File method not implemented.");
	}

	// Conversion
	SN::SN_Value SNI_Expression::DoEscape(enum skynet::EscapeType p_EscapeType) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " Escape method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoUnescape(enum skynet::EscapeType p_EscapeType) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " Unescape method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoIntToString() const
	{
		return SN::SN_Error(false, false, GetTypeName() + " IntToString method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoStringToInt() const
	{
		return SN::SN_Error(false, false, GetTypeName() + " StringToInt method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoDoubleToString() const
	{
		return SN::SN_Error(false, false, GetTypeName() + " DoubleToString method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoStringToDouble() const
	{
		return SN::SN_Error(false, false, GetTypeName() + " StringToDouble method not implemented.");
	}

	SN::SN_Error  SNI_Expression::AssertIsAValue(const SNI_Value * p_Parent, SN::SN_Expression p_Result)
	{
		return SN::SN_Error(false, false, GetTypeName() + " IsA function not implemented.");
	}

	// Inheritance
	SNI_Class *SNI_Expression::VClass()
	{
		return NULL;
	}

	SN::SN_Value SNI_Expression::DoIsA(const SNI_Value * p_Parent) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " IsA function not implemented.");
	}

	SN::SN_Value SNI_Expression::DoHasA(SNI_Value * p_Member, SNI_Value * p_Name) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " HasA function not implemented.");
	}

	// Sets
	SN::SN_Value SNI_Expression::DoBuildSet() const
	{
		return SN::SN_Error(false, false, GetTypeName() + " BuildSet function not implemented.");
	}

	SN::SN_Value SNI_Expression::DoHasMember(SNI_Value * /*p_Member*/) const
	{
		return SN::SN_Error(false, false, GetTypeName() + "  HasMember function not implemented.");
	}

	// ValueSets
	SN::SN_ValueSet SNI_Expression::DoRemove(const SN::SN_Value & /*p_Other*/)
	{
		return SN::SN_Error(false, false, GetTypeName() + " Remove not implemented.");
	}

	SN::SN_Error SNI_Expression::AssertSubscriptValue(const SNI_Value * p_Index, SN::SN_Expression p_Result)
	{
		return SN::SN_Error(false, false, GetTypeName() + " Subscript operator [] function not implemented on assert.");
	}

	SN::SN_Value SNI_Expression::DoSubscriptCall(const SN::SN_Value & p_Index) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " Subscript operator [] not implemented.");
	}

	SN::SN_Value SNI_Expression::DoReverseSubscript(const SN::SN_Value & p_Result) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " Reverse subscript operator [] not implemented.");
	}

	SN::SN_Value SNI_Expression::DoCountIf(SN::SN_Expression p_Value) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " CountIf function not implemented.");
	}

	SN::SN_Value SNI_Expression::DoCountAll() const
	{
		return SN::SN_Error(false, false, GetTypeName() + " CountAll function not implemented.");
	}

	SN::SN_Value SNI_Expression::DoSum() const
	{
		return SN::SN_Error(false, false, GetTypeName() + " Sum function not implemented.");
	}

	bool SNI_Expression::DoIsEmpty() const
	{
		ASSERTM(false, GetTypeName() + " IsEmpty not implemented.");
		return false;
	}
}
