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

// Language elements
#include "sni_meta.h"
#include "sni_lambda.h"
#include "sni_function.h"
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

// Numbers
#include "sni_negative.h"
#include "sni_add.h"
#include "sni_subtract.h"
#include "sni_divide.h"
#include "sni_multiply.h"
#include "sni_square.h"
#include "sni_squareroot.h"

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
#include "sni_file.h"

// Conversion
#include "sni_inttostring.h"
#include "sni_stringtoint.h"
#include "sni_doubletostring.h"
#include "sni_stringtodouble.h"

// Utility
#include "utility.h"
#include "sni_helpers.h"
#include "sni_manager.h"
#include "sni_cartesian.h"
#include "sni_cartcall.h"
#include "sni_cart.h"
#include "sni_splitter.h"

// Sets
#include "sni_buildset.h"
#include "sni_hasmember.h"

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

using namespace std;

#endif // !defined(SN_PCH_H_INCLUDED)
