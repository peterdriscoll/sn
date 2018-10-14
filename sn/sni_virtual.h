#if !defined(SNI_VIRTUAL_H_INCLUDED)
#define SNI_VIRTUAL_H_INCLUDED

#pragma once

#include <vector>

#include "sni_value.h"

namespace SN
{
	class SN_Expression;
}

namespace SNI
{
	class SNI_Class;
	class SNI_Expression;

	class SNI_Virtual : public SNI_Value
	{
		PGC_CLASS(SNI_Virtual);
	public:
		static SNI_Class * Class();

		SNI_Virtual();
		virtual ~SNI_Virtual();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual long GetPriority() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual size_t Hash() const;

		virtual bool IsFixed() const;

		virtual SN::SN_Value DoIsA(const SNI_Value * p_Parent) const;

		virtual void Fix();

		virtual SNI_Expression * Clone(SNI_Frame * p_Frame, bool & p_Changed);

		virtual SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel) const;

		virtual SN::SN_Expression Unify(SN::SN_ExpressionList * p_ParameterList);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Expression, bool p_Define = false);

	protected:
		virtual void PromoteMembers();

	private:
		static SNI_Class *m_Class;

		vector<SNI_Expression *> m_Vector;
		long m_DefineId;
		bool m_Fixed;
	};
}

#endif // !defined(SNI_VIRTUAL_H_INCLUDED)
