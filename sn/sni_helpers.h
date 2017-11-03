#if !defined(SNI_HELPERS_H_INCLUDED)
#define SNI_HELPERS_H_INCLUDED

#pragma once

namespace SN
{
	class SN_Expression;
	class SN_Error;
}

namespace SNI
{
	class SNI_Expression;

	class SNI_Helpers
	{
	public:
		static SN::SN_Error PartialAssertValue(SN::SN_Expression p_Left, SN::SN_Expression p_Right);

		static SNI_Expression * Null;
	};
}

#endif //SNI_HELPERS_H_INCLUDED
