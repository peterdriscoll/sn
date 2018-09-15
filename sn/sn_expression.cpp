#include "sn_expression.h"

#include "sn.h"
#include "sn_cartesian.h"
#include "sn_debug.h"

#include "sni_expression.h"
#include "sni_functiondef.h"
#include "sni_debug.h"
#include "sni_define.h"
#include "sni_lambda.h"
#include "sni_let.h"
#include "sni_local.h"
#include "sni_set.h"
#include "sni_value.h"
#include "sni_valueset.h"
#include "sni_char.h"
#include "sni_string.h"
#include "sni_stringref.h"
#include "sni_bool.h"
#include "sni_number.h"
#include "sni_function.h"
#include "sni_meta.h"
#include "sni_vardef.h"
#include "sni_variable.h"
#include "sni_error.h"
#include "sni_null.h"

#include "sn_pch.h"

namespace SN
{
	SN_Expression::SN_Expression()
		: SN_Base(NULL)
	{
	}

	SN_Expression::SN_Expression(const SN_Expression &p_Expression)
		: SN_Base(p_Expression.m_Expression)
	{
	}

	SN_Expression::SN_Expression(const SN_FunctionDef &p_Expression)
		: SN_Base(dynamic_cast<SNI::SNI_Expression *>(p_Expression.GetSNI_FunctionDef()))
	{
	}

	SN_Expression::SN_Expression(const SN_Function &p_Expression)
		: SN_Base(dynamic_cast<SNI::SNI_Expression *>(p_Expression.GetSNI_Function()))
	{
	}

	SN_Expression::SN_Expression(const SN_Lambda &p_Expression)
		: SN_Base(dynamic_cast<SNI::SNI_Expression *>(p_Expression.GetSNI_Lambda()))
	{
	}

	SN_Expression::SN_Expression(const SN_Let & p_Expression)
		: SN_Base(dynamic_cast<SNI::SNI_Expression *>(p_Expression.GetSNI_Let()))
	{
	}

	SN_Expression::SN_Expression(const SN_Local &p_Expression)
		: SN_Base(dynamic_cast<SNI::SNI_Expression *>(p_Expression.GetSNI_Local()))
	{
	}

	SN_Expression::SN_Expression(const SN_Value &p_Expression)
		: SN_Base(dynamic_cast<SNI::SNI_Expression *>(p_Expression.GetSNI_Value()))
	{
	}

	SN_Expression::SN_Expression(const SN_ValueSet &p_Expression)
		: SN_Base(dynamic_cast<SNI::SNI_Expression *>(p_Expression.GetSNI_ValueSet()))
	{
	}

	SN_Expression::SN_Expression(const SN_Set &p_Expression)
		: SN_Base(dynamic_cast<SNI::SNI_Expression *>(p_Expression.GetSNI_Set()))
	{
	}

	SN_Expression::SN_Expression(const SN_String &p_String)
		: SN_Base(p_String.GetSNI_String())
	{
	}

	SN_Expression::SN_Expression(const SN_StringRef &p_String)
		: SN_Base(dynamic_cast<SNI::SNI_Expression *>(p_String.GetSNI_StringRef()))
	{
	}

	SN_Expression::SN_Expression(const SN_Bool &p_Bool)
		: SN_Base(dynamic_cast<SNI::SNI_Expression *>(p_Bool.GetSNI_Bool()))
	{
	}

	SN_Expression::SN_Expression(const SN_Null &p_Null)
		: SN_Base(dynamic_cast<SNI::SNI_Expression *>(p_Null.GetSNI_Null()))
	{
	}

	SN_Expression::SN_Expression(const SN_Error &p_Error)
		: SN_Base(dynamic_cast<SNI::SNI_Expression *>(p_Error.GetSNI_Error()))
	{
	}

	SN_Expression::SN_Expression(const SN_Instance & p_Instance)
		: SN_Base(dynamic_cast<SNI::SNI_Expression *>(p_Instance.GetSNI_Instance()))
	{
	}

	SN_Expression::SN_Expression(const SN_Define &p_Define)
		: SN_Base(dynamic_cast<SNI::SNI_Expression *>(p_Define.GetSNI_Define()))
	{
	}

	SN_Expression::SN_Expression(const SN_Meta &p_Meta)
		: SN_Base(dynamic_cast<SNI::SNI_Expression *>(p_Meta.GetSNI_Meta()))
	{
	}

	SN_Expression::SN_Expression(const SN_Variable &p_Variable)
		: SN_Base(dynamic_cast<SNI::SNI_Expression *>(p_Variable.GetSNI_Variable()))
	{
	}

	SN_Expression::SN_Expression(const SN_Number &p_Number)
		: SN_Base(dynamic_cast<SNI::SNI_Expression *>(p_Number.GetSNI_Number()))
	{
	}

	SN_APPLY_TYPES(Expression, SN_Base, SN_CONSTRUCTOR_IMPL)

	SN_Expression::SN_Expression(const SN_Char &p_Char)
		: SN_Base(dynamic_cast<SNI::SNI_Expression *>(p_Char.GetSNI_Char()))
	{
	}

	SN_Expression::SN_Expression(const SNI::SNI_Expression *p_Expression)
		: SN_Base(const_cast<SNI::SNI_Expression *>(p_Expression))
	{
	}

	SN_Expression::SN_Expression(const string &p_Value)
		: SN_Base(dynamic_cast<SNI::SNI_Expression *>(new SNI::SNI_String(p_Value)))
	{
	}

	SN_Expression::SN_Expression(char *p_Value)
		: SN_Base(dynamic_cast<SNI::SNI_Expression *>(new SNI::SNI_String(string(p_Value))))
	{
	}

	SN_Expression::SN_Expression(const char &p_Value)
		: SN_Base(dynamic_cast<SNI::SNI_Expression *>(new SNI::SNI_Char(p_Value)))
	{
	}

	SN_Expression::SN_Expression(const long p_Value)
		: SN_Base(dynamic_cast<SNI::SNI_Expression *>(new SNI::SNI_Long(p_Value)))
	{
	}

	SN_Expression::SN_Expression(const double p_Value)
		: SN_Base(dynamic_cast<SNI::SNI_Expression *>(new SNI::SNI_Double(p_Value)))
	{
	}

	SN_Expression::~SN_Expression()
	{
	}

	//////////////////////////////////////////////////////////////////////
	// Queries
	//////////////////////////////////////////////////////////////////////

	SN_Expression SN_Expression::Debug() const
	{
		return SN_Debug(*this);
	}

	SNI::SNI_Expression * SN_Expression::GetSNI_Expression() const
	{
		return m_Expression;
	}

	SNI::SNI_Value * SN_Expression::GetSNI_Value() const
	{
		return dynamic_cast<SNI::SNI_Value *>(m_Expression);
	}

	SNI::SNI_Error * SN_Expression::GetSNI_Error() const
	{
		return dynamic_cast<SNI::SNI_Error *>(m_Expression);
	}

	SNI::SNI_ValueSet * SN_Expression::GetSNI_ValueSet() const
	{
		return dynamic_cast<SNI::SNI_ValueSet *>(m_Expression);
	}

	SN_Expression SN_Expression::GetVariableValue(bool p_IfComplete)
	{
		SNI_Expression *exp = GetSNI_Expression();
		if (exp)
		{
			return exp->GetVariableValue(p_IfComplete);
		}
		return exp;
	}

	SN::SN_Error SN_Expression::ForEach(std::function<SN::SN_Error(const SN_Expression &p_Param, SNI::SNI_World *p_World)> p_Action)
	{
		if (GetSNI_Expression())
		{
			return GetSNI_Expression()->ForEach(p_Action);
		}
		return NULL;
	}

	SN_Cartesian SN_Expression::CartProd(long p_Index, SNI_FunctionDef *p_FunctionDef) const
	{
		return GetSNI_Expression()->CartProd(p_Index, p_FunctionDef);
	}

	bool SN_Expression::IsReferableValue() const
	{
		return m_Expression && m_Expression->IsReferableValue();
	}

	bool SN_Expression::IsError() const
	{
		return m_Expression && m_Expression->IsError();
	}

	SN_ValueSet SN_Expression::DoRemove(const SN_Value &p_Other) const
	{
		return m_Expression->DoRemove(p_Other);
	}

	bool SN_Expression::DoIsEmpty() const
	{
		return m_Expression->DoIsEmpty();
	}

	bool SN_Expression::IsStringValue() const
	{
		return m_Expression->IsStringValue();
	}

	//----------

	bool SN_Expression::GetBool() const
	{
		return m_Expression->GetBool();
	}

	string SN_Expression::GetString() const
	{
		return m_Expression->GetString();
	}

	size_t SN_Expression::Count() const
	{
		return m_Expression->Count();
	}

	size_t SN_Expression::Length() const
	{
		return m_Expression->Length();
	}

	void SN_Expression::Simplify()
	{
		m_Expression->Simplify();
	}

	SN_Expression SN_Expression::SimplifyValue()
	{
		return m_Expression->SimplifyValue();
	}

	bool SN_Expression::IsRequested() const
	{
		return m_Expression->IsRequested();
	}

	SN_Error SN_Expression::AddValue(SN_Expression p_Value, long p_NumWorlds, SNI::SNI_World ** p_WorldList, SNI::SNI_WorldSet *p_WorldSet)
	{
		return GetSNI_Expression()->AddValue(p_Value, p_NumWorlds, p_WorldList, p_WorldSet);
	}

	SNI::SNI_WorldSet * SN_Expression::GetWorldSet()
	{
		return GetSNI_Expression()->GetWorldSet();
	}

	/////////////////////////////////////////////////////////////////////
	// Actions
	/////////////////////////////////////////////////////////////////////

	// Numbers
	SN_Expression SN_Expression::operator +(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator +(*this, p_Other);
	}

	SN_Expression SN_Expression::operator -(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator -(*this, p_Other);
	}

	SN_Expression SN_Expression::operator /(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator /(*this, p_Other);
	}

	SN_Expression SN_Expression::operator *(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator *(*this, p_Other);
	}

	SN_Expression SN_Expression::operator-() const
	{
		return SN_Operators::operator -(*this);
	}

	SN_Expression SN_Expression::Square() const
	{
		return SN_Operators::Square(*this);
	}

	SN_Expression SN_Expression::SquareRoot() const
	{
		return SN_Operators::SquareRoot(*this);
	}


	// Logic
	SN_Expression SN_Expression::operator!() const
	{
		return SN_Operators::operator !(*this);
	}

	SN_Expression SN_Expression::operator&&(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator &&(*this, p_Other);
	}

	SN_Expression SN_Expression::operator||(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator ||(*this, p_Other);
	}

	SN_Expression SN_Expression::If(const SN_Expression &p_PositiveCase, const SN_Expression &p_NegativeCase) const
	{
		return SN_Operators::If(*this, p_PositiveCase, p_NegativeCase);
	}

	SN_Expression SN_Expression::Implies(const SN_Expression &p_PositiveCase) const
	{
		return SN_Operators::Implies(*this, p_PositiveCase);
	}

	SN_Expression SN_Expression::RevAnd(const SN_Expression &p_PositiveCase) const
	{
		return SN_Operators::RevAnd(*this, p_PositiveCase);
	}

	SN_Expression SN_Expression::RevOr(const SN_Expression &p_Condition) const
	{
		return SN_Operators::RevOr(*this, p_Condition);
	}

	SN_Expression SN_Expression::UnaryAnd() const
	{
		return SN_Operators::UnaryAnd(*this);
	}

	SN_Expression SN_Expression::UnaryOr() const
	{
		return SN_Operators::UnaryOr(*this);
	}

	SN_Expression SN_Expression::Collapse() const
	{
		return SN_Operators::Collapse(*this);
	}

	SN_Expression SN_Expression::IsA(const SN_Expression & p_Parent) const
	{
		return SN_Operators::IsA(*this, p_Parent);
	}

	SN_Expression SN_Expression::HasA(const SN_Expression & p_Component, const SN_Expression & p_Name) const
	{
		return SN_Operators::HasA(*this, p_Component, p_Name);
	}

	// Sets
	SN_Expression SN_Expression::BuildSet() const
	{
		return SN_Operators::BuildSet(*this);
	}

	SN_Expression SN_Expression::HasMember(const SN_Expression &p_member) const
	{
		return SN_Operators::HasMember(*this, p_member);
	}

	// Comparison
	SN_Expression SN_Expression::operator==(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator==(*this, p_Other);
	}

	SN_Expression SN_Expression::operator <(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator <(*this, p_Other);
	}

	SN_Expression SN_Expression::operator >(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator >(*this, p_Other);
	}

	SN_Expression SN_Expression::operator <=(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator <=(*this, p_Other);
	}

	SN_Expression SN_Expression::operator >=(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator >=(*this, p_Other);
	}


	// Functions
	SN_Expression SN_Expression::operator()(const SN_Expression &p_Parameter) const
	{
		return SN_Operators::FunctionCall(*this, p_Parameter);
	}

	SN_Expression SN_Expression::operator()(const SN_Expression &p_Parameter, const SN_Expression &p_Condition) const
	{
		return SN_Operators::FunctionCall(*this, p_Parameter, p_Condition);
	}

	// Strings
	SN_Expression SN_Expression::Concat(const SN_Expression & p_Other) const
	{
		return SN_Operators::Concat(*this, p_Other);
	}

	SN_Expression SN_Expression::SubtractLeft(const SN_Expression &p_Other) const
	{
		return SN_Operators::SubtractLeft(*this, p_Other);
	}

	SN_Expression SN_Expression::SubtractRight(const SN_Expression &p_Other) const
	{
		return SN_Operators::SubtractRight(*this, p_Other);
	}

	SN_Expression SN_Expression::SubtractLeftChar() const
	{
		return SN_Operators::SubtractLeftChar(*this);
	}

	SN_Expression SN_Expression::SubtractRightChar() const
	{
		return SN_Operators::SubtractRightChar(*this);
	}

	SN_Expression SN_Expression::SelectLeftChar() const
	{
		return SN_Operators::SelectLeftChar(*this);
	}

	SN_Expression SN_Expression::SelectRightChar() const
	{
		return SN_Operators::SelectRightChar(*this);
	}

	SN_Expression SN_Expression::LookaheadLeft() const
	{
		return SN_Operators::LookaheadLeft(*this);
	}

	SN_Expression SN_Expression::LookaheadRight() const
	{
		return SN_Operators::LookaheadRight(*this);
	}

	SN_Expression SN_Expression::File() const
	{
		return SN_Operators::File(*this);
	}

	// Conversion
	SN_Expression SN_Expression::IntToString() const
	{
		return SN_Operators::IntToString(*this);
	}

	SN_Expression SN_Expression::StringToInt() const
	{
		return SN_Operators::StringToInt(*this);
	}

	SN_Expression SN_Expression::DoubleToString() const
	{
		return SN_Operators::DoubleToString(*this);
	}

	SN_Expression SN_Expression::StringToDouble() const
	{
		return SN_Operators::StringToDouble(*this);
	}

	/////////////////////////////////////////////////////////////////////
	// Actions
	/////////////////////////////////////////////////////////////////////

	// Logic
	SN_Value SN_Expression::DoNot() const
	{
		return m_Expression->DoNot();
	}

	SN_Value SN_Expression::DoAnd(const SN_Expression &p_Other) const
	{
		return m_Expression->DoAnd(dynamic_cast<SNI::SNI_Expression *>(p_Other.m_Expression));
	}

	SN_Value SN_Expression::DoOr(const SN_Expression &p_Other) const
	{
		return m_Expression->DoOr(p_Other.m_Expression);
	}

	SN_Value SN_Expression::DoIf(const SN_Expression & p_Positive, const SN_Expression & p_Negative) const
	{
		return m_Expression->DoIf(p_Positive.m_Expression, p_Negative.m_Expression);
	}

	SN_Value SN_Expression::DoImplies(const SN_Expression & p_Positive) const
	{
		return m_Expression->DoImplies(p_Positive.m_Expression);
	}

	SN_Value SN_Expression::DoRevAnd(const SN_Expression & p_PositiveCase) const
	{
		return m_Expression->DoRevAnd(p_PositiveCase.m_Expression);
	}

	SN_Value SN_Expression::DoRevOr(const SN_Expression & p_Condition) const
	{
		return m_Expression->DoRevOr(p_Condition.m_Expression);
	}

	SN_Value SN_Expression::DoCollapse() const
	{
		return m_Expression->DoCollapse();
	}
}