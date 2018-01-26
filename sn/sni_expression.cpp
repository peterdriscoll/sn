#include "sni_expression.h"

#include "logcontext.h"
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
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Expression::SNI_Expression()
	{

	}

	SNI_Expression::SNI_Expression(const SNI_Expression &p_Expression)
		: SNI_Base(p_Expression)
	{

	}

	SNI_Expression::~SNI_Expression()
	{

	}

	void SNI_Expression::PromoteMembers()
	{
	}

	//-----------------------------------------------------------------------
	// Logging
	//-----------------------------------------------------------------------

	string SNI_Expression::GetTypeName() const
	{
		return "Expression";
	}

	string SNI_Expression::DisplayCpp() const
	{
		return GetTypeName();
	}

	string SNI_Expression::DisplaySN(long /*priority*/, SNI_VariableConstPointerList & /*p_DisplayVariableList*/) const
	{
		return GetTypeName();
	}

	string SNI_Expression::DisplayValueSN(long priority, SNI_VariableConstPointerList & p_DisplayVariableList) const
	{
		return DisplaySN(priority, p_DisplayVariableList);
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

	string SNI_Expression::DisplayCall(long priority, SNI_VariableConstPointerList &p_DisplayVariableList, SN::SN_ExpressionList * p_ParameterList) const
	{
		string text;
		string delimeter = "(";
		for (SN::SN_Expression &p: *p_ParameterList)
		{
			text += delimeter + p.GetSNI_Expression()->DisplayValueSN(GetPriority(), p_DisplayVariableList);
		}
		return DisplaySN(priority, p_DisplayVariableList) + "(" + text + ")";
	}

	SNI_Expression * SNI_Expression::Clone(SNI_Frame *p_Frame, bool &/*p_Changed*/)
	{
		return this;
	}

	SNI_Expression * SNI_Expression::Clone(const SNI_Variable *p_Variable)
	{
		bool changed = false;
		SNI_Expression * result = Clone(SNI_Frame::Push(p_Variable), changed);
		return result;
	}

	string SNI_Expression::Bracket(long p_Priority, const string &p_Expression) const
	{
		string bracketLeft;
		string bracketRight;
		if (p_Priority > GetPriority())
		{
			bracketLeft = "(";
			bracketRight = ")";
		}
		return bracketLeft + p_Expression + bracketRight;
	}

	string SNI_Expression::DisplaySN0() const
	{
		SNI_VariableConstPointerList l_DisplayList;
		return DisplaySN(0, l_DisplayList);
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

	bool SNI_Expression::IsStringValue() const
	{
		return dynamic_cast<const SNI_String *>(this) || dynamic_cast<const SNI_StringRef *>(this) || dynamic_cast<const SNI_Char *>(this);
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

	bool SNI_Expression::IsKnownValue() const
	{
		return false;
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

	bool SNI_Expression::IsError() const
	{
		return false;
	}

	bool SNI_Expression::AllValues() const
	{
		return false;
	}

	SN::SN_Error SNI_Expression::AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World ** p_World, SNI_WorldSet * p_WorldSet)
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Expression::AddValue ( )");

		return SN::SN_Error(GetTypeName() + " AddValue not implemented .");
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
		return skynet::Null;
	}

	size_t SNI_Expression::Cardinality(size_t p_MaxCardinality) const
	{
		return p_MaxCardinality;
	}

	SN::SN_Error SNI_Expression::ForEach(std::function<SN::SN_Error(const SN::SN_Expression &p_Param, SNI_World *p_World)> p_Action)
	{
		return p_Action(SN::SN_Expression(this), NULL);
	}

	SN::SN_Error SNI_Expression::ForEachCart(long p_Depth, SNI_Cart * p_Cart)
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Expression::ForEachCart ( cartesian )");

		return SN::SN_Error(GetTypeName() + " ForEachCart not implemented .");
	}

	void SNI_Expression::ForEachSplit(SNI_Splitter * p_Splitter)
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Expression::ForEachSplit ( splitter )");

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

	SN::SN_Expression SNI_Expression::Evaluate(long /* p_MetaLevel = 0 */) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_Expression::PartialEvaluate(long /* p_MetaLevel = 0 */) const
	{
		return this;
	}

	SN::SN_Error SNI_Expression::Assert()
	{
		return false;
	}

	SN::SN_Error SNI_Expression::AssertValue(const SN::SN_Expression &p_Value)
	{
		SN::SN_Value thisValue(this);
		if (SN::Is<SNI_ValueSet *>(p_Value))
		{
			SN::SN_ValueSet valueSet(p_Value, 0);
			SN::LogContext context(DisplaySN0() + "SNI_Expression::AssertValue ( " + valueSet.DisplaySN() + " )");
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
						if (!world->Fail())
						{
							return false;
						}
					}
				}
			}
			valueSet.Simplify();
			return result;
		}
//		if (p_Value.IsVariable())
//		{
//			SN::SN_Expression value = p_Value;
//			return value.AssertValue(this);
//		}
		return p_Value.Equivalent(thisValue);
	}

	SN::SN_Error SNI_Expression::SelfAssert()
	{
		return true;
	}

	SN::SN_Error SNI_Expression::PartialAssert()
	{
		return false;
	}

	SN::SN_Error SNI_Expression::PartialAssertValue(const SN::SN_Expression &/*p_Expression*/, bool /* p_Define */)
	{
		return false;
	}

	SN::SN_Expression SNI_Expression::Call(SN::SN_ExpressionList * p_ParameterList, long  /* p_MetaLevel = 0 */) const
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Expression::Call ( " + DisplayPmExpressionList(p_ParameterList) + " )");

		return SN::SN_Error(GetTypeName() + " Call not implemented.");
	}

	SN::SN_Expression SNI_Expression::PartialCall(SN::SN_ExpressionList * p_ParameterList, long  /* p_MetaLevel = 0 */) const
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Concat::PartialCall ( " + DisplayPmExpressionList(p_ParameterList) + " )");

		return SN::SN_Error(GetTypeName() + " Partial Call not implemented.");
	}

	SN::SN_Expression SNI_Expression::Unify(SN::SN_ExpressionList * /*p_ParameterList*/)
	{
		return SN::SN_Error(GetTypeName() + " Unify not implemented.");
	}

	SN::SN_Error SNI_Expression::PartialUnify(SN::SN_ParameterList * /*p_ParameterList*/, SN::SN_Expression /*p_Expression*/)
	{
		return false;
	}

	void SNI_Expression::AssertThrow()
	{
		SN::SN_Manager manager;
		AssertAction();
	}
	void SNI_Expression::PartialAssertThrow()
	{
		SN::SN_Manager manager;
		PartialAssertAction();
	}
	void SNI_Expression::EvaluateThrow()
	{
		SN::SN_Manager manager;
		EvaluateThrow();
	}
	void SNI_Expression::PartialEvaluateThrow()
	{
		SN::SN_Manager manager;
		PartialEvaluateThrow();
	}

	void SNI_Expression::AssertAction()
	{
		SN::LogContext context("SNI_Expression::AssertAction()");
		SN::SN_Expression clone = Clone(NULL);
		HandleAssertAction(context, clone.Assert(), "Assert");
		SNI_Frame::Pop();
	}

	void SNI_Expression::PartialAssertAction()
	{
		SN::LogContext context("SNI_Expression::PartialAssertAction()");
		HandleAssertAction(context, PartialAssert(), "Partial assert");
	}

	void SNI_Expression::HandleAssertAction(SN::LogContext & p_Context, SN::SN_Expression p_Result, string p_Text)
	{
		SNI_DelayedProcessor::GetProcessor()->Run();
		SN::SN_Error e = p_Result;
		if (e.IsError())
		{
			e.AddNote(p_Context, this, p_Text);
			e.Log();
			SN::SN_Manager::GetTopManager().ErrorHandler()(e);
		}
	}

	void SNI_Expression::EvaluateAction()
	{
		SN::LogContext context("SNI_Expression::EvaluateAction()");
		SN::SN_Expression clone = Clone(NULL);
		HandleEvaluateAction(context, clone.Evaluate(), "Evaluate");
		SNI_Frame::Pop();
	}

	void SNI_Expression::PartialEvaluateAction()
	{
		SN::LogContext context("SNI_Expression::PartialEvaluateAction()");
		HandleEvaluateAction(context, PartialEvaluate(), "Partial evaluate");
	}

	void SNI_Expression::HandleEvaluateAction(SN::LogContext &p_Context, SN::SN_Expression p_Result, string p_Text)
	{
		SNI_DelayedProcessor::GetProcessor()->Run();
		SN::SN_Error e = p_Result;
		SN::SN_Bool b = p_Result;
		if (b.IsNull())
		{
			if (e.IsNull())
			{
				e = SN::SN_Error("Expected bool, returned " + SN::SN_Expression(this).DisplayValueSN());
			}
		}
		else
		{
			if (!b.GetBool())
			{
				e = SN::SN_Error("Expected true, returned " + SN::SN_Expression(this).DisplayValueSN());
			}
		}
		if (e.IsError())
		{
			e.AddNote(p_Context, this, p_Text);
			e.Log();
			SN::SN_Manager::GetTopManager().ErrorHandler()(e);
		}
	}

	//-----------------------------------------------------------------------
	// Implementation
	//-----------------------------------------------------------------------

	// Numbers
	SN::SN_Value SNI_Expression::DoAdd(SNI_Value * /*p_Other*/) const
	{
		return SN::SN_Error(GetTypeName() + " + operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoSubtract(SNI_Value * /*p_Other*/) const
	{
		return SN::SN_Error(GetTypeName() + " - operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoDivide(SNI_Value * /*p_Other*/) const
	{
		return SN::SN_Error(GetTypeName() + " / operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoMultiply(SNI_Value * /*p_Other*/) const
	{
		return SN::SN_Error(GetTypeName() + " * operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoNegative() const
	{
		return SN::SN_Error(GetTypeName() + " Unary - operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoSquare() const
	{
		return SN::SN_Error(GetTypeName() + "  Square function not implemented.");
	}

	SN::SN_Value SNI_Expression::DoSquareRoot() const
	{
		return SN::SN_Error(GetTypeName() + "  SquareRoot function not implemented.");
	}

	//Logic
	SN::SN_Value SNI_Expression::DoNot() const
	{
		return SN::SN_Error(GetTypeName() + " not function not implemented.");
	}

	SN::SN_Value SNI_Expression::DoRevAnd(SNI_Expression * p_PositiveCase) const
	{
		return SN::SN_Error(GetTypeName() + " 'reverse and' function not implemented.");
	}

	SN::SN_Value SNI_Expression::DoRevOr(SNI_Expression * p_PositiveCase) const
	{
		return SN::SN_Error(GetTypeName() + " 'reverse or' function not implemented.");
	}

	SN::SN_Value SNI_Expression::DoCollapse()
	{
		return SN::SN_Error(GetTypeName() + " collapse function not implemented.");
	}

	SN::SN_Value SNI_Expression::DoUnaryAnd() const
	{
		return SN::SN_Error(GetTypeName() + " 'reverse implies' function not implemented.");
	}

	SN::SN_Value SNI_Expression::DoUnaryOr() const
	{
		return SN::SN_Error(GetTypeName() + " 'reverse implies' function not implemented.");
	}

	SN::SN_Value SNI_Expression::DoAnd(SNI_Expression * /*p_Other*/) const
	{
		return SN::SN_Error(GetTypeName() + " && operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoOr(SNI_Expression * /*p_Other*/) const
	{
		return SN::SN_Error(GetTypeName() + " || operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoIf(SNI_Expression * /*p_PositiveCase*/, SNI_Expression * /*p_NegativeCase*/) const
	{
		return SN::SN_Error(GetTypeName() + " If operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoImplies(SNI_Expression * /*p_PositiveCase*/) const
	{
		return SN::SN_Error(GetTypeName() + " Implies operator not implemented.");
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

	SN::SN_Value SNI_Expression::DoLessThan(SNI_Value * /*p_Other*/) const
	{
		return SN::SN_Error(GetTypeName() + " < operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoGreaterThan(SNI_Value * /*p_Other*/) const
	{
		return SN::SN_Error(GetTypeName() + " > operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoLessEquals(SNI_Value * /*p_Other*/) const
	{
		return SN::SN_Error(GetTypeName() + " <= operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoGreaterEquals(SNI_Value * /*p_Other*/) const
	{
		return SN::SN_Error(GetTypeName() + " >= operator not implemented.");
	}

	//Strings
	SN::SN_Value SNI_Expression::DoConcat(SNI_Value * /*p_Other*/) const
	{
		return SN::SN_Error(GetTypeName() + " + operator not implemented.");
	}

	SN::SN_Value SNI_Expression::DoSubtractLeft(SNI_Value * /*p_Other*/) const
	{
		return SN::SN_Error(GetTypeName() + " SubtractLeft method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoSubtractRight(SNI_Value * /*p_Other*/) const
	{
		return SN::SN_Error(GetTypeName() + " SubtractRight method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoSubtractLeftChar() const
	{
		return SN::SN_Error(GetTypeName() + " SubtractLeftChar method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoSubtractRightChar() const
	{
		return SN::SN_Error(GetTypeName() + " SubtractRightChar method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoSelectRightChar() const
	{
		return SN::SN_Error(GetTypeName() + " SelectRightChar method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoSelectLeftChar() const
	{
		return SN::SN_Error(GetTypeName() + " SelectLeftChar method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoLookaheadLeft() const
	{
		return SN::SN_Error(GetTypeName() + " LookaheadLeft method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoLookaheadRight() const
	{
		return SN::SN_Error(GetTypeName() + " LookaheadRight method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoFile() const
	{
		return SN::SN_Error(GetTypeName() + " File method not implemented.");
	}

	// Conversion
	SN::SN_Value SNI_Expression::DoIntToString() const
	{
		return SN::SN_Error(GetTypeName() + " IntToString method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoStringToInt() const
	{
		return SN::SN_Error(GetTypeName() + " StringToInt method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoDoubleToString() const
	{
		return SN::SN_Error(GetTypeName() + " DoubleToString method not implemented.");
	}

	SN::SN_Value SNI_Expression::DoStringToDouble() const
	{
		return SN::SN_Error(GetTypeName() + " StringToDouble method not implemented.");
	}

	// Sets
	SN::SN_Value SNI_Expression::DoBuildSet() const
	{
		return SN::SN_Error(GetTypeName() + "  BuildSet function not implemented.");
	}

	SN::SN_Value SNI_Expression::DoHasMember(SNI_Value * /*p_Member*/) const
	{
		return SN::SN_Error(GetTypeName() + "  HasMember function not implemented.");
	}

	// ValueSets
	SN::SN_ValueSet SNI_Expression::DoRemove(const SN::SN_Value & /*p_Other*/)
	{
		return SN::SN_Error(GetTypeName() + " Remove not implemented.");
	}

	bool SNI_Expression::DoIsEmpty() const
	{
		ASSERTM(false, GetTypeName() + " IsEmpty not implemented.");
		return false;
	}
}
