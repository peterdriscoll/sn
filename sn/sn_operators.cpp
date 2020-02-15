#include "sn_operators.h"
#include "sn.h"

#include "sni_valueset.h"

#include "sn_pch.h"

namespace SN
{
	namespace SN_Operators
	{
		// Value sets
		SN::SN_Value GeneralDoOr(const SN::SN_Value &p_Left, const SN::SN_Value &p_Right)
		{
			return SNI_ValueSet::GeneralDoOr(p_Left.GetSNI_Value(), p_Right.GetSNI_Value());
		}

		// Commands
		SN::SN_Expression Assert(const SN::SN_Expression & p_Expression)
		{
			return SN::SN_Function(skynet::Assert, p_Expression);
		}

		SN::SN_Expression PartialAssert(const SN::SN_Expression & p_Expression)
		{
			return SN::SN_Function(skynet::PartialAssert, p_Expression);
		}

		SN::SN_Expression Evaluate(const SN::SN_Expression & p_Expression)
		{
			return SN::SN_Function(skynet::Evaluate, p_Expression);
		}

		SN::SN_Expression PartialEvaluate(const SN::SN_Expression & p_Expression)
		{
			return SN::SN_Function(skynet::PartialEvaluate, p_Expression);
		}

		SN::SN_Expression Meta(const SN::SN_Expression &p_Expression, const SN::SN_Expression &p_MetaLevel)
		{
			return SN::SN_Function(SN::SN_Function(skynet::MetaLevel, p_Expression), p_MetaLevel);
		}

		// Logic
		SN::SN_Expression operator !(const SN::SN_Expression &p_Condition)
		{
			return SN::SN_Function(skynet::Not, p_Condition);
		}

		SN::SN_Expression operator &&(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			return SN::SN_Function(SN::SN_Function(skynet::And, p_Left), p_Right);
		}

		SN::SN_Expression operator ||(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			return SN::SN_Function(SN::SN_Function(skynet::Or, p_Left), p_Right);
		}

		SN::SN_Expression If(const SN::SN_Expression &p_Condition, const SN::SN_Expression &p_PositiveCase, const SN::SN_Expression &p_NegativeCase)
		{
			return SN::SN_Function(SN::SN_Function(SN::SN_Function(skynet::If, p_Condition), p_PositiveCase), p_NegativeCase);
		}

		SN::SN_Expression Implies(const SN::SN_Expression &p_Condition, const SN::SN_Expression &p_PositiveCase)
		{
			return SN::SN_Function(SN::SN_Function(skynet::Implies, p_Condition), p_PositiveCase);
		}

		SN::SN_Expression RevAnd(const SN::SN_Expression &p_Condition, const SN::SN_Expression &p_PositiveCase)
		{
			return SN::SN_Function(SN::SN_Function(skynet::RevAnd, p_Condition), p_PositiveCase);
		}

		SN::SN_Expression RevOr(const SN::SN_Expression &p_PositiveCase, const SN::SN_Expression &p_Condition)
		{
			return SN::SN_Function(SN::SN_Function(skynet::RevOr, p_PositiveCase), p_Condition);
		}

		SN::SN_Expression UnaryAnd(const SN::SN_Expression & p_Condition)
		{
			return SN::SN_Function(skynet::UnaryAnd, p_Condition);
		}

		SN::SN_Expression UnaryOr(const SN::SN_Expression & p_Result)
		{
			return SN::SN_Function(skynet::UnaryOr, p_Result);
		}

		SN::SN_Expression Collapse(const SN::SN_Expression &p_Value)
		{
			return SN::SN_Function(skynet::Collapse, p_Value);
		}

		// Comparison
		SN::SN_Expression operator ==(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			return SN::SN_Function(SN::SN_Function(skynet::Equals, p_Left), p_Right);
		}

		SN::SN_Expression operator <(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			return SN::SN_Function(SN::SN_Function(skynet::LessThan, p_Left), p_Right);
		}

		SN::SN_Expression operator >(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			return SN::SN_Function(SN::SN_Function(skynet::GreaterThan, p_Left), p_Right);
		}

		SN::SN_Expression operator <=(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			return SN::SN_Function(SN::SN_Function(skynet::LessEqual, p_Left), p_Right);
		}

		SN::SN_Expression operator >=(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			return SN::SN_Function(SN::SN_Function(skynet::GreaterEqual, p_Left), p_Right);
		}

		// Strings
		SN::SN_Expression Concat(const SN::SN_Expression & p_Result, const SN::SN_Expression & p_Left)
		{
			return SN::SN_Function(SN::SN_Function(skynet::Concat, p_Result), p_Left);
		}

		SN::SN_Expression SubtractLeft(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Left)
		{
			return SN::SN_Function(SN::SN_Function(skynet::SubtractLeft, p_Result), p_Left);
		}

		SN::SN_Expression SubtractRight(const SN::SN_Expression &p_Result, const SN::SN_Expression &p_Right)
		{
			return SN::SN_Function(SN::SN_Function(skynet::SubtractRight, p_Result), p_Right);
		}

		SN::SN_Expression SubtractLeftChar(const SN::SN_Expression &p_Left)
		{
			return SN::SN_Function(skynet::SubtractLeftChar, p_Left);
		}

		SN::SN_Expression SubtractRightChar(const SN::SN_Expression &p_Left)
		{
			return SN::SN_Function(skynet::SubtractRightChar, p_Left);
		}

		SN::SN_Expression SelectLeftChar(const SN::SN_Expression &p_Left)
		{
			return SN::SN_Function(skynet::SelectLeftChar, p_Left);
		}

		SN::SN_Expression SelectRightChar(const SN::SN_Expression &p_Left)
		{
			return SN::SN_Function(skynet::SelectRightChar, p_Left);
		}

		SN::SN_Expression LookaheadLeft(const SN::SN_Expression &p_Left)
		{
			return SN::SN_Function(skynet::LookaheadLeft, p_Left);
		}

		SN::SN_Expression LookaheadRight(const SN::SN_Expression &p_Left)
		{
			return SN::SN_Function(skynet::LookaheadRight, p_Left);
		}

		SN::SN_Expression File(const SN::SN_Expression &p_Left)
		{
			return SN::SN_Function(skynet::File, p_Left);
		}

		// Numbers
		SN::SN_Expression operator +(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			return SN::SN_Function(SN::SN_Function(skynet::Add, p_Left), p_Right);
		}

		SN::SN_Expression operator -(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			return SN::SN_Function(SN::SN_Function(skynet::Subtract, p_Left), p_Right);
		}

		SN::SN_Expression operator *(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			if (p_Left.Equivalent(p_Right))
			{
				return SN::SN_Function(skynet::Square, p_Left);
			}
			return SN::SN_Function(SN::SN_Function(skynet::Multiply, p_Left), p_Right);
		}

		SN::SN_Expression operator /(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right)
		{
			return SN::SN_Function(SN::SN_Function(skynet::Divide, p_Left), p_Right);
		}

		SN::SN_Expression operator -(const SN::SN_Expression &p_Param)
		{
			return SN::SN_Function(skynet::Negative, p_Param);
		}

		SN::SN_Expression Square(const SN::SN_Expression &p_Expression)
		{
			return SN::SN_Function(skynet::Square, p_Expression);
		}

		SN::SN_Expression SquareRoot(const SN::SN_Expression & p_Expression)
		{
			return SN::SN_Function(skynet::SquareRoot, p_Expression);
		}

		// Escape conversions
		SN::SN_Expression Escape(enum SN::EscapeType p_EscapeType, const SN::SN_Expression &p_Left)
		{
			SN_FunctionDef def(skynet::EscapeCPP);
			switch (p_EscapeType)
			{
			case CPP:
				def = skynet::EscapeCPP;
				break;
			case JSON:
				def = skynet::EscapeJSON;
				break;
			}
			return SN::SN_Function(def, p_Left);
		}

		SN::SN_Expression Unescape(enum SN::EscapeType p_EscapeType, const SN::SN_Expression &p_Left)
		{
			SN_FunctionDef def(skynet::UnescapeCPP);
			switch (p_EscapeType)
			{
			case CPP:
				def = skynet::UnescapeCPP;
				break;
			case JSON:
				def = skynet::UnescapeJSON;
				break;
			}
			return SN::SN_Function(def, p_Left);
		}

		// Conversions
		SN::SN_Expression IntToString(const SN::SN_Expression &p_Left)
		{
			return SN::SN_Function(skynet::IntToString, p_Left);
		}

		SN::SN_Expression StringToInt(const SN::SN_Expression &p_Left)
		{
			return SN::SN_Function(skynet::StringToInt, p_Left);
		}

		SN::SN_Expression DoubleToString(const SN::SN_Expression &p_Left)
		{
			return SN::SN_Function(skynet::DoubleToString, p_Left);
		}

		SN::SN_Expression StringToDouble(const SN::SN_Expression &p_Left)
		{
			return SN::SN_Function(skynet::StringToDouble, p_Left);
		}

		// Inheritance
		SN::SN_Expression IsA(const SN::SN_Expression &p_Child, const SN::SN_Expression &p_Parent)
		{
			return SN::SN_Function(SN::SN_Function(skynet::IsA, p_Child), p_Parent);
		}

		SN::SN_Expression HasA(const SN::SN_Expression &p_Parent, const SN::SN_Expression &p_Component, const SN::SN_Expression &p_Name)
		{
			return SN::SN_Function(SN::SN_Function(SN::SN_Function(skynet::HasA, p_Parent), p_Component), p_Name);
		}

		// Sets
		SN::SN_Expression BuildSet(const SN::SN_Expression &p_Expression)
		{
			return SN::SN_Function(skynet::BuildSet, p_Expression);
		}

		SN::SN_Expression HasMember(const SN::SN_Expression &p_Set, const SN::SN_Expression &p_Member)
		{
			return SN::SN_Function(SN::SN_Function(skynet::HasMember, p_Set), p_Member);
		}

		SN::SN_Expression SubscriptCall(const SN::SN_Expression & p_Mapping, const SN::SN_Expression & p_Index)
		{
			return SN::SN_Function(SN::SN_Function(skynet::Subscript, p_Mapping), p_Index);
		}

		SN::SN_Expression ReverseSubscript(const SN::SN_Expression & p_Mapping, const SN::SN_Expression & p_Value)
		{
			return SN::SN_Function(SN::SN_Function(skynet::ReverseSubscript, p_Mapping), p_Value);
		}

		SN::SN_Expression CountIf(const SN::SN_Expression & p_Mapping, const SN::SN_Expression & p_Lambda)
		{
			return SN::SN_Function(SN::SN_Function(skynet::CountIf, p_Mapping), p_Lambda);
		}

		SN::SN_Expression CountAll(const SN::SN_Expression & p_Mapping)
		{
			return SN::SN_Function(skynet::CountAll, p_Mapping);
		}

		SN::SN_Expression Sum(const SN::SN_Expression & p_Mapping)
		{
			return SN::SN_Function(skynet::Sum, p_Mapping);
		}

		// Function calls
		SN::SN_Expression FunctionCall(const SN::SN_Expression &p_Function, const SN::SN_Expression &p_Parameter)
		{
			return SN::SN_Function(p_Function, p_Parameter);
		}

		SN::SN_Expression FunctionCall(const SN::SN_Expression &p_Function, const SN::SN_Expression &p_Parameter, const SN::SN_Expression &p_Condition)
		{
			return SN::SN_Function(p_Function, p_Parameter, p_Condition);
		}
	}

}
