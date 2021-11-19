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
	}

	void SN_OperatorVariables::Setup()
	{
		Link();

		{
			SN_LOCAL_INLINE(f);
			SN_LOCAL_INLINE(p);
			SN_LOCAL_INLINE(s);
			SN_LOCAL_INLINE(v);
			SN_LOCAL_INLINE(w);

			(ImperativeCall(f)(p)(s) == Local(v, Local(w, Let(
				v == f.BuildMeta(Short(1)).IsA(Value::Class()).If(StateValue(f, s), f(s))
				&& w == p.BuildMeta(Short(1)).IsA(Value::Class()).If(StateValue(p, v.State()), p(v.State()))
				, StateValue(v.Value()(w.Value()), w.State()))))).PartialAssert().Do();
		}
	}

	void SN_OperatorVariables::Link()
	{
		SN_LINK_VALUE(FunctionCall, skynet::FunctionCall, skynet::DefineType::Inline);
		SN_LINK_INLINE(ImperativeCall);
	}

	void SN_OperatorVariables::Cleanup()
	{
		ImperativeCall.SetValue(skynet::Null);
		SN_Domain::GetSNI_Domain()->Clear();
	}
}