#include "sni_attributedef.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sn_parameter.h"
#include "sn_error.h"

#include "sni_helpers.h"
#include "sni_null.h"
#include "sni_error.h"
#include "sni_result.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_AttributeDef::SNI_AttributeDef()
	{
	}

	SNI_AttributeDef::~SNI_AttributeDef()
	{
	}

	string SNI_AttributeDef::GetTypeName() const
	{
		return "IsA";
	}

	string SNI_AttributeDef::DisplayCpp() const
	{
		return "SN::IsA";
	}

	string SNI_AttributeDef::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "IsA";
	}

	long SNI_AttributeDef::GetPriority() const
	{
		return 0;
	}

	bool SNI_AttributeDef::IsKnownValue(const SN::SN_Expression &p_Param, long) const
	{
		return p_Param.IsKnownTypeValue();
	}
}
