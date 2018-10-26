#if !defined(SNI_IF_H_INCLUDED)
#define SNI_IF_H_INCLUDED

#pragma once

#include "sni_functiondef.h"

namespace SNI
{
	class SNI_If : public SNI_FunctionDef
	{
		PGC_CLASS(SNI_If)
	public:
		SNI_If();
		virtual ~SNI_If();

		/// @cond
		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;
		virtual string GetOperator() const;
		virtual long GetNumParameters() const;
		virtual string DisplayCall(long priority, SNI_DisplayOptions & p_DisplayOptions, SN::SN_ExpressionList * p_ParameterList) const;
		/// @endcond

		virtual SN::SN_Expression CallArray(SN::SN_Expression * p_ParamList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression UnifyArray(SN::SN_Expression * p_ParameterList);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Expression, bool p_Define = false);
		virtual SN::SN_Error PartialUnifyInternal(SN::SN_Expression &p_Condition, SN::SN_Expression &p_PositiveCase, SN::SN_Expression &p_NegativeCase, SN::SN_Expression &p_Result);

		size_t CardinalityOfCall(long p_NumParams, SN::SN_Expression * p_ParameterList) const;

	protected:
		virtual SN::SN_Value CallElement(long p_Depth, SN::SN_Expression * p_ParamList, SNI_World ** p_WorldList, SN::SN_ValueSet p_Result) const;
	};
}

#endif // !defined(SNI_IF_H_INCLUDED)
