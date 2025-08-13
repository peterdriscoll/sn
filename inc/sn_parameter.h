#if !defined(SN_PARAMETER_H_INCLUDED)
#define SN_PARAMETER_H_INCLUDED

#pragma once

#include "sn_expression.h"

#include "exp_ctrl_sn.h"
#include <string>
#include <vector>


namespace SN
{
	class SN_Parameter
	{
	public:
		SN_Parameter();
		SN_Parameter(const SN_Expression &p_Value, const SN_Expression &p_Condition);
		SN_Parameter(const SN_Expression & p_Value);
		virtual ~SN_Parameter();

		SN_Expression &GetValue();
		SN_Expression &GetCondition();

		void UpdateValue();

	private:
		SN_Expression m_Value;
		SN_Expression m_Condition;
	};

	typedef std::vector<SN_Parameter> SN_ParameterList;
}

#endif // !defined(SN_PARAMETER_H_INCLUDED)
