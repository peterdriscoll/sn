#if !defined(SNI_CART_H_INCLUDED)
#define SNI_CART_H_INCLUDED

#pragma once

#include "sn_expression.h"

namespace SNI
{
	class SNI_World;

	class SNI_Cart
	{
	public:
		virtual SN::SN_Error ProcessValue(long p_Depth, const SN::SN_Expression & p_Param, SNI_World * p_World) = 0;
	};
}

#endif //SNI_CART_H_INCLUDED
