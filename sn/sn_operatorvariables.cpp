#include "sn_operatorvariables.h"

#include "sn_pch.h"

using namespace std;
using namespace skynet;
using namespace PGCX;

namespace SN
{
	SN_OperatorVariables::SN_OperatorVariables()
		: SN_Domain("OperatorVariables")
	{
		Setup();
	}

	void SN_OperatorVariables::Setup()
	{
		Link();
	}
	void SN_OperatorVariables::Link()
	{
		SN_LINK_VALUE(FunctionCall, skynet::FunctionCall, skynet::DefineType::Inline);
	}
}