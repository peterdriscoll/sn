#include "sni_functioncall.h"

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
	SNI_FunctionCall::SNI_FunctionCall()
	{
	}

	SNI_FunctionCall::~SNI_FunctionCall()
	{
	}

	string SNI_FunctionCall::GetTypeName() const
	{
		return "FunctionCall";
	}

	string SNI_FunctionCall::DisplayCpp() const
	{
		return "SN::FunctionCall";
	}

	string SNI_FunctionCall::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "call";
	}

	long SNI_FunctionCall::GetPriority() const
	{
		return 0;
	}

	string SNI_FunctionCall::GetOperator() const
	{
		return " ";
	}

	bool SNI_FunctionCall::Equivalent(SNI_Object * p_Other) const
	{
		if (dynamic_cast<SNI_FunctionCall *>(p_Other))
		{
			SNI_FunctionCall * l_function = dynamic_cast<SNI_FunctionCall *>(p_Other);
			return this == l_function;
		}
		return false;
	}

	SN::SN_Expression SNI_FunctionCall::DoEvaluate(long p_MetaLevel /* = 0 */) const
	{
		return this;
	}

	SN::SN_Expression SNI_FunctionCall::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		if (p_ParameterList->size() <= 2)
		{
			return SN::SN_Error(false, false, "FunctionCall operator must have two parameters.");
		}

		if (0 < p_MetaLevel)
		{
			SN::SN_Expression function = p_ParameterList->back();
			p_ParameterList->pop_back();
			SN::SN_Expression param = p_ParameterList->back(); 
			p_ParameterList->pop_back();

			return SN::SN_Function(SN::SN_Function(this, function.DoEvaluate(p_MetaLevel)), param.DoEvaluate(p_MetaLevel));
		}

		SN::SN_Expression function = p_ParameterList->back();
		p_ParameterList->pop_back();
		
		return function;
	}

	SN::SN_Expression SNI_FunctionCall::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		ASSERTM(p_ParameterList->size() > 1, "FunctionCall must have a parameter");

		SN::SN_Expression function = p_ParameterList->back();
		p_ParameterList->pop_back();
		SN::SN_Expression result = function.GetSNI_Expression()->PartialCall(p_ParameterList, p_MetaLevel);
		string s1 = result.DisplaySN();
		return result;
	}

	SN::SN_Expression SNI_FunctionCall::Unify(SN::SN_ExpressionList * p_ParameterList)
	{
		ASSERTM(p_ParameterList->size() > 1, "FunctionCall must have a parameter");

		SN::SN_Expression function = p_ParameterList->back();
		p_ParameterList->pop_back();
		return function;
	}

	SN::SN_Error SNI_FunctionCall::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		ASSERTM(p_ParameterList->size() > 1, "FunctionCall must have a parameter");

		SN::SN_Expression function = p_ParameterList->back().GetValue();
		p_ParameterList->pop_back();
		return function.GetSNI_Expression()->PartialUnify(p_ParameterList, p_Result);
	}

}
