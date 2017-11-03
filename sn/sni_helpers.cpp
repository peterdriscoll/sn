#include "sni_helpers.h"

#include "logcontext.h"
#include "sn.h"

#include "sni_null.h"
#include "sni_meta.h"
#include "sni_variable.h"
#include "sni_error.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ SNI_Expression * SNI_Helpers::Null = new SNI_Null();

	/*static*/ SN::SN_Error SNI_Helpers::PartialAssertValue(SN::SN_Expression p_Left, SN::SN_Expression p_Right)
	{
		SN::LogContext context("SNI::PartialAssertValue ( " + SN::SN_Expression(p_Left).DisplaySN() + ", " + SN::SN_Expression(p_Right).DisplaySN() + " )");

		bool ok = true;
		if (SN::Is<SNI_Expression *>(p_Left))
		{
			ok |= p_Left.PartialAssertValue(p_Right).GetBool();
		}
		if (SN::Is<SNI_Expression *>(p_Right))
		{
			ok |= p_Right.PartialAssertValue(p_Left).GetBool();
		}
		return ok;
	}
}
