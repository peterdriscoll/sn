#include "sn.h"

namespace SN
{
	template <typename F>
	class SN_Operators2
	{
		// Value sets
		static SN::SN_Value GeneralDoOr(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right)
		{
			return SNI_ValueSet::GeneralDoOr(p_Left.GetSNI_Value(), p_Right.GetSNI_Value());
		}

		// Commands
		static SN::SN_Expression Assert(const SN::SN_Expression & p_Expression)
		{
			return SN::SN_Function(skynet::Assert, p_Expression);
		}

		static SN::SN_Expression PartialAssert(const SN::SN_Expression & p_Expression)
		{
			return SN::SN_Function(skynet::PartialAssert, p_Expression);
		}

		static SN::SN_Expression Evaluate(const SN::SN_Expression & p_Expression)
		{
			return SN::SN_Function(skynet::Evaluate, p_Expression);
		}

		static SN::SN_Expression PartialEvaluate(const SN::SN_Expression & p_Expression)
		{
			return SN::SN_Function(skynet::PartialEvaluate, p_Expression);
		}

		static SN::SN_Expression BuildMeta(const SN::SN_Expression &p_Expression, const SN::SN_Expression &p_MetaLevel)
		{
			return F(F(skynet::MetaLevel, p_Expression), p_MetaLevel);
		}

		static SN::SN_Expression Notes(const SN::SN_Expression &p_Expression, const SN::SN_Expression &p_Before, const SN::SN_Expression &p_After)
		{
			return p_Expression;
		}

		// Logic
		static SN::SN_Expression operator !(const SN::SN_Expression &p_Condition)
		{
			return F(skynet::Not, p_Condition);
		}

		static SN::SN_Expression operator &&(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			return F(F(skynet::And, p_Left), p_Right);
		}

		static SN::SN_Expression operator ||(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			return F(F(skynet::Or, p_Left), p_Right);
		}

		static SN::SN_Expression If(const SN::SN_Expression &p_Condition, const SN::SN_Expression &p_PositiveCase, const SN::SN_Expression &p_NegativeCase)
		{
			return F(F(F(skynet::If, p_Condition), p_PositiveCase), p_NegativeCase);
		}

		static SN::SN_Expression Implies(const SN::SN_Expression &p_Condition, const SN::SN_Expression &p_PositiveCase)
		{
			return F(F(skynet::Implies, p_Condition), p_PositiveCase);
		}

		SN::SN_Expression RevAnd(const SN::SN_Expression &p_Condition, const SN::SN_Expression &p_PositiveCase)
		{
			return F(F(skynet::RevAnd, p_Condition), p_PositiveCase);
		}

		SN::SN_Expression RevOr(const SN::SN_Expression &p_PositiveCase, const SN::SN_Expression &p_Condition)
		{
			return F(F(skynet::RevOr, p_PositiveCase), p_Condition);
		}

		SN::SN_Expression UnaryAnd(const SN::SN_Expression & p_Condition)
		{
			return F(skynet::UnaryAnd, p_Condition);
		}

		SN::SN_Expression UnaryOr(const SN::SN_Expression & p_Result)
		{
			return F(skynet::UnaryOr, p_Result);
		}

		SN::SN_Expression Collapse(const SN::SN_Expression &p_Value)
		{
			return F(skynet::Collapse, p_Value);
		}

		// Comparison
		static SN::SN_Expression operator ==(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			return F(F(skynet::Equals, p_Left), p_Right);
		}

		static SN::SN_Expression operator !=(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			return F(F(skynet::NotEquals, p_Left), p_Right);
		}

		static SN::SN_Expression operator <(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			return F(F(skynet::LessThan, p_Left), p_Right);
		}

		static SN::SN_Expression operator >(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			return F(F(skynet::GreaterThan, p_Left), p_Right);
		}

		static SN::SN_Expression operator <=(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			return F(F(skynet::LessEqual, p_Left), p_Right);
		}

		static SN::SN_Expression operator >=(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			return F(F(skynet::GreaterEqual, p_Left), p_Right);
		}

		// Strings
		static SN::SN_Expression Concat(const SN::SN_Expression & p_Result, const SN::SN_Expression & p_Left)
		{
			return F(F(skynet::Concat, p_Result), p_Left);
		}

		static SN::SN_Expression SubtractLeft(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Left)
		{
			return F(F(skynet::SubtractLeft, p_Result), p_Left);
		}

		static SN::SN_Expression SubtractRight(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Right)
		{
			return F(F(skynet::SubtractRight, p_Result), p_Right);
		}

		static SN::SN_Expression SubtractLeftChar(const SN::SN_Expression &p_Left)
		{
			return F(skynet::SubtractLeftChar, p_Left);
		}

		static SN::SN_Expression SubtractRightChar(const SN::SN_Expression &p_Left)
		{
			return F(skynet::SubtractRightChar, p_Left);
		}

		static SN::SN_Expression SelectLeftChar(const SN::SN_Expression &p_Left)
		{
			return F(skynet::SelectLeftChar, p_Left);
		}

		static SN::SN_Expression SelectRightChar(const SN::SN_Expression &p_Left)
		{
			return F(skynet::SelectRightChar, p_Left);
		}

		static SN::SN_Expression LookaheadLeft(const SN::SN_Expression &p_Left)
		{
			return F(skynet::LookaheadLeft, p_Left);
		}

		SN::SN_Expression LookaheadRight(const SN::SN_Expression &p_Left)
		{
			return F(skynet::LookaheadRight, p_Left);
		}

		SN::SN_Expression LookStringLeft(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			return F(F(skynet::LookStringLeft, p_Left), p_Right);
		}

		SN::SN_Expression LookStringRight(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			return F(F(skynet::LookStringRight, p_Left), p_Right);
		}

		SN::SN_Expression File(const SN::SN_Expression &p_Left)
		{
			return F(skynet::File, p_Left);
		}

		// Numbers
		SN::SN_Expression operator +(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			return F(F(skynet::Add, p_Left), p_Right);
		}

		SN::SN_Expression operator -(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			return F(F(skynet::Subtract, p_Left), p_Right);
		}

		SN::SN_Expression operator *(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			if (p_Left.Equivalent(p_Right))
			{
				return F(skynet::Square, p_Left);
			}
			return F(F(skynet::Multiply, p_Left), p_Right);
		}

		static SN::SN_Expression operator /(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			return F(F(skynet::Divide, p_Left), p_Right);
		}

		SN::SN_Expression operator -(const SN::SN_Expression &p_Param)
		{
			return F(skynet::Negative, p_Param);
		}

		SN::SN_Expression Square(const SN::SN_Expression &p_Expression)
		{
			return F(skynet::Square, p_Expression);
		}

		SN::SN_Expression SquareRoot(const SN::SN_Expression & p_Expression)
		{
			return F(skynet::SquareRoot, p_Expression);
		}

		// Escape conversions
		SN::SN_Expression Escape(enum skynet::EscapeType p_EscapeType, const SN::SN_Expression &p_Left)
		{
			SN_FunctionDef def(skynet::EscapeCPP);
			switch (p_EscapeType)
			{
			case skynet::CPP:
				def = skynet::EscapeCPP;
				break;
			case skynet::JSON:
				def = skynet::EscapeJSON;
				break;
			}
			return F(def, p_Left);
		}

		SN::SN_Expression Unescape(enum skynet::EscapeType p_EscapeType, const SN::SN_Expression &p_Left)
		{
			SN_FunctionDef def(skynet::UnescapeCPP);
			switch (p_EscapeType)
			{
			case skynet::CPP:
				def = skynet::UnescapeCPP;
				break;
			case skynet::JSON:
				def = skynet::UnescapeJSON;
				break;
			}
			return F(def, p_Left);
		}

		// Conversions
		SN::SN_Expression IntToString(const SN::SN_Expression &p_Left)
		{
			return F(skynet::IntToString, p_Left);
		}

		SN::SN_Expression StringToInt(const SN::SN_Expression &p_Left)
		{
			return F(skynet::StringToInt, p_Left);
		}

		SN::SN_Expression DoubleToString(const SN::SN_Expression &p_Left)
		{
			return F(skynet::DoubleToString, p_Left);
		}

		SN::SN_Expression StringToDouble(const SN::SN_Expression &p_Left)
		{
			return F(skynet::StringToDouble, p_Left);
		}

		// Inheritance
		SN::SN_Expression IsA(const SN::SN_Expression &p_Child, const SN::SN_Expression &p_Parent)
		{
			return F(F(skynet::IsA, p_Child), p_Parent);
		}

		SN::SN_Expression HasA(const SN::SN_Expression &p_Parent, const SN::SN_Expression &p_Component, const SN::SN_Expression &p_Name)
		{
			return F(F(F(skynet::HasA, p_Parent), p_Component), p_Name);
		}

		// Sets
		SN::SN_Expression BuildSet(const SN::SN_Expression &p_Expression)
		{
			return F(skynet::BuildSet, p_Expression);
		}

		SN::SN_Expression HasMember(const SN::SN_Expression &p_Set, const SN::SN_Expression &p_Member)
		{
			return F(F(skynet::HasMember, p_Set), p_Member);
		}

		SN::SN_Expression SubscriptCall(const SN::SN_Expression & p_Mapping, const SN::SN_Expression & p_Index)
		{
			return F(F(skynet::Subscript, p_Mapping), p_Index);
		}

		SN::SN_Expression ReverseSubscript(const SN::SN_Expression & p_Mapping, const SN::SN_Expression & p_Value)
		{
			return F(F(skynet::ReverseSubscript, p_Mapping), p_Value);
		}

		SN::SN_Expression CountIf(const SN::SN_Expression & p_Mapping, const SN::SN_Expression & p_Lambda)
		{
			return F(F(skynet::CountIf, p_Mapping), p_Lambda);
		}

		SN::SN_Expression CountAll(const SN::SN_Expression & p_Mapping)
		{
			return F(skynet::CountAll, p_Mapping);
		}

		SN::SN_Expression Sum(const SN::SN_Expression & p_Mapping)
		{
			return F(skynet::Sum, p_Mapping);
		}

		// Function calls
		SN::SN_Function F(const SN::SN_Expression &p_Function, const SN::SN_Expression &p_Parameter)
		{
			// return SN::SN_Function(p_Function, p_Parameter);
			return SN::SN_Function(SN::SN_Function(skynet::F, p_Function), p_Parameter);
		}
	}
}
