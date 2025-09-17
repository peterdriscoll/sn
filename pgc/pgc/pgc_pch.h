#pragma once

#include "config.h"

#include "pgc_ready.h"
#include "pgc_registry.h"
#include "sref.h"
#include "memberref.h"
#include "ref.h"
#include "pgc_user.h"
#include "pgc_promotionstrategy.h"
#include "pgc_base.h"
#include "pgc_transaction.h"
#include "pgc_stacktransaction.h"
#include "pgc_task.h"
#include "pgc_promotion.h"
#include "promotioncapture.h"
#include "thread.h"
#include "threadregistry.h"
#include "debug.h"

#include <iostream>
#include <typeinfo>       // operator typeid
#include <string>
#include <vector>
#include <unordered_map>