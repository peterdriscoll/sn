#include "sn_cartesian.h"

#include "sn_value.h"
#include "sn_valueset.h"
#include "sn_parameter.h"
#include "sn_variable.h"
#include "sn_error.h"
#include "sn_manager.h"

#include "sni_cartesian.h"
#include "sni_functiondef.h"

#include "sn_pch.h"

namespace SN
{
	SN_Cartesian::SN_Cartesian(const SN_Expression &p_Expression, long p_Index, SNI_FunctionDef *p_FunctionDef)
		: m_Cartesian(new SNI::SNI_Cartesian(p_Expression, p_Index, p_FunctionDef))
	{};

	SN_Cartesian::SN_Cartesian(const SN_Cartesian &p_Cartesian)
		: m_Cartesian(p_Cartesian.GetSNI_Cartesian())
	{};

	SN_Cartesian::~SN_Cartesian()
	{
		delete m_Cartesian;
	};

	SN_Cartesian &SN_Cartesian::operator *(const SN_Cartesian &p_Cartesian)
	{
		(*m_Cartesian) * p_Cartesian;
		return *this;
	};

	SN_Error SN_Cartesian::ForEach(std::function<SN_Error(long p_Depth, SN_Expression *p_ParamList, SNI::SNI_World **p_WorldList, long p_CalcPos, long p_TotalCalc)> p_Action)
	{
		return m_Cartesian->ForEach(p_Action);
	};

	SN_Value SN_Cartesian::ForEachCall(const SNI::SNI_FunctionDef *p_FunctionDef)
	{
		return m_Cartesian->ForEachCall(p_FunctionDef);
	}

	SN_Error SN_Cartesian::ForEachUnify(const SNI::SNI_FunctionDef *p_FunctionDef)
	{
		return m_Cartesian->ForEachUnify(p_FunctionDef);
	}

	SNI::SNI_Cartesian * SN_Cartesian::GetSNI_Cartesian() const
	{
		SNI::SNI_Cartesian *cartesian = m_Cartesian;
		const_cast<SN_Cartesian *>(this)->m_Cartesian = NULL; // Handing the object over. The caller will delete.
		return cartesian;
	}
}
