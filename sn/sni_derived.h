#if !defined(SNI_DERIVED_H_INCLUDED)
#define SNI_DERIVED_H_INCLUDED

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

	class SNI_Derived : public SNI_Value
	{
		PGC_CLASS(SNI_Derived);
	public:
		static SNI_Class * Class();

		SNI_Derived();
		virtual ~SNI_Derived();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual long GetPriority() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual size_t Hash() const;

		virtual bool IsFixed() const;

		virtual SN::SN_Value DoIsA(const SNI_Value * p_Parent) const;

		virtual void Fix();

	protected:
		virtual void PromoteMembers();

	private:
		static SNI_Class *m_Class;

		vector<SN::SN_Expression> m_Vector;
		bool m_Fixed;
	};
}

#endif // !defined(SNI_DERIVED_H_INCLUDED)
