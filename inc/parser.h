#if !defined(PARSER_H_INCLUDED)
#define PARSER_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn.h"

namespace SN
{
	class SN_EXPORT Parser
	{
	public:
		SN::SN_Variable Expression();
		Parser();
		virtual ~Parser();
	};
}

#endif // !defined(PARSER_H_INCLUDED)
