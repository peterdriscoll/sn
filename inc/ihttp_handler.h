#if !defined(IHTTP_HANDLER_H)
#define IHTTP_HANDLER_H

#pragma once

#include "sn_factory.h"

class IHTTP_Handler
{
	DEFINE_INTERFACE(IHTTP_Handler);

public:
	virtual ~IHTTP_Handler() {};
	virtual bool handle_response(const char *p_Path, const char *p_ParamString, const char *p_Extension) = 0;
	virtual const char *response_data() = 0;
	virtual const char *extension() = 0;
};

#endif // IHTTP_HANDLER_H