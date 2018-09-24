#if !defined(SN_EXPRESSION_H_INCLUDED)
#define SN_EXPRESSION_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"

#include "sn_base.h"
#include "sn_macros.h"

#include <vector>
#include <functional>

using namespace std;

namespace SNI
{
	class SNI_Expression;
	class SNI_Error;
	class SNI_Value;
    class SNI_ValueSet;
	class SNI_World;
	class SNI_WorldSet;
	class SNI_FunctionDef;
	class SNI_Cartesian;
}

namespace SN
{
	class SN_FunctionDef;
	class SN_Function;
	class SN_Lambda;
	class SN_Let;
	class SN_Local;
	class SN_Value;
	class SN_ValueSet;
	class SN_Set;
	class SN_String;
	class SN_StringRef;
	class SN_Bool;
	class SN_Null;
	class SN_Define;
	class SN_Meta;
	class SN_Variable;
	class SN_Error;
	class SN_Number;
	class SN_Char;
	class SN_Cartesian;
	class SN_Class;
	class SN_Instance;

	class SN_Context
	{
	public:
		SN_ExpressionList m_ParameterList;
		SNI::SNI_World         *m_world;
	};

	template <typename T>
	class SN_Real;

	SN_APPLY_TYPES(DUMMY, DUMMY, SN_FORWARD)

	class SN_EXPORT SN_Expression : public SN_Base<SNI::SNI_Expression, SN_Expression, SN_Error>
	{
	public:
		// Constructors
		SN_Expression();
		SN_Expression(const SN_Expression &p_Expression);

		// Constructors values
		SN_Expression(const SN_FunctionDef &p_Expression);
		SN_Expression(const SN_Function &p_Expression);
		SN_Expression(const SN_Lambda &p_Expression);
		SN_Expression(const SN_Let &p_Expression);
		SN_Expression(const SN_Local &p_Expression);
		SN_Expression(const SN_Value &p_Expression);
		SN_Expression(const SN_ValueSet &p_Expression);
		SN_Expression(const SN_Set &p_Expression);
		SN_Expression(const SN_String &p_String);
		SN_Expression(const SN_StringRef &p_String);
		SN_Expression(const SN_Bool &p_Bool);
		SN_Expression(const SN_Define &p_Define);
		SN_Expression(const SN_Meta &p_Meta);
		SN_Expression(const SN_Variable &p_Variable);
		SN_Expression(const SN_Number &p_Number);
		SN_Expression(const SN_Char &p_Char);
		SN_Expression(const SN_Null &p_Null);
		SN_Expression(const SN_Error &p_Error);
		SN_Expression(const SN_Class &p_Class);
		SN_Expression(const SN_Instance &p_Instance);

		SN_APPLY_TYPES(SN_Expression, DUMMY, SN_CONSTRUCTOR)

		SN_Expression(const SNI::SNI_Expression *p_other);

		//SN_Expression(bool p_Bool);
		SN_Expression(const long p_Value);
		SN_Expression(const double p_Value);
		SN_Expression(const string &p_Value);
		SN_Expression(const char &p_Value);
		SN_Expression(char *p_Value);

		// Destructor
		virtual ~SN_Expression();

		bool IsReferableValue() const;
		bool IsError() const;
		bool DoIsEmpty() const;
		bool IsStringValue() const;
		SN_ValueSet DoRemove(const SN_Value &p_Other) const;
		void Simplify();
		SN_Expression SimplifyValue();
		bool IsRequested() const;
		SN_Error AddValue(SN_Expression p_Value, long p_NumWorlds, SNI::SNI_World **p_WorldList, SNI::SNI_WorldSet *p_WorldSet);
		SNI::SNI_WorldSet *GetWorldSet();

		/////////////////////////////////////////////////////////////////////
		// Constructs
		/////////////////////////////////////////////////////////////////////

		// Numbers
		SN_Expression operator +(const SN_Expression &p_Other) const;
		SN_Expression operator -(const SN_Expression &p_Other) const;
		SN_Expression operator /(const SN_Expression &p_Other) const;
		SN_Expression operator *(const SN_Expression &p_Other) const;
		SN_Expression operator -() const;

		SN_Expression Square() const;
		SN_Expression SquareRoot() const;

		// Logic
		SN_Expression operator!() const;
		SN_Expression operator&&(const SN_Expression &right) const;
		SN_Expression operator||(const SN_Expression &right) const;

		SN_Expression If(const SN_Expression &p_PositiveCase, const SN_Expression &p_NegativeCase) const;
		SN_Expression Implies(const SN_Expression &p_PositiveCase) const;
		SN_Expression RevAnd(const SN_Expression &p_PositiveCase) const;
		SN_Expression RevOr(const SN_Expression &p_Condition) const;
		SN_Expression UnaryAnd() const;
		SN_Expression UnaryOr() const;
		SN_Expression Collapse() const;

		// Inheritance
		SN_Expression IsA(const SN_Expression &p_Parent) const;
		SN_Expression HasA(const SN_Expression &p_Component, const SN_Expression &p_Name) const;


		// Comparison
		SN_Expression operator ==(const SN_Expression &p_Other) const;
		SN_Expression operator <(const SN_Expression &p_Other) const;
		SN_Expression operator >(const SN_Expression &p_Other) const;
		SN_Expression operator <=(const SN_Expression &p_Other) const;
		SN_Expression operator >=(const SN_Expression &p_Other) const;

		// Function
		SN_Expression operator()(const SN_Expression &p_Parameter) const;
		SN_Expression operator()(const SN_Expression &p_Parameter, const SN_Expression &p_Condition) const;


		// Sets
		SN_Expression BuildSet() const;
		SN_Expression HasMember(const SN_Expression &p_member) const;

		// Strings
		SN_Expression Concat(const SN_Expression &p_Other) const;
		SN_Expression SubtractLeft(const SN_Expression &p_Other) const;
		SN_Expression SubtractRight(const SN_Expression &p_Other) const;
		SN_Expression SubtractLeftChar() const;
		SN_Expression SubtractRightChar() const;
		SN_Expression SelectLeftChar() const;
		SN_Expression SelectRightChar() const;
		SN_Expression LookaheadLeft() const;
		SN_Expression LookaheadRight() const;
		SN_Expression File() const;

		// Conversions
		SN_Expression IntToString() const;
		SN_Expression StringToInt() const;
		SN_Expression DoubleToString() const;
		SN_Expression StringToDouble() const;

		/////////////////////////////////////////////////////////////////////
		// Actions
		/////////////////////////////////////////////////////////////////////

		// Logic
		SN_Value DoNot() const;
		SN_Value DoOr(const SN_Expression &p_Other) const;
		SN_Value DoAnd(const SN_Expression &p_Other) const;
		SN_Value DoIf(const SN_Expression &p_Positive, const SN_Expression &p_Negative) const;
		SN_Value DoImplies(const SN_Expression &p_Positive) const;
		SN_Value DoCollapse() const;
		SN_Value DoRevAnd(const SN_Expression &p_PositiveCase) const;
		SN_Value DoRevOr(const SN_Expression &p_Condition) const;

		bool GetBool() const;
		string GetString() const;
		size_t Count() const;
		size_t Length() const;
		SN_Expression Debug() const;
		SNI::SNI_Expression * GetSNI_Expression() const;
		SNI::SNI_Value * GetSNI_Value() const;
		SNI::SNI_Error * GetSNI_Error() const;
		SNI::SNI_ValueSet * GetSNI_ValueSet() const;
		SN_Expression GetVariableValue(bool p_IfComplete = true);
		SN::SN_Error ForEach(std::function<SN::SN_Error(const SN_Expression &p_Param, SNI::SNI_World *p_World)> p_Action);
		SN_Cartesian CartProd(long p_Index, SNI::SNI_FunctionDef *p_FunctionDef = NULL) const;
	};

	typedef vector<SN_Expression> SN_ExpressionList;

	template <typename T>
	T Is(const SN_Expression &p_Expression)
	{
		if (dynamic_cast<SNI::SNI_Null *>(p_Expression.GetSNI_Expression()))
		{
			return NULL;
		}
		if (dynamic_cast<SNI::SNI_Error *>(p_Expression.GetSNI_Expression()))
		{
			return NULL;
		}
		return dynamic_cast<T>(p_Expression.GetSNI_Expression());
	}

}

#endif // !defined(SN_EXPRESSION_H_INCLUDED)
