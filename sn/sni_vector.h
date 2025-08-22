#if !defined(SNI_VECTOR_H_INCLUDED)
#define SNI_VECTOR_H_INCLUDED

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

	class SNI_Vector : public SNI_Value
	{
		PGC_CLASS(SNI_Vector);
	public:
		static SNI_Class * Class();

		SNI_Vector();
		virtual ~SNI_Vector();

		virtual SNI_Expression *Copy() const;

		virtual std::string GetTypeName() const;
		virtual std::string DisplayCpp() const;
		virtual long GetPriority() const;
		virtual std::string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual size_t Hash() const;

		virtual bool IsFixed() const;

		virtual SN::SN_Error AssertIsAValue(const SNI_Value * p_Parent, SN::SN_Expression p_Result);
		virtual SN::SN_Value DoIsA(const SNI_Value * p_Parent) const;

		virtual SN::SN_Error AssertSubscriptValue(const SNI_Value *p_Index, SN::SN_Expression p_Result);
		virtual SN::SN_Value DoSubscriptCall(const SN::SN_Value &p_Index) const;
		virtual SN::SN_Value DoReverseSubscript(const SN::SN_Value & p_Result) const;

		virtual SN::SN_Value DoCountIf(SN::SN_Expression p_Value) const;
		virtual SN::SN_Value DoCountAll() const;

		virtual SN::SN_Value DoSum() const;

		virtual void Fix();

	protected:
		virtual void PromoteMembers();

	private:
		

		std::vector<SN::SN_Expression> m_Vector;
		bool m_Fixed;
	};
}

#endif // !defined(SNI_VECTOR_H_INCLUDED)
