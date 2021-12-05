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
				v == f.BuildMeta(Short(1)).IsA(Value::Class()).If(StateValue(f, s), Function(f, s))
				&& w == p.BuildMeta(Short(1)).IsA(Value::Class()).If(StateValue(p, v.State()), Function(p, v.State()))
				, StateValue(Function(v.Value(), w.Value()), w.State()))))).PartialAssert().Do();
		}

		{
			SN_LOCAL_INLINE(l);
			SN_LOCAL_INLINE(r);
			SN_LOCAL_INLINE(s);
			SN_LOCAL_INLINE(v);
			SN_LOCAL_INLINE(w);

			(ImperativeAssign(l)(r)(s) == Local(v, Local(w, Let(
				v == l.BuildMeta(Short(1)).IsA(Value::Class()).If(StateValue(l, s), Function(l, s))
				&& w == r.BuildMeta(Short(1)).IsA(Value::Class()).If(StateValue(r, v.State()), Function(r, v.State()))
				, Function(Function(Function(skynet::Assign, v.Value()), w.Value()), w.State()))))).PartialAssert().Do();
		}
	}

	void SN_OperatorVariables::Link()
	{
		SN_LINK_VALUE(FunctionCall, skynet::FunctionCall, skynet::DefineType::Inline);
		SN_LINK_VALUE(Assign, skynet::Assign, skynet::DefineType::Inline);
		SN_LINK_INLINE(ImperativeCall);
		SN_LINK_INLINE(ImperativeAssign);
	}

	void SN_OperatorVariables::Cleanup()
	{
		ImperativeCall.SetValue(skynet::Null);
		ImperativeAssign.SetValue(skynet::Null);
		SN_Domain::GetSNI_Domain()->Clear();
	}
}