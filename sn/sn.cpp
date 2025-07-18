#include "sn.h"

#include "sni_null.h"

// Operations
#include "sni_unary.h"
#include "sni_binary.h"
#include "sni_comparison.h"
#include "sni_functioncall.h"

// Numbers
#include "sni_negative.h"
#include "sni_add.h"
#include "sni_subtract.h"
#include "sni_divide.h"
#include "sni_multiply.h"
#include "sni_square.h"
#include "sni_squareroot.h"

// Commands
#include "sni_assert.h"
#include "sni_partialassert.h"
#include "sni_evaluate.h"
#include "sni_partialevaluate.h"
#include "sni_metalevel.h"

// Logic
#include "sni_not.h"
#include "sni_or.h"
#include "sni_and.h"
#include "sni_if.h"
#include "sni_implies.h"
#include "sni_revand.h"
#include "sni_revor.h"
#include "sni_unaryand.h"
#include "sni_unaryor.h"
#include "sni_collapse.h"
#include "sni_same.h"

// Comparisons
#include "sni_equals.h"
#include "sni_notequals.h"
#include "sni_lessthan.h"
#include "sni_greaterthan.h"
#include "sni_lessequals.h"
#include "sni_greaterequals.h"

// Strings
#include "sni_subtractleft.h"
#include "sni_subtractright.h"
#include "sni_subtractleftchar.h"
#include "sni_subtractrightchar.h"
#include "sni_selectleftchar.h"
#include "sni_selectrightchar.h"
#include "sni_lookaheadleft.h"
#include "sni_lookaheadright.h"
#include "sni_lookstringleft.h"
#include "sni_lookstringright.h"

// Escape conversion
#include "sni_escape.h"
#include "sni_unescape.h"

// Conversion
#include "sni_inttostring.h"
#include "sni_stringtoint.h"
#include "sni_doubletostring.h"
#include "sni_stringtodouble.h"

// Inheritance
#include "sni_isa.h"
#include "sni_hasa.h"

// Mappings
#include "sni_subscript.h"
#include "sni_reversesubscript.h"
#include "sni_countif.h"
#include "sni_countall.h"
#include "sni_sum.h"

// Sets
#include "sni_buildset.h"
#include "sni_hasmember.h"

// Attributes
#include "sni_getstate.h"
#include "sni_getvalue.h"

// State
#include "sni_assign.h"
#include "sni_state.h"
#include "sni_statebuffer.h"
#include "sni_statevalue.h"

#include "sn_pch.h"

namespace skynet
{
	// Numbers
	SN::SN_FunctionDef Negative(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_Negative()));
	SN::SN_FunctionDef Add(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_Add()));
	SN::SN_FunctionDef Subtract(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_Subtract()));
	SN::SN_FunctionDef Divide(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_Divide()));
	SN::SN_FunctionDef Multiply(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_Multiply()));
	SN::SN_FunctionDef Square(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_Square()));
	SN::SN_FunctionDef SquareRoot(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_SquareRoot()));

	SN::SN_FunctionDef Assert(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_Assert()));
	SN::SN_FunctionDef PartialAssert(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_PartialAssert()));
	SN::SN_FunctionDef Evaluate(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_Evaluate()));
	SN::SN_FunctionDef PartialEvaluate(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_PartialEvaluate()));
	SN::SN_FunctionDef MetaLevel(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_MetaLevel()));

	// Logic
	SN::SN_FunctionDef Not(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_Not()));
	SN::SN_FunctionDef Or(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_Or()));
	SN::SN_FunctionDef OrNonPartial(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_Or(false)));
	SN::SN_FunctionDef And(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_And()));
	SN::SN_FunctionDef If(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_If()));
	SN::SN_FunctionDef Implies(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_Implies()));
	SN::SN_FunctionDef RevAnd(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_RevAnd()));
	SN::SN_FunctionDef RevOr(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_RevOr()));
	SN::SN_FunctionDef UnaryAnd(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_UnaryAnd()));
	SN::SN_FunctionDef UnaryOr(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_UnaryOr()));
	SN::SN_FunctionDef Collapse(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_Collapse()));
	SN::SN_FunctionDef Same(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_Same()));

	// Comparison
	SN::SN_FunctionDef Equals(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_Equals()));
	SN::SN_FunctionDef NotEquals(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_NotEquals()));
	SN::SN_FunctionDef LessThan(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_LessThan()));
	SN::SN_FunctionDef GreaterThan(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_GreaterThan()));
	SN::SN_FunctionDef LessEqual(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_LessEquals()));
	SN::SN_FunctionDef GreaterEqual(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_GreaterEquals()));

	// Strings
	SN::SN_FunctionDef Concat(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_Concat()));
	SN::SN_FunctionDef SubtractLeft(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_SubtractLeft()));
	SN::SN_FunctionDef SubtractRight(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_SubtractRight()));
	SN::SN_FunctionDef SubtractLeftChar(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_SubtractLeftChar()));
	SN::SN_FunctionDef SubtractRightChar(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_SubtractRightChar()));
	SN::SN_FunctionDef SelectLeftChar(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_SelectLeftChar()));
	SN::SN_FunctionDef SelectRightChar(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_SelectRightChar()));
	SN::SN_FunctionDef LookaheadLeft(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_LookaheadLeft()));
	SN::SN_FunctionDef LookaheadRight(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_LookaheadRight()));
	SN::SN_FunctionDef LookStringLeft(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_LookStringLeft()));
	SN::SN_FunctionDef LookStringRight(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_LookStringRight()));
	SN::SN_FunctionDef File(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_File()));

	// Escape Conversions
	SN::SN_FunctionDef EscapeCPP(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_Escape(skynet::CPP)));
	SN::SN_FunctionDef UnescapeCPP(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_Unescape(skynet::CPP)));
	SN::SN_FunctionDef EscapeJSON(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_Escape(skynet::JSON)));
	SN::SN_FunctionDef UnescapeJSON(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_Unescape(skynet::JSON)));

	// Conversions
	SN::SN_FunctionDef IntToString(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_IntToString()));
	SN::SN_FunctionDef StringToInt(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_StringToInt()));
	SN::SN_FunctionDef DoubleToString(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_DoubleToString()));
	SN::SN_FunctionDef StringToDouble(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_StringToDouble()));

	// Inheritance
	SN::SN_FunctionDef IsA(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_IsA()));
	SN::SN_FunctionDef HasA(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_HasA()));

	// Mapping/Vector
	SN::SN_FunctionDef Subscript(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_Subscript()));
	SN::SN_FunctionDef ReverseSubscript(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_ReverseSubscript()));
	SN::SN_FunctionDef CountIf(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_CountIf()));
	SN::SN_FunctionDef CountAll(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_CountAll()));
	SN::SN_FunctionDef Sum(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_Sum()));

	// Sets
	SN::SN_FunctionDef BuildSet(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_BuildSet()));
	SN::SN_FunctionDef HasMember(dynamic_cast<SNI::SNI_FunctionDef *>(new SNI::SNI_HasMember()));

	// Attributes
	SN::SN_FunctionDef GetState(dynamic_cast<SNI::SNI_FunctionDef*>(new SNI::SNI_GetState()));
	SN::SN_FunctionDef GetValue(dynamic_cast<SNI::SNI_FunctionDef*>(new SNI::SNI_GetValue()));

	// Imperative
	SN::SN_FunctionDef Assign(dynamic_cast<SNI::SNI_FunctionDef*>(new SNI::SNI_Assign()));

	// Function Call
	SN::SN_Expression FunctionCall(dynamic_cast<SNI::SNI_Expression *>(new SNI::SNI_FunctionCall()));

	// Constants
	SN::SN_Bool True(true);
	SN::SN_Bool False(false);

	// Constants
	SN::SN_Error OK(true, false);
	SN::SN_Error Fail(false, false);

	SN::SN_Error RerunRequest(false, false, "Rerun request.");

	SN::SN_Expression Null(dynamic_cast<SNI::SNI_Expression *>(new SNI::SNI_Null()));

	// Operator variables
	SN::SN_OperatorVariables Operators;
}
