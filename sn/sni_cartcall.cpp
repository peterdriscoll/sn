#include "sni_cartcall.h"

#include "logcontext.h"
#include "sn.h"
#include "sn_value.h"
#include "sn_valueset.h"
#include "sn_parameter.h"
#include "sn_variable.h"
#include "sn_error.h"
#include "sn_manager.h"

#include "sni_functiondef.h"
#include "sni_world.h"
#include "sni_worldset.h"
#include "sni_result.h"
#include "sni_helpers.h"
#include "sni_delayedprocessor.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_CartCall::SNI_CartCall(const SNI_FunctionDef *p_FunctionDef, long p_Depth, SN::SN_Expression * p_InputList)
		: m_FunctionDef(p_FunctionDef)
		, m_Depth(p_Depth)
		, m_InputList(p_InputList)
	    , m_ValueList(new SN::SN_Expression[p_Depth])
	    , m_WorldList(new SNI_World *[p_Depth])
	{
	}

	SNI_CartCall::~SNI_CartCall()
	{
		delete [] m_ValueList;
		delete [] m_WorldList;
	};

	SN::SN_Error SNI_CartCall::ProcessValue(long p_Depth, const SN::SN_Expression &p_Param, SNI_World*p_World)
	{
		SN::SN_Error e(true);
		m_ValueList[p_Depth] = p_Param;
		m_WorldList[p_Depth] = p_World;
		if (p_Depth < m_Depth-1)
		{
			// e = m_InputList[p_Depth+1].GetSNI_Expression()->ForEachCart(p_Depth+1, this);
		}
		else
		{
			size_t maxCard = SN::SN_Manager::GetTopManager().MaxCardinalityCall();
			size_t card = m_FunctionDef->CardinalityOfCall(p_Depth + 1, m_ValueList);
			if (maxCard < card)
			{
				e = SN::SN_Error(true, true, "Cardinality "+ to_string(card)+ " exceeds maximum cardinality "+to_string(maxCard)+".");
			}
			else
			{
				e = m_FunctionDef->CallElement(p_Depth + 1, m_ValueList, m_WorldList, m_Result);
			}
		}
		return e;
	}

	SN::SN_Error SNI_CartCall::ForEach()
	{
		return SN::SN_Error(); // m_InputList[0].GetSNI_Expression()->ForEachCartCall(0, this);
	}
}