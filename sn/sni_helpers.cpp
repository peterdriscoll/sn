#include "sni_helpers.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

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
		LOGGING(SN::LogContext context("SNI::PartialAssertValue ( " + SN::SN_Expression(p_Left).DisplaySN() + ", " + SN::SN_Expression(p_Right).DisplaySN() + " )"));

		bool ok = true;
		SNI_Error::PushHandler();
		if (SN::Is<SNI_Expression *>(p_Left))
		{
			ok |= p_Left.PartialAssertValue(p_Right).GetBool();
		}
		if (SN::Is<SNI_Expression *>(p_Right))
		{
			ok |= p_Right.PartialAssertValue(p_Left).GetBool();
		}
		SNI_Error::PopHandler();
		SN::SN_Error e(ok, false, "Partial assert left to right, and right to left both failed.");
		if (e.IsSignificantError())
		{
			SNI_CallRecord *callRecord = new SNI_CallRecord("Searching for equation definition.", NULL);
			LOGGING(callRecord->SetLogContext(context));
			e.GetSNI_Error()->AddNote(callRecord);
		}
		return e;
	}
}
