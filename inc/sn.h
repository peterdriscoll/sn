#ifndef SN_H_INCLUDED
#define SN_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"

#include <string>

//	#define USE_LOGGING

//	The original logging system. Very detailed, and hard to follow.
//	It writes out an indented structure for the logging.
//	Not really useful and slows the processessing down. Better without
//	it. It uses the Log and LogContext classes. It is a replaced with
//	the newer, higher level SNI_Log, which creates the log in the 
//	debugger dashboard.

#ifdef USE_LOGGING
#define LOGGING(l) l
#define LOG_RETURN(context, result) context.LogReturn(result)
#else
#define LOGGING(l)
#define LOG_RETURN(context, result) result
#endif

//#define DIRECT_PASS_TYPE skynet::ReturnValueToVariable
#define DIRECT_PASS_TYPE skynet::DirectPass
#define EVALUATION_TYPE skynet::Lazy /*Lazy, Strict*/
#define LOGIC_TYPE skynet::Parsing
#define TAIL_CALL_OPTIMIZATION true
#define AUTO_EXPAND_NULL true

namespace skynet
{
	enum DebugAction
	{
		None, Run, RunToEnd, Rerun, Debug, CodeBreak, StepOver, StepInto, StepParameter, StepOut, GotoStepCount, GoBackToStepCount, Quit, Abort
	};

	enum EscapeType
	{
		CPP, JSON
	};

	enum DirectPassType
	{
		ReturnValueToVariable, DirectPass
	};

	enum EvaluationType
	{
		Lazy, Strict
	};

	enum LogicType
	{
		Pure, Parsing, FailAsNeg
	};

	enum DefineType
	{
		Inname, Inline
	};
}

#ifdef USE_LOGGING
#include "log.h"
#include "logcontext.h"
#endif

#include "sn_factory.h"
#include "sn_error.h"
#include "sn_manager.h"
#include "sn_transaction.h"
#include "sn_thread.h"
#include "sn_operators.h"
#include "sn_expression.h"
#include "sn_function.h"
#include "sn_functiondef.h"
#include "sn_lambda.h"
#include "sn_null.h"
#include "sn_define.h"
#include "sn_meta.h"
#include "sn_object.h"
#include "sn_string.h"
#include "sn_bool.h"
#include "sn_value.h"
#include "sn_set.h"
#include "sn_valueset.h"
#include "sn_variable.h"
#include "sn_error.h"
#include "sn_char.h"
#include "sn_stringref.h"
#include "sn_number.h"
#include "sn_parameter.h"
#include "sn_local.h"
#include "sn_let.h"
#include "sn_debug.h"
#include "sn_real.h"
#include "sn_cartesian.h"
#include "sn_mapping.h"
#include "sn_vector.h"
#include "sn_class.h"
#include "sn_instance.h"
#include "sn_derived.h"
#include "sn_virtual.h"
#include "sn_domain.h"
#include "sn_operatorvariables.h"
#include "sn_assignment.h"
#include "sn_state.h"
#include "sn_statevalue.h"

// Declare a variable, and tell the variable its variable name.
// When the program and the program model generated the variable will know its human readable name.
// Use on things that can be declared
//   SNI_Variable
//   SNI_Function
#define SN_DECLARE(n) \
    SN::SN_Variable n(string(#n), skynet::Inname, false)

#define SN_DECLARE_INLINE(n) \
	SN::SN_Variable n(string(#n), skynet::Inline, false)

#define SN_LOCAL(n) \
	SN::SN_Variable n(string(#n), skynet::Inname, true)

#define SN_LOCAL_INLINE(n) \
	SN::SN_Variable n(string(#n), skynet::Inline, true)

#define SN_DECLARE_VALUE(n, v) \
    SN::SN_Variable n(string(#n), v, skynet::Inname, false)

#define SN_DECLARE_VALUE_INLINE(n, v) \
	SN::SN_Variable n(string(#n), v, skynet::Inline, false)

#define SN_LOCAL_VALUE(n, v) \
	SN::SN_Variable n(string(#n), v, skynet::Inname, true)

#define SN_LOCAL_VALUE_INLINE(n, v) \
	SN::SN_Variable n(string(#n), v, skynet::Inline, true)

#define SN_LINK(n) \
	(*this)[#n] = n; \
	n.SetName(#n);

#define SN_LINK_INLINE(n) \
	(*this)[#n] = n; \
	n.SetName(#n); \
	n.SetInline(skynet::Inline)

#define SN_LINK_VALUE(n, r, i) \
	(*this)[#n] = n; \
	n.SetName(#n); \
	n.SetValue(r, i)

#define SN_DOMAIN(n) \
    SN::SN_Domain n(string(#n))

#define SN_DEFINE_REAL(C, B, TYPE, NAME)                       \
     SN_EXTERN template class SN_EXPORT SN::SN_Real<TYPE>;     \
     typedef SN::SN_Real<TYPE> NAME;

namespace skynet
{
	// Manager
	typedef SN::SN_Manager Manager;
	typedef SN::SN_Transaction Transaction;
	typedef SN::SN_Thread Thread;

	// Logging
#ifdef USE_LOGGING
	typedef SN::Log Log;
	typedef SN::LogContext LogContext;
#endif

	// types
	typedef SN::SN_Value Value;
	typedef SN::SN_Bool Bool;
	typedef SN::SN_Number Number;
	typedef SN::SN_Char Char;
	typedef SN::SN_String String;
	typedef SN::SN_Mapping Mapping;
	typedef SN::SN_Vector Vector;
	typedef SN::SN_StringRef StringRef;
	typedef SN::SN_ValueSet ValueSet;
	typedef SN::SN_Set Set;
	typedef SN::SN_Error Error;
	typedef SN::SN_Derived Derived;
	typedef SN::SN_Virtual Virtual;
	typedef SN::SN_Domain Domain;
	typedef SN::SN_StateValue Assignment;
	typedef SN::SN_StateValue State;
	typedef SN::SN_StateValue StateValue;

	// Expressions
	typedef SN::SN_Expression Expression;
	typedef SN::SN_Function Function;
	typedef SN::SN_FunctionDef FunctionDef;
	typedef SN::SN_Lambda Lambda;
	typedef SN::SN_Define Define;
	typedef SN::SN_Meta Meta;
	typedef SN::SN_Variable Variable;
	typedef SN::SN_Local Local;
	typedef SN::SN_Let Let;

	SN_APPLY_TYPES(DUMMY, DUMMY, SN_DEFINE_REAL)

	// Functions
	// * Numbers
	extern SN_EXPORT SN::SN_FunctionDef Negative;
	extern SN_EXPORT SN::SN_FunctionDef Add;
	extern SN_EXPORT SN::SN_FunctionDef Subtract;
	extern SN_EXPORT SN::SN_FunctionDef Multiply;
	extern SN_EXPORT SN::SN_FunctionDef Divide;
	extern SN_EXPORT SN::SN_FunctionDef Square;
	extern SN_EXPORT SN::SN_FunctionDef SquareRoot;

	// * Commands
	extern SN_EXPORT SN::SN_FunctionDef Assert;
	extern SN_EXPORT SN::SN_FunctionDef PartialAssert;
	extern SN_EXPORT SN::SN_FunctionDef Evaluate;
	extern SN_EXPORT SN::SN_FunctionDef PartialEvaluate;
	extern SN_EXPORT SN::SN_FunctionDef MetaLevel;

	// * Logic
	extern SN_EXPORT SN::SN_FunctionDef Not;
	extern SN_EXPORT SN::SN_FunctionDef Or;
	extern SN_EXPORT SN::SN_FunctionDef OrNonPartial;
	extern SN_EXPORT SN::SN_FunctionDef And;
	extern SN_EXPORT SN::SN_FunctionDef If;
	extern SN_EXPORT SN::SN_FunctionDef Implies;
	extern SN_EXPORT SN::SN_FunctionDef RevAnd;
	extern SN_EXPORT SN::SN_FunctionDef RevOr;
	extern SN_EXPORT SN::SN_FunctionDef UnaryAnd;
	extern SN_EXPORT SN::SN_FunctionDef UnaryOr;
	extern SN_EXPORT SN::SN_FunctionDef Collapse;
	extern SN_EXPORT SN::SN_FunctionDef Same;

	// * Comparisons
	extern SN_EXPORT SN::SN_FunctionDef Equals;
	extern SN_EXPORT SN::SN_FunctionDef NotEquals;
	extern SN_EXPORT SN::SN_FunctionDef LessThan;
	extern SN_EXPORT SN::SN_FunctionDef GreaterThan;
	extern SN_EXPORT SN::SN_FunctionDef LessEqual;
	extern SN_EXPORT SN::SN_FunctionDef GreaterEqual;
	
	// * Strings
	extern SN_EXPORT SN::SN_FunctionDef Concat;
	extern SN_EXPORT SN::SN_FunctionDef SubtractLeft;
	extern SN_EXPORT SN::SN_FunctionDef SubtractRight;
	extern SN_EXPORT SN::SN_FunctionDef SubtractLeftChar;
	extern SN_EXPORT SN::SN_FunctionDef SubtractRightChar;
	extern SN_EXPORT SN::SN_FunctionDef SelectLeftChar;
	extern SN_EXPORT SN::SN_FunctionDef SelectRightChar;
	extern SN_EXPORT SN::SN_FunctionDef LookaheadLeft;
	extern SN_EXPORT SN::SN_FunctionDef LookaheadRight;
	extern SN_EXPORT SN::SN_FunctionDef LookStringLeft;
	extern SN_EXPORT SN::SN_FunctionDef LookStringRight;
	extern SN_EXPORT SN::SN_FunctionDef File;

	// * Conversions
	extern SN_EXPORT SN::SN_FunctionDef EscapeCPP;
	extern SN_EXPORT SN::SN_FunctionDef UnescapeCPP;
	extern SN_EXPORT SN::SN_FunctionDef EscapeJSON;
	extern SN_EXPORT SN::SN_FunctionDef UnescapeJSON;
	extern SN_EXPORT SN::SN_FunctionDef IntToString;
	extern SN_EXPORT SN::SN_FunctionDef StringToInt;
	extern SN_EXPORT SN::SN_FunctionDef DoubleToString;
	extern SN_EXPORT SN::SN_FunctionDef StringToDouble;

	// * Inheritance
	extern SN_EXPORT SN::SN_FunctionDef IsA;
	extern SN_EXPORT SN::SN_FunctionDef HasA;

	// * Mapping, vector.
	extern SN_EXPORT SN::SN_FunctionDef Subscript;
	extern SN_EXPORT SN::SN_FunctionDef ReverseSubscript;
	extern SN_EXPORT SN::SN_FunctionDef CountIf;
	extern SN_EXPORT SN::SN_FunctionDef CountAll;
	extern SN_EXPORT SN::SN_FunctionDef Sum;

	// * Sets
	extern SN_EXPORT SN::SN_FunctionDef BuildSet;
	extern SN_EXPORT SN::SN_FunctionDef HasMember;

	// * Attributes
	extern SN_EXPORT SN::SN_FunctionDef GetState;
	extern SN_EXPORT SN::SN_FunctionDef GetValue;

	// * Function call
	extern SN_EXPORT SN::SN_Expression FunctionCall;

	// * Constants
	extern SN_EXPORT SN::SN_Bool True;
	extern SN_EXPORT SN::SN_Bool False;

	// * Error
	extern SN_EXPORT SN::SN_Error OK;
	extern SN_EXPORT SN::SN_Error Fail;

	extern SN_EXPORT SN::SN_Error RerunRequest;

	extern SN_EXPORT SN::SN_Expression Null;

	// * Operator Variables

	extern SN_EXPORT SN::SN_OperatorVariables Operators;
};

#endif // SN_H_INCLUDED
