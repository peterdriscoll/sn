#include "sni_file.h"

#include "sni_helpers.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_File::SNI_File()
	{

	}

	SNI_File::~SNI_File()
	{

	}

	string SNI_File::GetTypeName() const
	{
		return "File";
	}

	string SNI_File::DisplayCpp() const
	{
		return "SN::File";
	}

	string SNI_File::DisplaySN(long /*priority*/, SNI_VariablePointerList & /*p_DisplayVariableList*/) const
	{
		return "File";
	}

	long SNI_File::GetPriority() const
	{
		return 1;
	}

	SN::SN_Value SNI_File::PrimaryFunctionValue(const SN::SN_Value &p_Param) const
	{
		return p_Param.GetSNI_Value()->DoFile();
	}

	SN::SN_Expression SNI_File::PrimaryFunctionExpression(const SN::SN_Expression &p_Param) const
	{
		return p_Param.File();
	}

	SN::SN_Value SNI_File::InverseFunctionValue(const SN::SN_Value &p_Param) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_File::InverseFunctionExpression(const SN::SN_Expression & p_Param) const
	{
		return skynet::Null;
	}

	SN::SN_Expression SNI_File::Unify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result)
	{
		SN::LogContext context("SNI_File::Unify ( " + DisplayPmParameterList(p_ParameterList) + " = " + p_Result.DisplaySN() + " )");
		SN::SN_String name = (*p_ParameterList)[0].GetValue().Evaluate();
		if (!name.IsNull())
		{
			SN::SN_Value file = name.GetSNI_String()->DoFile();
			if (file.IsNull())
			{
				SN::SN_String contents = p_Result.Evaluate();
				if (!contents.IsNull())
				{
					name.GetSNI_String()->DoWriteFile(contents);
					return SN::SN_Error(true);
				}
			}
			else
			{
				return p_Result.AssertValue(file);
			}
		}
		SNI_DelayedProcessor::GetProcessor()->Delay(SN::SN_FunctionDef(dynamic_cast<SNI_FunctionDef*>(this)), p_ParameterList, p_Result);
		return SN::SN_Error(true);
	}
}
