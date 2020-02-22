#if !defined(SNL_H_INCLUDED)
#define SNL_H_INCLUDED

#pragma once

#include "pgc.h"

#include "sn.h"

#include "snl_parser.h"
#include "snl_validate.h"
#include "snl_characterset.h"

namespace skynet
{
	typedef SNL::SNL_CharacterSet CharacterSet;
	typedef SNL::SNL_Validate Validate;
	typedef SNL::SNL_Parser Parser;
}

#endif // !defined(SNL_H_INCLUDED)