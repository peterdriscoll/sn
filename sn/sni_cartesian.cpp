#include "sni_cartesian.h"

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
	SNI_Cartesian::SNI_Cartesian(const SN::SN_Expression &p_Expression, long p_Index, SNI_FunctionDef *p_FunctionDef)
		: m_Cartesian(NULL)
	{};

	SNI_Cartesian::~SNI_Cartesian()
	{
		delete [] m_Cartesian;
	};

	SNI_Cartesian &SNI_Cartesian::operator *(const SN::SN_Cartesian &p_Cartesian)
	{
		if (m_Cartesian)
			(*m_Cartesian) * p_Cartesian;
		else
			m_Cartesian = p_Cartesian.GetSNI_Cartesian();
		return *this;
	};

	long SNI_Cartesian::Depth()
	{
		return m_Cartesian ? 1 + m_Cartesian->Depth() : 1;
	};

	SN::SN_Error SNI_Cartesian::ForEach(std::function<SN::SN_Error(long p_Depth, SN::SN_Expression *p_ParamList, SNI_World **p_WorldList, long p_CalcPos, long p_TotalCalc)> p_Action)
	{
		long depth = Depth();

		return Loop(0, new SN::SN_Expression[depth], new SNI_World*[depth], -1, 0, p_Action);
	};

	SN::SN_Error SNI_Cartesian::Loop(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, long p_CalcPos, long p_TotalCalc, std::function<SN::SN_Error(long p_Depth, SN::SN_Expression *p_ParamList, SNI_World **p_WorldList, long p_CalcPos, long p_TotalCalc)> p_Action)
	{
		SNI_Cartesian *SNI_Cartesian = m_Cartesian;
		m_Evaluate.ForEach(
			[&p_ParamList, &p_WorldList, p_Depth, p_CalcPos, p_TotalCalc, SNI_Cartesian, p_Action](const SN::SN_Expression &p_Expression, SNI_World *p_World) -> SN::SN_Error
			{
				p_ParamList[p_Depth] = p_Expression;
				p_WorldList[p_Depth] = p_World;
				if (SNI_Cartesian)
				{
					SN::SN_Error e = SNI_Cartesian->Loop(p_Depth + 1, p_ParamList, p_WorldList, p_CalcPos, p_TotalCalc, p_Action);
					if (e.IsError())
					{
						return e;
					}
				}
				else
				{
					SN::SN_Error e = p_Action(p_Depth, p_ParamList, p_WorldList, p_CalcPos, p_TotalCalc);
					if (e.IsError())
					{
						return e;
					}
				}
				return skynet::OK;
			}
		);
		return skynet::OK;
	}
}
