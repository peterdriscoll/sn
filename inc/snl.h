#if !defined(SNL_H_INCLUDED)
#define SNL_H_INCLUDED

#pragma once

#include "pgc.h"

#include "sn.h"

#include "snl_parse.h"
#include "snl_validate.h"
#include "snl_characterset.h"

namespace skynet
{
	typedef SNL::SNL_CharacterSet CharacterSet;
	typedef SNL::SNL_Validate Validate;
	typedef SNL::SNL_Parse Parse;
}

#endif // !defined(SNL_H_INCLUDED)