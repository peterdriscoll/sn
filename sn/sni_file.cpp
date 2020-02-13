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

	string SNI_File::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
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

	SN::SN_Expression SNI_File::UnifyArray(SN::SN_Expression * p_ParamList, const SNI_Expression *p_Source)
	{
		SN::SN_String name = p_ParamList[1].DoEvaluate();
		if (!name.IsNull())
		{
			SN::SN_Value file = name.GetSNI_String()->DoFile();
			if (file.IsNull())
			{
				SN::SN_String contents = p_ParamList[0].DoEvaluate();
				if (!contents.IsNull())
				{
					name.GetSNI_String()->DoWriteFile(contents);
					return skynet::OK;
				}
			}
			else
			{
				return p_ParamList[0].AssertValue(file);
			}
		}
		SNI_Thread::GetThread()->GetProcessor()->Delay(SN::SN_FunctionDef(dynamic_cast<SNI_FunctionDef*>(this)), GetNumParameters(), p_ParamList, p_Source);
		return skynet::OK;
	}
}
