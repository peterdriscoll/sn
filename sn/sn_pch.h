#if !defined(SN_PCH_H_INCLUDED)
#define SN_PCH_H_INCLUDED

#pragma once

#include "pgc.h"

#include "sn.h"

// Worlds
#include "sni_world.h"
#include "sni_worldset.h"

// Delayed
#include "sni_delayedprocessor.h"
#include "sni_delayedcall.h"

// Other
#include "sni_replacement.h"
#include "sni_frame.h"
#include "sni_http_handler.h"

// Language elements
#include "sni_meta.h"
#include "sni_lambda.h"
#include "sni_function.h"
#include "sni_incompletefunction.h"
#include "sni_define.h"
#include "sni_local.h"
#include "sni_let.h"
#include "sni_debug.h"

// Base types
#include "sni_expression.h"
#include "sni_value.h"
#include "sni_valueset.h"
#include "sni_variable.h"
#include "sni_real.h"
#include "sni_number.h"
#include "sni_mapping.h"
#include "sni_vector.h"
#include "sni_class.h"
#include "sni_instance.h"
#include "sni_derived.h"
#include "sni_virtual.h"
#include "sni_domain.h"

// Exception types
#include "sni_null.h"
#include "sni_error.h"

// Types
#include "sni_string.h"
#include "sni_stringref.h"
#include "sni_bool.h"
#include "sni_char.h"
#include "sni_set.h"

// Operations
#include "sni_functiondef.h"
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
#include "sni_file.h"

// Escape conversion
#include "sni_escape.h"
#include "sni_unescape.h"

// Conversion
#include "sni_inttostring.h"
#include "sni_stringtoint.h"
#include "sni_doubletostring.h"
#include "sni_stringtodouble.h"

// Utility
#include "utility.h"
#include "sni_log.h"
#include "log.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sni_logbuffer.h"
#include "sni_helpers.h"
#include "sni_manager.h"
#include "sni_transaction.h"
#include "sni_user.h"
#include "sni_cartesian.h"
#include "sni_cart.h"
#include "sni_cartcall.h"
#include "sni_cartunify.h"
#include "sni_splitter.h"
#include "sni_debugcommand.h"
#include "sni_thread.h"
#include "sni_displayoptions.h"
#include "sni_user.h"

// Inheritance
#include "sni_attributedef.h"
#include "sni_isa.h"
#include "sni_hasa.h"

// Mapping, Vector
#include "sni_subscript.h"
#include "sni_reversesubscript.h"
#include "sni_countif.h"
#include "sni_countall.h"
#include "sni_sum.h"

// Sets
#include "sni_buildset.h"
#include "sni_hasmember.h"

// Imperative
#include "sni_assign.h"
#include "sni_state.h"
#include "sni_statebuffer.h"
#include "sni_statevalue.h"

// Attributes
#include "sni_getstate.h"
#include "sni_getvalue.h"

#include <time.h> // time_t, tm, time, localtime, strftime
#include <direct.h>
#include <string>
#include <climits>
#include <math.h>
#include <vector>
#include <unordered_map>
#include <limits>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <utility>
#include <signal.h>

using namespace std;

#endif // !defined(SN_PCH_H_INCLUDED)
